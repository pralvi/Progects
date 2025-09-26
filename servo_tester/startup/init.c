#include "init.h"
#include "gpio.h"
#include "cdc.h"
#include "spi.h"

volatile uint32_t tick = 0;
void SysTick_Handler() {
    tick++;
}

static void init_rcc (void) {
    //Set SysClock to 72Mhz from HSE
    // enable prefetch buffer and set flash latency 2WS for 72MHz
    _BMD(FLASH->ACR, FLASH_ACR_PRFTBE| FLASH_ACR_LATENCY, FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2);

    //Enable HSE
    _BMD(RCC->CR, RCC_CR_HSEON, RCC_CR_HSEON);
    _WBS(RCC->CR, RCC_CR_HSERDY); //Wait HSE ready

    _BMD(RCC->CFGR,
         RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL | RCC_CFGR_PPRE1,
         RCC_CFGR_PLLSRC        //PLL Source is HSE
       | RCC_CFGR_PLLMULL9      //Multiply input clock by 9
       //| RCC_CFGR_PLLMULL4      //Multiply input clock by 4
       | RCC_CFGR_PPRE1_DIV2);  //APB1 prescaler is 2 (36MHz)
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

void init_adc() {
    // Enable required APB2 clocks
    _BMD(RCC->APB2ENR, RCC_APB2ENR_ADC1EN_Msk, RCC_APB2ENR_ADC1EN); // ADC1 clock enable

    //Configure ADC pin
    _BMD(RCC->APB2ENR, RCC_APB2ENR_IOPAEN_Msk, RCC_APB2ENR_IOPAEN); // IO port C clocks enable
    //CFG_PIN(ADC_PIN, INPUT, FLOATING);    //Default state. Can be commented out

    ADC1->CR1 |= ADC_CR1_EOCIE; //End of Conversion interrupt enabled

    ADC1->CR2 |= ADC_CR2_ADON //Turn ADC on from Power Down
              |  ADC_CR2_CONT //Continuous conversions
      ;

    ADC1->SQR3 |= 3; //Convert channel #3 (PA3)
    ADC1->SMPR2 |= ADC_SMPR2_SMP3_2 | ADC_SMPR2_SMP3_1 | ADC_SMPR2_SMP3_0; //Set sample rate (239.5 cycles), Tconv = 239.5 + 12.5 = 252 cycles (35.7 KHz on 9 MHz ADCCLK )

    NVIC_SetPriority(ADC1_2_IRQn, 3); // Set lowest priority
    NVIC_EnableIRQ(ADC1_2_IRQn); //Enable ADC1 and ADC2 global Interrupt
}

void calibrate_adc() {
   //Before starting a calibration, the ADC must have been in power-on state (ADON bit = ‘1’) for at least two ADC clock cycles.
   ADC1->CR2 |= ADC_CR2_CAL;          //Start ADC calibration
   while (ADC1->CR2 & ADC_CR2_CAL);   //Wait calibration done
}

//Init TIM4 PWM
void init_tim() {
    // Enable required APB2 clocks
    _BMD(RCC->APB2ENR, RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN,
         RCC_APB2ENR_AFIOEN     // Alternate function IO clock enable
         | RCC_APB2ENR_IOPBEN   // IO port B clock enable
                 );

    // Enable required APB1 clocks
    _BMD(RCC->APB1ENR, RCC_APB1ENR_TIM4EN_Msk, RCC_APB1ENR_TIM4EN);     // Timer 4 clock enable

    //Output pins
    CFG_PIN(B, 9, ALTERNATE, PUSHPULL | SLOPE_2MHZ);

    //Configure timer
    TIM4->CR1  |= TIM_CR1_ARPE;  // Preload Mode

    //CH4 Configure
    TIM4->CCMR2 = TIM_CCMR2_OC4M ^ TIM_CCMR2_OC4M_0 // Set up to use PWM mode 1.
                                | TIM_CCMR2_OC4PE   // CH4 Preload Enabled
                  ;

    TIM4->CCER |= TIM_CCER_CC4E; // Output enabled

//    TIM4->PSC  = 0;
//    TIM4->ARR  = 1;
//    TIM4->CCR4 = 1;

    // PWM Freq = 50Hz
    TIM4->PSC  = 133;
    TIM4->ARR  = 10777;
    TIM4->CCR4 = 0;
    TIM4->EGR = TIM_EGR_UG; //Update shadow registers


    TIM4->DIER |= TIM_DIER_UIE;  // Update interrupt enable
    NVIC_EnableIRQ(TIM4_IRQn);

    DBGMCU->CR |= DBGMCU_CR_DBG_TIM4_STOP;
}

#define USARTBAUD(baud) (uint32_t)((uint32_t)((F_CPU) / (baud) / 16) * 16 + (((F_CPU) / (baud)) % 16))
void init_usart1(void) {
    // Enable required APB2 clocks
    _BMD(RCC->APB2ENR, RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN,
                     RCC_APB2ENR_AFIOEN     // Alternate function IO clock enable
                     | RCC_APB2ENR_IOPAEN   // IO port A clock enable
                     | RCC_APB2ENR_USART1EN  //USART1 clock enable
                     );
    //Configure USART1
    USART1->BRR = USARTBAUD(115200);

    _BST(USART1->CR1, USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE); // USART1 ON, TX ON, RX ON, RXNE Interrupt enabled
    CFG_PIN(A, 9, ALTERNATE, PUSHPULL | SLOPE_2MHZ); //PA9 - USART1 TX Pin configuration
    NVIC_EnableIRQ(USART1_IRQn);
}

void init_gpio(void) {
    // Enable required APB2 clocks
    _BMD(RCC->APB2ENR, RCC_APB2ENR_IOPAEN_Msk | RCC_APB2ENR_IOPCEN_Msk, RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN); // IO port C clocks enable

    CFG_PIN(A, 0, INPUT, PULLUP);
    CFG_PIN(A, 1, INPUT, PULLUP);


    CFG_PIN(A, 4, OUTPUT, PUSHPULL | SLOPE_50MHZ);


    CFG_PIN(A, 8, ALTERNATE, PUSHPULL | SLOPE_50MHZ);
    //RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;
    RCC->CFGR |= RCC_CFGR_MCO_PLLCLK_DIV2;


    CFG_PIN(LED_PIN, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    LED_OFF;
}

void init_exti() {
    // Enable required APB1 clocks
    _BMD(RCC->APB1ENR, RCC_APB1ENR_TIM2EN_Msk, RCC_APB1ENR_TIM2EN);     // Timer 2 clock enable

    TIM2->PSC  =  4;
    TIM2->ARR  =  0xFFFF;
    TIM2->CR1 |= TIM_CR1_CEN;    //Start timer
    DBGMCU->CR |= DBGMCU_CR_DBG_TIM2_STOP;

    //Configure feedback EXTIs
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI2_PA;     // PA2 -> EXTI2

    EXTI->RTSR |= EXTI_RTSR_TR2; //Raising trigger enables
    EXTI->FTSR |= EXTI_FTSR_TR2; //Falling trigger enables

    //Enable interupts
    EXTI->IMR |= EXTI_IMR_MR2;

    //Enable EXRI IRQs
    NVIC_EnableIRQ (EXTI2_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 0xF);
    NVIC_SetPriority(ADC1_2_IRQn, 0xF);
}

void SystemInit(void) {
    init_rcc();
    init_adc();
    init_gpio();
    //init_exti();
    //init_usart1();
    //InitSPI(1);
    init_tim();
    calibrate_adc();
}
