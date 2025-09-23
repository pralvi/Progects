#include "init.h"

struct usart_tx_buffer usart_tx_buf;
struct usart_tx_buffer rpi_tx_buf;

volatile uint32_t tick = 0;
void SysTick_Handler() {
    tick++;
}

uint64_t GetMicroseconds() {
    register uint32_t t = tick;
    register uint32_t val = SysTick->VAL;
    if (SCB->ICSR & SCB_ICSR_PENDSTSET_Msk) { //Turnover happened
        val = SysTick->VAL;
        t++;
    }

    return (uint64_t)t * 1000 + (SysTimerTick - val) / 36;
}

void Delay_us(uint32_t Delay)
{
  uint64_t tickstart = GetMicroseconds();
  while ((GetMicroseconds() - tickstart) <= Delay)
  {
  }
}



static void init_rcc (void) {
    //Set SysClock to 72Mhz from HSE
    // enable prefetch buffer and set flash latency 2WS for 72MHz
    _BMD(FLASH->ACR, FLASH_ACR_PRFTBE| FLASH_ACR_LATENCY, FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2);

    //Enable Internal HS 8Mhz
    _BMD(RCC->CR, RCC_CR_HSION, RCC_CR_HSION);
    _WBS(RCC->CR, RCC_CR_HSIRDY); //Wait HSI ready

    _BMD(RCC->CFGR,
          RCC_CFGR_PLLMULL | RCC_CFGR_PPRE1,
                //PLL Source is HSI
        RCC_CFGR_PLLMULL9      //Multiply input clock by 9
       | RCC_CFGR_PPRE1_DIV1);  //APB1 prescaler is 2 (36MHz)
                                //APB1 frequency should be between 10MHz and 36MHz in case if USB use
    //Enable PLL
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY); //Wait HSE ready

    //Select PLL
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WBS(RCC->CFGR, RCC_CFGR_SWS_PLL); // Wait PLL selected

    //Configure SysTick Timer
    SysTick->LOAD = SysTimerTick;		// Load SysTimer period
    SysTick->VAL = SysTimerTick;		// Clear SysTimer counter

    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk
        | SysTick_CTRL_ENABLE_Msk;// Start timer
}

void init_gpio(void) {

    // Enable required APB2 clocks
    _BMD(RCC->APB2ENR, RCC_APB2ENR_AFIOEN_Msk | RCC_APB2ENR_IOPAEN_Msk | RCC_APB2ENR_IOPBEN_Msk | RCC_APB2ENR_IOPCEN_Msk, RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN); // IO ports A, B, C clocks enable

    // Disable JTAG and OSC_IN/OUT to extend the number of available GPIO pins. SWD remains enabled
    AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE | AFIO_MAPR_PD01_REMAP;

//    SET_PIN(LED_STATUS, 0);
//    SET_PIN(LED_LOCK, 0);
//    SET_PIN(LED_UNLOCK, 0);
//    SET_PIN(LED_ERROR, 0);
//    SET_PIN(POWER_RFID, 0);
//
//    CFG_PIN(LED_STATUS, OUTPUT, PUSHPULL | SLOPE_2MHZ);
//    CFG_PIN(LED_LOCK, OUTPUT, PUSHPULL | SLOPE_2MHZ);
//    CFG_PIN(LED_UNLOCK, OUTPUT, PUSHPULL | SLOPE_2MHZ);
//    CFG_PIN(LED_ERROR, OUTPUT, PUSHPULL | SLOPE_2MHZ);
//    CFG_PIN(POWER_RFID, OUTPUT, PUSHPULL | SLOPE_2MHZ);

}

uint8_t ParseUARTCommand(char* cmd, uint8_t size) {

    uint8_t response[8];
    ExecuteTextCommand(cmd, size, response);
    return 1;
}

static void ParseCommand(uint8_t c) {
    static uint8_t buf[CMD_MAX_SIZE];

    static uint8_t cmdIdx = 0;

    if ((c == 0x0A) || (c == 0x0D)) {
        if (cmdIdx > 0) {
            buf[cmdIdx] = 0;
            ParseUARTCommand((char*)buf, cmdIdx);
            //Process_UART_CMD(buf, cmdIdx);
        }
        cmdIdx = 0;
    } else {
        buf[cmdIdx++] = c;

        if (cmdIdx >= CMD_MAX_SIZE)
            cmdIdx = 0;
    }
}

static uint8_t USART_Handler(USART_TypeDef* usart, uint8_t event, uint8_t* data) {
    if (event & USART_RX_EVENT)
        ParseCommand(*data);

    if (event & USART_TX_EVENT) {
        if (RBUF_IS_EMPTY(usart_tx_buf))
            return 0;

        RBUF_READ(usart_tx_buf, *data);
    }

    return 1;
}

void InitHardware() {
    init_gpio();
    init_usart(USART1, 115200, USART_Handler);
    __enable_irq();

}

void SystemInit(void) {
    init_rcc();

}

