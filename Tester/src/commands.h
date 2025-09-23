#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <stdint.h>

void TX_CharUART(char c);

void PrintInt(char* prefix, int32_t val, void tx_char(char));
void PrintText(char* text, void tx_char(char));

uint8_t ExecuteTextCommand(char* cmd, uint8_t cmd_size, uint8_t* responce);
#endif /* _COMMANDS_H_ */
