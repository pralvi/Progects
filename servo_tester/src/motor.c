#include "motor.h"
#include "init.h"
#include "u2a.h"

volatile uint32_t *pwmGen = 0;

void InitPWM() {
    // Enable required APB1 clocks
    _BMD(RCC->APB1ENR, RCC_APB1ENR_TIM2EN_Msk, RCC_APB1ENR_TIM2EN);     // Timer 2 clock enable

    // Enable required APB2 clocks
    _BMD(RCC->APB2ENR, RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN,
         RCC_APB2ENR_AFIOEN     // Alternate function IO clock enable
         | RCC_APB2ENR_IOPAEN   // IO port A clock enable
                 );

    _BMD(RCC->APB1ENR, RCC_APB1ENR_TIM2EN_Msk, RCC_APB1ENR_TIM2EN);     // Timer 2 clock enable

    //Output pins
    CFG_PIN(FWD_PIN, ALTERNATE, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(REV_PIN, ALTERNATE, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(EN_PIN, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(EN2_PIN, OUTPUT, PUSHPULL | SLOPE_2MHZ);

    //Configure timer
    //TIM2 - Duty PWM
    TIM2->CR1  |= TIM_CR1_ARPE;  // Preload Mode

    //CH2 Configure
    TIM2->CCMR1 = TIM_CCMR1_OC2M ^ TIM_CCMR1_OC2M_0 // Set up to use PWM mode 1.
                                | TIM_CCMR1_OC2PE;  // CH2 Preload Enabled
    //CH3 Configure
    TIM2->CCMR2 = TIM_CCMR2_OC3M ^ TIM_CCMR2_OC3M_0 // Set up to use PWM mode 1.
                                | TIM_CCMR2_OC3PE;  // CH3 Preload Enabled

    TIM2->CCER |= TIM_CCER_CC2E | TIM_CCER_CC3E; // Output enabled

    // PWM Freq = 5KHz
    TIM2->PSC = 0;
    TIM2->ARR = 14399;

    TIM2->CCR2 = 0;
    TIM2->CCR3 = 0;

    pwmGen = &TIM2->CCR2;

    TIM2->CR1 |= TIM_CR1_CEN;    //Start timer
}

__WEAK uint_fast16_t ValidateDuty(uint32_t duty) {
    return MIN(TIM2->ARR, duty);
}

uint8_t SetPWMDuty(uint32_t duty) {
    *pwmGen = ValidateDuty(duty);
    return *pwmGen;
}

uint8_t SetDirection(uint8_t dir) {
    volatile uint32_t *gen = (dir) ? &TIM2->CCR3 : &TIM2->CCR2;
    if (pwmGen != gen) {
        *pwmGen = 0;
        pwmGen = gen;
    }
    return dir;
}

uint8_t SetEnabled(uint8_t en) {
    if (en) {
        SET_PIN(EN_PIN, 1);
        SET_PIN(EN2_PIN, 1);
    } else {
        SET_PIN(EN_PIN, 0);
        SET_PIN(EN2_PIN, 0);
        SetPWMDuty(0);
    }
    return en;
}

void GetStatus(void CB(char)) {
    CB(GET_PIN(EN_PIN) ? 'E' : 'D');
    CB((pwmGen == &TIM2->CCR2) ? 'F' : 'R');
    utoaLTRCB(*pwmGen, CB);
    CB('\r');
    CB('\n');
}

uint8_t IsMoving() {
    return *pwmGen;
}

uint8_t IsForward() {
    return pwmGen == &TIM2->CCR2;
}
