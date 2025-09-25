#include "commands.h"
#include "init.h"

#include "u2a.h"



static char*    CAN_responce_buf;
static uint8_t  CAN_responce_idx;
void TX_CharCAN(char c) {
    if (CAN_responce_idx < 8)
        if (c != '\r' && c != '\n')
            CAN_responce_buf[CAN_responce_idx++] = c;
}

void TX_CharUART(char c) {
    RBUF_WRITE(usart_tx_buf, c);
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

void PrintBool(char* prefix, uint8_t val, void tx_char(char)) {
    PrintText(prefix, tx_char);
    uint8_t a = 0b10000000;
    while (a)
    {
        if (val & a) tx_char('1'); else tx_char('0');
        a = a >> 1;
    }
}

uint8_t ExecuteTextCommand(char* cmd, uint8_t cmd_size, uint8_t* responce) {
    if (cmd_size) {
        void (*tx_char)(char);
        tx_char = TX_CharUART;

        switch(cmd[0])
        {
            case 'S': // Status
                PrintText("tester\r\n", tx_char);
            break;
            case 'R': // reset
                NVIC_SystemReset();
            break;
            case 'T': // ru6
                start_556RU6();
                PrintText("556RU6\r\n", tx_char);
            break;

            default:
                PrintText("Unknown command\r\n", tx_char);
            break;
        }
        usart_transmit(USART1);
    }
    return 0;
}


