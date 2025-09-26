#include <stdint.h>
#include <stdbool.h>
#include "usb.h"
#include "stm32.h"
#include "cdc.h"
#include "init.h"
#include "bridge.h"
#include "flash.h"
#include "u2a.h"
#include "watchdog.h"

volatile uint32_t TestMode = 0;

void TX_CB(char c) {
    CDC_TX_CHAR(c);
}

unsigned char PrintVal(char* pref, int32_t val) {
    CDC_TX(pref);
    unsigned char l = itoaLTRCB(val, TX_CB);
    CDC_TX_CHAR(' ');
    return l + strlen(pref) + 1;
}

void ProcessCMD(uint8_t *cmd, uint8_t size) {
//    static uint32_t inc = 0;
//    switch(cmd[0]) {
//    case 0: //Binary Command
//        switch(*(++cmd)) {
//        case 0: //Set PWM Duty
//            utoaLTRCB(SetPWMDuty(*(++cmd) * MAX_EFFORT / 100), TX_CB);
//            CDC_TX("% OK\r\n");
//            break;
//        case 1: //Set Direction
//            SetDirection(*(++cmd));
//            CDC_TX((*cmd) ? "REV OK\r\n" : "FWD OK\r\n")
//            break;
//        case 2: //Set Enabled
//            SetEnabled(*(++cmd));
//            CDC_TX((*cmd) ? "EN OK\r\n" : "DIS OK\r\n")
//            break;
//        case 3: //Get Status
//            GetStatus(TX_CB);
//            break;
//        case 4: //Set Target
//            inc = 0;
//            targetPos = (*(++cmd) * MAX_ENC_POS) + zero_pos;
//            utoaLTRCB(targetPos, TX_CB);
//            CDC_TX(" Pos OK\r\n");
//            enPid = tick + pid_time;
//            break;
//        case 5: //Get Pos
//            PrintPos(0);
//            break;
////        case 6: //Increment Target
////            targetPos += *(++cmd) * MAX_ENC_POS;
////            utoaLTRCB(targetPos, TX_CB);
////            CDC_TX(" Pos OK\r\n");
////            enPid = tick + pid_time;
////            break;
//
//        case 7: //Inc Target
//            inc += *(++cmd);
//            targetPos = zero_pos + (inc * MAX_ENC_POS);
//            utoaLTRCB(targetPos, TX_CB);
//            CDC_TX(" Pos OK\r\n");
//            enPid = tick + pid_time;
//            break;
//
//        case 8: //Permanent PidEnable
//            enPid = tick + 1000*3600;
//            break;
//        }
//        break;
//    case 'K': // Coefficient
//        switch(cmd[2]) {
//        case '=':
//                if (cmd[1] == 'p')
//                    pid->Kp = parseFloat(&cmd[3]);
//                else if (cmd[1] == 'i')
//                    InitKi(pid, parseFloat(&cmd[3]), MAX_EFFORT);
//                else
//                    pid->Kd = parseFloat(&cmd[3]);
//        case '?':
//                CDC_TX_CHAR(cmd[0]);
//                CDC_TX_CHAR(cmd[1]);
//                CDC_TX_CHAR('=');
//                ftoaCB(cmd[1] == 'p' ? pid->Kp : cmd[1] == 'i' ? pid->Ki : pid->Kd, TX_CB);
//                CDC_TX("\r\n");
//            break;
//        }
//        break;
//    }
}

volatile uint32_t mode = 0; // 0 - manual, 1 - center, 2 - auto
volatile uint32_t CCR_value = 0;
void ADC1_2_IRQHandler() {
#define AVG_CNT 512
    static int32_t inc = 3;
    static uint32_t sum = 0;
    static uint32_t num = 0;
    sum += ADC1->DR;
    num++;
    if (num == AVG_CNT) {
        //Min Pos    = 283	- 0.52mS
        //Max Pos    = 1308 - 2.43mS
        //Center Pos = 808  - 1.50mS

        switch(mode) {
        case 1:
            CCR_value = 808;
            break;
        case 2:
            if ((CCR_value + inc) < 283 || (CCR_value + inc) > 1308)
                inc = -inc;
            CCR_value += inc;
            break;
        default:
            CCR_value = 283 + ((sum / AVG_CNT) >> 2);
        }

        sum = 0;
        num = 0;

        //CCR_value = 283;
    }
}

void TIM4_IRQHandler() {
  TIM4->SR = 0;
  TIM4->CCR4 = CCR_value;
}

volatile uint32_t min_cnt = 10000000;
volatile uint32_t max_cnt = 0;
void EXTI2_IRQHandler() {
    uint32_t cnt = TIM2->CNT;
    uint32_t cnt1 = TIM2->CNT;
    cnt1 -= cnt;

//    GPIOA->ODR = 1 << 4;
//    GPIOA->ODR = 0;

    EXTI->PR = EXTI_PR_PR2;
//
//
//    static uint32_t prev_cnt = 0;
//    if (GPIOA->IDR & 0x04) {
//        //start timer
//        prev_cnt = cnt;
//    } else {
//        //calc time
//        uint32_t time = cnt - prev_cnt;
//        if (cnt < prev_cnt)
//            time += 0x10000;
//
        min_cnt = MIN(cnt1, min_cnt);
        max_cnt = MAX(cnt1, max_cnt);
//    }

}


// B9 -> servo out signal
// A3 -> ADC in
//  {A1, A0} - mode
//   0   0 - Manual
//   0   1 - Center
//   1   0 - Auto
//   1   1 - Manual

//PA3 - ADC
//PB9 - Output Servo Control

void main(void) {
    usbd_device* udev = InitCDC();
    __enable_irq();

    CCR_value = 283;
    ADC1->CR2 |= ADC_CR2_ADON; //Start continuous conversions
    TIM4->CR1 |= TIM_CR1_CEN;    //Start timer

    for(;;) {
        static uint32_t pa = 3;
        uint32_t idr = GPIOA->IDR & 3;
        if (idr != pa) {
            uint32_t delay = tick + 250;
            while(delay < tick) __WFI(); // de-bounce timeout

            if (idr == (GPIOA->IDR & 3)) {
                pa = idr;
                mode = ~idr & 3;
            }
        }

        static uint32_t next = 0;
        switch(mode) {
        case 1:
            LED_ON;
            break;
        case 2:
            if (tick >= next) {
                next = tick + 1000;
                LED_TOGGLE;
            }
            break;
        default:
            LED_OFF;
        }

//        static uint32_t next = 1000;
//        if (tick >= next) {
//            next += 1000;
//            if (USBIsAvailable()) {
//                LED_TOGGLE;
//            } else
//                LED_OFF;
//        }
        __WFI();
    }
}
