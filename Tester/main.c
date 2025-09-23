#include <stdint.h>
#include <stdbool.h>
#include "init.h"
#include "commands.h"



int main(void) {
    InitHardware();
    uint32_t prev_tick = 01;
    for(;;) {
        if (tick != prev_tick) {
              process_tasks();
              prev_tick = tick;
        }
        __WFI();
    }
}


