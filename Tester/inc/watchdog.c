#include "watchdog.h"

void IWDG_InitEx(uint32_t us) {
    if (us < 100)
        us = 100;
    else if (us > 26214400)
        us = 26214400;
    
    uint32_t rlr = us / 100;  //compositive divider - 40000 * us / 100000, min divider is 4
    uint32_t psc = 0;
    while (rlr & 0xFFFFF000) {
        rlr >>= 1;
        ++psc;
    }    
    IWDG_Init(psc, rlr - 1);
}

void IWDG_Init(uint32_t psc, uint32_t rlr) {
    IWDG->KR = 0x5555; 
    IWDG->PR = psc; 
    IWDG->RLR = rlr; 
    IWDG->KR = 0xAAAA; 
    IWDG->KR = 0xCCCC; 
}

void IWDG_reset(){
    IWDG->KR=0xAAAA; //Reset iwdg counter
}
