#include "565RU6.h"
#include "init.h"


void init_565RU6(void)
{

    SET_PIN(ADR0 , 0);
    SET_PIN(ADR1 , 0);
    SET_PIN(ADR2 , 0);
    SET_PIN(ADR3 , 0);
    SET_PIN(ADR4 , 0);
    SET_PIN(ADR5 , 0);
    SET_PIN(ADR6 , 0);

    SET_PIN(RAS , 1);
    SET_PIN(CAS , 1);
    SET_PIN(DI , 1);

    CFG_PIN(ADR0, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR1, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR2, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR3, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR4, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR5, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR6, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR7, OUTPUT, PUSHPULL | SLOPE_2MHZ);

    CFG_PIN(RAS, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(CAS, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(DI, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(DO, INPUT, FLOATING);

}

void set_lo_adr_556RU6(uint16_t adr)
{
    SET_PIN(ADR0 , adr & 1);
    SET_PIN(ADR1 , adr & 2);
    SET_PIN(ADR2 , adr & 4);
    SET_PIN(ADR3 , adr & 8);
    SET_PIN(ADR4 , adr & 16);
    SET_PIN(ADR5 , adr & 32);
    SET_PIN(ADR6 , adr & 64);
}

void set_hi_adr_556RU6(uint16_t adr)
{
    SET_PIN(ADR0 , adr & 128);
    SET_PIN(ADR1 , adr & 256);
    SET_PIN(ADR2 , adr & 512);
    SET_PIN(ADR3 , adr & 1024);
    SET_PIN(ADR4 , adr & 2048);
    SET_PIN(ADR5 , adr & 4096);
    SET_PIN(ADR6 , adr & 8192);
}

void regen_556RU6(uint8_t tag) // 128 * 2us = 256us cycle
{
    for (uint8_t i = 0; i <128; i++)
    {
        set_lo_adr_556RU6(i);
        SET_PIN(RAS , 0);
        Delay_us(1);
        SET_PIN(RAS , 1);
        Delay_us(1);
    }

}

uint8_t read_556RU6(uint16_t adr)
{
    set_lo_adr_556RU6(adr);
    SET_PIN(RAS , 0);
    Delay_us(1);
    set_hi_adr_556RU6(adr);
    SET_PIN(WR , 1);
    Delay_us(1);
    SET_PIN(CAS , 0);
    Delay_us(1);
    SET_PIN(CAS , 1);
    SET_PIN(RAS , 1);
    return GET_PIN(DO);
}

void write_556RU6(uint16_t adr, uint8_t data)
{
    set_lo_adr_556RU6(adr);
    SET_PIN(RAS , 0);
    Delay_us(1);
    set_hi_adr_556RU6(adr);
    SET_PIN(DI , data);
    SET_PIN(WR , 0);
    Delay_us(1);
    SET_PIN(CAS , 0);
    Delay_us(1);
    SET_PIN(WR , 1);
    SET_PIN(CAS , 1);
    SET_PIN(RAS , 1);
}

void start_556RU6(void)
{
    if (tester_mode == MODE_556RU6) return;
    tester_mode = MODE_556RU6;
    init_565RU6();
    task_run(regen_556RU6,2,1);

}


