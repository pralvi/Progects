#include "init.h"
#include "stm32f10x_rcc.h"
#include "usb.h"
#include "cdc.h"
#include "bridge.h"

uint8_t tester_mode;

uint8_t device_buffer[2048] = {0};

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


void Clear_Buffer(void)
{
   for (uint16_t i = 0; i < 2048 ; i++)
        device_buffer[i] = 0;
}





void Delay_us2(uint16_t Delay)
{
  uint64_t tickstart = GetMicroseconds();
  while ((GetMicroseconds() - tickstart) <= Delay)
  {
  }
}

void Delay_us(uint16_t delay)
{
    uint16_t counter = delay;
    while(counter)
    {
        counter--;
    }
    return;
    TIM1->PSC = (F_CPU / 1000000) - 1;      // установка предделителя таймера (период 1 us)
    TIM1->ARR = delay;                      // количество периодов
    TIM1->EGR = TIM_EGR_UG;                 // генерируем событие обновления
    TIM1->CR1 = TIM_CR1_CEN|TIM_CR1_OPM;    // включаем режим одного импульса
    while ((TIM1->CR1 & TIM_CR1_CEN)!=0);   // ждем пока тикает
}

//static void init_rcc_36 (void) {
//    //Set SysClock to 36Mhz from HSI
// // enable prefetch buffer and set flash latency 2WS for 72MHz
//    _BMD(FLASH->ACR, FLASH_ACR_PRFTBE| FLASH_ACR_LATENCY, FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2);
//    //Enable Internal HS 8Mhz
//    _BMD(RCC->CR, RCC_CR_HSION, RCC_CR_HSION);
//    _WBS(RCC->CR, RCC_CR_HSIRDY); //Wait HSI ready
//
//    _BMD(RCC->CFGR,
//          RCC_CFGR_PLLMULL | RCC_CFGR_PPRE1,
//                //PLL Source is HSI
//        RCC_CFGR_PLLMULL9      //Multiply input clock by 9
//       | RCC_CFGR_PPRE1_DIV1);  //APB1 prescaler is 2 (36MHz)
//                                //APB1 frequency should be between 10MHz and 36MHz in case if USB use
//    //Enable PLL
//    _BST(RCC->CR, RCC_CR_PLLON);
//    _WBS(RCC->CR, RCC_CR_PLLRDY); //Wait HSE ready
//
//    //Select PLL
//    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
//    _WBS(RCC->CFGR, RCC_CFGR_SWS_PLL); // Wait PLL selected
//
//    //Configure SysTick Timer
//    SysTick->LOAD = SysTimerTick;		// Load SysTimer period
//    SysTick->VAL = SysTimerTick;		// Clear SysTimer counter
//
//    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk
//        | SysTick_CTRL_ENABLE_Msk;// Start timer
//
//
//
//}


static void init_rcc (void) {
    //Set SysClock to 72Mhz from HSE
    // enable prefetch buffer and set flash latency 2WS for 72MHz
    _BMD(FLASH->ACR, FLASH_ACR_PRFTBE| FLASH_ACR_LATENCY, FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2);

    //Enable HSE
    _BMD(RCC->CR, RCC_CR_HSEON, RCC_CR_HSEON);
    _WBS(RCC->CR, RCC_CR_HSERDY); //Wait HSE ready

    _BMD(RCC->CFGR,
         RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL | RCC_CFGR_PPRE1 | RCC_CFGR_ADCPRE,
         RCC_CFGR_PLLSRC        //PLL Source is HSE
       | RCC_CFGR_PLLMULL9      //Multiply input clock by 9
       | RCC_CFGR_PPRE1_DIV2    //APB1 prescaler is 2 (36MHz)
                                //APB1 frequency should be between 10MHz and 36MHz in case if USB use
       | RCC_CFGR_ADCPRE_DIV8   //ADCCLK = PCLK2 divided by 8 (9MHz)
       );

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
    SET_PIN(LED_PIN , 1);
    CFG_PIN(LED_PIN, OUTPUT, PUSHPULL | SLOPE_2MHZ);

}

static void ParseCommand(uint8_t c) {
    static uint8_t buf[CMD_MAX_SIZE];

    static uint8_t cmdIdx = 0;

    if ((c == 0x0A) || (c == 0x0D)) {
        if (cmdIdx > 0) {
            buf[cmdIdx] = 0;
            ExecuteTextCommand((char*)buf, cmdIdx);
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

    //usbd_device* udev = InitCDC();
    InitCDC();

    __enable_irq();

    tester_mode = MODE_NONE;

}

void SystemInit(void) {
    init_rcc();

}

