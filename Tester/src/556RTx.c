#include "556RTx.h"
#include "init.h"
#include "commands.h"
#include "gpio.h"



uint16_t bufsize;
uint16_t write_adr = 0;
uint8_t write_data = 0;


void clear_data(void)
{
    SET_PIN(DATA0 , 0);
    SET_PIN(DATA1 , 0);
    SET_PIN(DATA2 , 0);
    SET_PIN(DATA3 , 0);
    SET_PIN(DATA4 , 0);
    SET_PIN(DATA5 , 0);
    SET_PIN(DATA6 , 0);
    SET_PIN(DATA7 , 0);
}

void set_datain_pulldown(void)
{
    CFG_PIN(DIN0, INPUT, PULLDOWN);
    CFG_PIN(DIN1, INPUT, PULLDOWN);
    CFG_PIN(DIN2, INPUT, PULLDOWN);
    CFG_PIN(DIN3, INPUT, PULLDOWN);
    CFG_PIN(DIN4, INPUT, PULLDOWN);
    CFG_PIN(DIN5, INPUT, PULLDOWN);
    CFG_PIN(DIN6, INPUT, PULLDOWN);
    CFG_PIN(DIN7, INPUT, PULLDOWN);

}

void set_datain_pullup(void)
{
    CFG_PIN(DIN0, INPUT, PULLUP);
    CFG_PIN(DIN1, INPUT, PULLUP);
    CFG_PIN(DIN2, INPUT, PULLUP);
    CFG_PIN(DIN3, INPUT, PULLUP);
    CFG_PIN(DIN4, INPUT, PULLUP);
    CFG_PIN(DIN5, INPUT, PULLUP);
    CFG_PIN(DIN6, INPUT, PULLUP);
    CFG_PIN(DIN7, INPUT, PULLUP);
}

void init_556RTx(void)
{

    SET_PIN(ADR0 , 0);
    SET_PIN(ADR1 , 0);
    SET_PIN(ADR2 , 0);
    SET_PIN(ADR3 , 0);
    SET_PIN(ADR4 , 0);
    SET_PIN(ADR5 , 0);
    SET_PIN(ADR6 , 0);
    SET_PIN(ADR7 , 0);
    SET_PIN(ADR8 , 0);
    SET_PIN(ADR9 , 0);
    SET_PIN(ADR10 , 0); // FOR RT5 RT17 CS4 !!

    clear_data();


    SET_PIN(CS2 , 1);
    SET_PIN(CS3 , 1);

    SET_PIN(ON12V , 0);

    CFG_PIN(ADR0, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR1, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR2, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR3, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR4, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR5, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR6, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR7, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR8, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR9, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(ADR10, OUTPUT, PUSHPULL | SLOPE_2MHZ);

    CFG_PIN(DATA0, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(DATA1, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(DATA2, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(DATA3, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(DATA4, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(DATA5, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(DATA6, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(DATA7, OUTPUT, PUSHPULL | SLOPE_2MHZ);


    CFG_PIN(CS2, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    CFG_PIN(CS3, OUTPUT, PUSHPULL | SLOPE_2MHZ);

    CFG_PIN(ON12V, OUTPUT, PUSHPULL | SLOPE_2MHZ);

    set_datain_pullup();


    if (tester_mode == MODE_556RT4)
    {
        bufsize = RT4BUFSIZE;
        SET_PIN(CS1 , 1);
        CFG_PIN(CS1, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    }
    if (tester_mode == MODE_556RT5)
    {
        bufsize = RT5BUFSIZE;
        SET_PIN(CS1 , 1);
        SET_PIN(CS4 , 1);
        CFG_PIN(CS1, OUTPUT, PUSHPULL | SLOPE_2MHZ);
        CFG_PIN(CS4, OUTPUT, PUSHPULL | SLOPE_2MHZ);
    }

    if (tester_mode == MODE_556RT17) bufsize = RT17BUFSIZE;
    if (tester_mode == MODE_556RT18) bufsize = RT18BUFSIZE;
}

void set_adr_RTx(uint16_t adr)
{
    SET_PIN(ADR0 , adr & 1);
    SET_PIN(ADR1 , adr & 2);
    SET_PIN(ADR2 , adr & 4);
    SET_PIN(ADR3 , adr & 8);
    SET_PIN(ADR4 , adr & 16);
    SET_PIN(ADR5 , adr & 32);
    SET_PIN(ADR6 , adr & 64);
    SET_PIN(ADR7 , adr & 128);
    SET_PIN(ADR8 , adr & 256);
    SET_PIN(ADR9 , adr & 512);
    SET_PIN(ADR10 , adr & 1024);
}

void program_byte_RTx(uint16_t adr, uint8_t data  )
{
    set_adr_RTx(adr);
    data = ~data;
    set_datain_pulldown();
    if (tester_mode == MODE_556RT4)
    {
        SET_PIN(DATA0 , data & 1);
        SET_PIN(DATA1 , (data >> 1) & 1);
        SET_PIN(DATA2 , (data >> 2) & 1);
        SET_PIN(DATA3 , (data >> 3) & 1);
        SET_PIN(CS1 , 0);
        SET_PIN(CS2 , 0);
        Delay_mks(10);
        SET_PIN(ON12V , 1);
        Delay_mks(10);
        SET_PIN(ON12V , 0);
        SET_PIN(CS1 , 1);
        SET_PIN(CS2 , 1);

        Delay_mks(100000);
        PrintInt("Adr=", adr, TX_CB);
        PrintInt(" Prg=", (uint8_t)~data, TX_CB);


    }
     if (tester_mode == MODE_556RT5)
    {
        SET_PIN(CS1 , 0);
        SET_PIN(CS2 , 0);
    }
    set_datain_pullup();
    clear_data();
    uint8_t prg = read_byte_RTx(adr);
    PrintInt(" Data=", prg, TX_CB);
    if (((uint8_t)~data) == prg)
        PrintText(" OK\n", TX_CB);
    else
        PrintText(" BAD\n", TX_CB);
}

void task_program_RTx(uint8_t tag)
{
    if (tag == 3)
    program_byte_RTx(write_adr,write_data);
    if (tag == 4)
    {
        program_byte_RTx(write_adr,write_data);
        write_adr++;
        if (write_adr != bufsize)
        {
            task_runOnce(task_program_RTx,10,4);
        }

    }
}

uint8_t read_byte_RTx(uint16_t adr)
{
    uint8_t Data = 0;
    set_adr_RTx(adr);

    if (tester_mode == MODE_556RT4)
    {
        SET_PIN(CS1 , 0);
        SET_PIN(CS2 , 0);
    }
     if (tester_mode == MODE_556RT5)
    {
        SET_PIN(CS1 , 1);
        SET_PIN(CS2 , 1);
        SET_PIN(CS3 , 0);
        SET_PIN(CS4 , 0);
    }

    Data |=  GET_PIN(DIN0);
    Data |=  GET_PIN(DIN1) << 1;
    Data |=  GET_PIN(DIN2) << 2;
    Data |=  GET_PIN(DIN3) << 3;
    Data |=  GET_PIN(DIN4) << 4;
    Data |=  GET_PIN(DIN5) << 5;
    Data |=  GET_PIN(DIN6) << 6;
    Data |=  GET_PIN(DIN7) << 7;


    if (tester_mode == MODE_556RT4)
    {
        Data &= 0x0F;
        SET_PIN(CS1 , 1);
        SET_PIN(CS2 , 1);
    }
    if (tester_mode == MODE_556RT5)
    {
        SET_PIN(CS1 , 1);
        SET_PIN(CS2 , 1);
        SET_PIN(CS3 , 1);
        SET_PIN(CS4 , 1);
    }


    return Data;
}


void task_send_RTx(uint8_t tag)
{
    static uint8_t counter = 0;
    static uint16_t i = 0;

        if (counter == 0)
        {
            PrintHex("", i,TX_CB);
            PrintInt(" ", i,TX_CB);
        }

        PrintHex(" ", device_buffer[i],TX_CB);
        counter++;
        if (counter == 16)
        {
           PrintText("\n", TX_CB);
           counter = 0;
        }
    i++;
    if (i == bufsize)
    {
        i = 0;
        counter = 0;
        task_cancel(task_current(), task_send_RTx);
    }
}


void read_RTx(void)
{
     for (uint16_t i = 0; i < bufsize; i++)
     {
         device_buffer[i] = read_byte_RTx(i);
     }
     task_run(task_send_RTx,2,1);

}















