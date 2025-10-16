#include "565RU6.h"
#include "init.h"
#include "commands.h"
#include "gpio.h"

uint8_t RU6_mode = RU6_NONE;
uint8_t RU6_write_mode = 0;
uint8_t cycle_test = 0;

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
    SET_PIN(WR , 1);
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
    CFG_PIN(WR, OUTPUT, PUSHPULL | SLOPE_2MHZ);
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



uint8_t read_556RU6(uint16_t adr)
{
    SET_PIN(WR , 1);
    set_lo_adr_556RU6(adr);
    //Delay_us(1);
    SET_PIN(RAS , 0);
    Delay_us(1);
    set_hi_adr_556RU6(adr);
    //Delay_us(1);
    SET_PIN(CAS , 0);
    Delay_us(2);
    uint8_t data = GET_PIN(DO);
    SET_PIN(CAS , 1);
    SET_PIN(RAS , 1);
    return data;

}

void write_556RU6(uint16_t adr, uint8_t data)
{
    set_lo_adr_556RU6(adr);
    SET_PIN(RAS , 0);
    Delay_us(1);
    set_hi_adr_556RU6(adr);
    SET_PIN(DI , data);
    //Delay_us(1);
    SET_PIN(WR , 0);
    //Delay_us(1);
    SET_PIN(CAS , 0);
    Delay_us(2);
    SET_PIN(WR , 1);
    SET_PIN(CAS , 1);
    SET_PIN(RAS , 1);
    Delay_us(1);
}



void test_process_RU6(uint8_t frame)
{
    static uint16_t i = 0;

    if (RU6_mode == RU6_SEND) PrintHex("", i,TX_CB);

    while (frame--)
    {
        if (RU6_mode == RU6_READ)
        {
             uint8_t Data = read_556RU6(i);
             uint8_t Shift = i & 7;

            device_buffer[i >> 3] |= (Data << Shift);
            i++;
        }
        if (RU6_mode == RU6_WRITE)
        {
            if (RU6_write_mode == 0 ) write_556RU6(i,((uint8_t)i & 1));
            if (RU6_write_mode == 1 ) write_556RU6(i,((uint8_t)~i & 1));
            if (RU6_write_mode == 2 ) write_556RU6(i,0);
            if (RU6_write_mode == 3 ) write_556RU6(i,0xFF);
            i++;
        }
        if (RU6_mode == RU6_SEND)
        {
            PrintHex(" ", device_buffer[i],TX_CB);
            i++;
            if (frame == 0) PrintText("\r\n", TX_CB);
            if  (i == RU6BUFSIZE)
            {
                i = 0;
                RU6_mode = RU6_NONE;
                return;
            }
        }

    }
    if (i == (RU6_ADR_MAX + 1))
    {
        i = 0 ;
        if (RU6_mode == RU6_WRITE)
        {
            PrintText("WRITE OK\r\n", TX_CB);
            RU6_mode = RU6_NONE;
            return;
        }
        RU6_mode = RU6_SEND;
    }
}



void task_556RU6(uint8_t tag) // 128 * 2us = 256us cycle
{
    static uint8_t refresh = 0;
    refresh ^=1;
    if (refresh)
    {
__disable_irq();
        for (uint8_t i = 0; i <128; i++)
        {
            set_lo_adr_556RU6(i);
            //Delay_us(1);
            SET_PIN(RAS , 0);
            Delay_us(1);
            SET_PIN(RAS , 1);
            Delay_us(1);
        }
__enable_irq();
    }
    else
    {
        if (RU6_mode == RU6_READ) test_process_RU6(16);
        if (RU6_mode == RU6_WRITE) test_process_RU6(8);
        if (RU6_mode == RU6_SEND) test_process_RU6(8);
    }
}




void cycle_test_556RU6(uint8_t tag)
{
    static uint8_t state = 0;
    if (cycle_test == 0)
    {
        state = 0;
        return;
    }

    if (RU6_mode != RU6_NONE) return;

    switch (state)
    {
    case 0:
    {
        RU6_mode = RU6_WRITE;
        state = 1;
        break;
    }
    case 1:
    {
        RU6_mode = RU6_READ;
        Clear_Buffer();
        RU6_write_mode++;
        RU6_write_mode &= 3;

        state = 2;
        break;
    }
    case 2:
    {
        uint8_t Data = device_buffer[0];
        uint64_t result = 0;
        for (uint16_t i=0; i < RU6BUFSIZE; i++)
        {
            device_buffer[i] ^= Data;
            result += device_buffer[i];
        }
         PrintInt("Result XOR = ",result, TX_CB);
        PrintText("\r\n", TX_CB);
        state = 0;
        break;
    }

    }



}

void start_556RU6(void)
{
    if (tester_mode == MODE_556RU6) return;
    tester_mode = MODE_556RU6;
    RU6_mode = RU6_NONE;
    init_565RU6();
    Clear_Buffer();
    task_run(task_556RU6,1,1);
    task_run(cycle_test_556RU6,1000,1);


}
