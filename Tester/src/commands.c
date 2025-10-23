#include "commands.h"
#include "init.h"

#include "u2a.h"
#include "bridge.h"

void TX_CharUART(char c) {
    RBUF_WRITE(usart_tx_buf, c);
}

void TX_CB(char c) {
    CDC_TX_CHAR(c);
}

void PrintText(char* text, void tx_char(char)) {
    while(*text)
        tx_char(*text++);
}

void PrintInt(char* prefix, int32_t val, void tx_char(char)) {
    PrintText(prefix, tx_char);
    itoaLTRCB(val, tx_char);
    tx_char(' ');
}

void PrintFloat(char* prefix, float val, void tx_char(char)) {
    PrintText(prefix, tx_char);
    ftoaCB(val, tx_char);
    tx_char(' ');
}

void PrintHex(char* prefix, int32_t val, void tx_char(char)) {
    PrintText(prefix, tx_char);
    utoHexCB(val, tx_char);
    tx_char(' ');
}


void PrintBool(char* prefix, uint8_t val, void tx_char(char)) {
    PrintText(prefix, tx_char);
    uint8_t a = 0b10000000;
    while (a)
    {
        if (val & a) tx_char('1'); else tx_char('0');
        a = a >> 1;
    }
}

uint8_t ExecuteTextCommand(char* cmd, uint8_t cmd_size) {
    if (cmd_size) {
        void (*tx_char)(char);
        //tx_char = TX_CharUART;
        tx_char = TX_CB;

        switch(cmd[0])
        {
            case 'S': // Status
                PrintText("tester\n", tx_char);
            break;

            case 'R': // ru6
                if(cmd[1] == 'C')
                {
                   start_556RU6();

                    cycle_test ^= 1;
                    PrintText("556RU6 cycle\n", tx_char);
                }
                if(cmd[1] == 'R')
                {
                    start_556RU6();

                    if ((RU6_mode == RU6_NONE) && (tester_mode = MODE_565RU6))
                    {
                        RU6_mode = RU6_READ;
                    }
                    Clear_Buffer();
                    PrintText("556RU6 READ\n", tx_char);
                }
                if(cmd[1] == 'W')
                {
                    start_556RU6();

                    if (cmd_size >= 2)
                    {
                     RU6_write_mode = parseFloat((uint8_t*)&cmd[2]);
                    }
                    if ((RU6_mode == RU6_NONE) && (tester_mode = MODE_565RU6))
                    {
                        RU6_mode = RU6_WRITE;
                    }
                    PrintText("556RU6 WRITE\n", tx_char);
                }
            break;
            case 'T': // rtx
                if(cmd[1] == '4')
                {
                    if (tester_mode != MODE_556RT4)
                    {
                        tester_mode = MODE_556RT4;
                        init_556RTx();
                    }
                }
                if(cmd[1] == '5')
                {
                    if (tester_mode != MODE_556RT5)
                    {
                        tester_mode = MODE_556RT5;
                        init_556RTx();
                    }

                }
                    if (cmd[2] == 'A') //SET Adress
                    {
                        write_adr = parseFloat((uint8_t*)&cmd[3]);
                        PrintInt("ADR=", write_adr, tx_char);
                        PrintText("\n", tx_char);
                        break;
                    }
                    if (cmd[2] == 'D') // SET Data
                    {
                        write_data = parseFloat((uint8_t*)&cmd[3]);
                        PrintInt("ADR=", write_adr, tx_char);
                        PrintInt(" DATA=", write_data, tx_char);
                        PrintText("\n", tx_char);
                        break;
                    }
                    if (cmd[2] == 'W')// Write one data to adress
                    {
                        write_data = parseFloat((uint8_t*)&cmd[3]);
                        task_runOnce(task_program_RTx,10,3);
                        PrintInt("ADR=", write_adr, tx_char);
                        PrintInt(" DATA=", write_data, tx_char);
                        PrintText("\n", tx_char);
                        break;

                    }
                    if (cmd[2] == 'P') // Write data from adress = 0  to adress = bufsize
                    {
                        task_runOnce(task_program_RTx,10,4);
                        write_adr = 0;
                        PrintInt("ADR=", write_adr, tx_char);
                        PrintInt(" DATA=", write_data, tx_char);
                        PrintText("\n", tx_char);
                        break;

                    }
                PrintInt("", tester_mode, tx_char);
                PrintText("\n", tx_char);
                read_RTx();
            break;
            default:
                PrintText("Unknown command\n", tx_char);
            break;
        }
        //usart_transmit(USART1);
    }
    return 0;
}


