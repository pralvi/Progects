#include "as5047.h"
#include "init.h"
#include "spi.h"
#include "gpio.h"

#define CS_LOW  SET_PIN(SPI_CS_PIN, 0)
#define CS_HIGH SET_PIN(SPI_CS_PIN, 1)

#define AS_NOP          0xC000 //No operation
#define AS_ERRFL        0x4001 //Error register
#define AS_DIAAGC       0xFFFC //Diagnostic and AGC
#define AS_MAG          0x3FFD //CORDIC magnitude
#define AS_ANGLEUNC     0x7FFE //Measured angle without dynamic angle error compensation
#define AS_ANGLECOM     0xFFFF //Measured angle with dynamic angle error compensation    

#define TO_CMD(W) ((uint8_t[]){W >> 8, W & 0xFF})
#define TO_FLT(W) (uint16_t)(((W & 0xFF) << 8) + (W >> 8))
#define ITEM_COUNT(A) (sizeof(A)/sizeof(A[0]))

uint16_t AS_Command(void* cmd) {
    CS_LOW;
    uint8_t res[2];
    SPI_SendReceiveDMA(cmd, res, 2);
    while(SPI_IsDMAEnabled()) __WFI();
    CS_HIGH;
    return res[0] << 8 | res[1];
}

void AS_Init() {
    while(1) {
        uint32_t status = AS_Diagnostic();
        if ((status & 0xFFFF7F00) == 0x100) 
            break;
        __WFI();
    }
}

uint32_t AS_Diagnostic() {
//    AS_Command(TO_CMD(AS_ERRFL));
//    uint16_t err = AS_Command(TO_CMD(AS_NOP));
    AS_Command(TO_CMD(AS_DIAAGC));
    uint16_t diag = AS_Command(TO_CMD(AS_NOP));
    AS_Command(TO_CMD(AS_ERRFL));
    return AS_Command(TO_CMD(AS_NOP)) << 16 | diag;
}

char* AS_DiagnosticText() {
    uint32_t status = AS_Diagnostic();
    
    if (status & (1 << 30 | 1 << 14)) return "Command frame error occurred";
    if (status & (1 << 18)) return "Parity error";
    if (status & (1 << 17)) return "Invalid register";
    if (status & (1 << 16)) return "Framing error";
    if (status & (1 << 11)) return "MAGL - magnetic field strength too low, set if AGC = 0xFF. This indicates the output noise of the measured angle may be increased";
    if (status & (1 << 10)) return "MAGH - magnetic field strength too high, set if AGC = 0x00. This indicates the non-linearity error may be increased";
    if (status & (1 << 9))  return "COF - CORDIC overflow. This indicates the measured angle is not reliable";
    if (status & (1 << 8))  return "LF - offset compensation completed. At power-up, an internal offset compensation procedure is started, and this bit is set when the procedure is completed";
    
    return 0;
}

uint16_t AS_ReadPosition() {
    uint16_t pos = AS_Command(TO_CMD(AS_ANGLEUNC));
    if (!pos)
        pos = AS_Command(TO_CMD(AS_ANGLEUNC));

    return pos & 0x3FFF;
}

PositionCB* positionCB;

//void CB(uint16_t position)

#define ABS(a) ((a) >= 0  ? (a) : (-(a)))

void SPI_DMA_RXC(void) { //Receive over DMA completed
    static uint8_t res[] = {0x00, 0x00};
    static uint16_t cmd[] = {
        TO_FLT(AS_ERRFL),       //Clear errors if any
        TO_FLT(AS_ANGLEUNC),    //Query position
        TO_FLT(AS_NOP)};
    static uint8_t cmd_pos = 0;
    static uint16_t cur_pos = 0;
    static uint32_t err_cnt = 0;

    if (positionCB) {
        CS_HIGH;
        if (!cmd_pos) {
            if (!(res[0] & 0x40)){//Process data
                uint16_t pos = (res[0] << 8 | res[1]) & 0x3FFF;
                if (pos) {
                    int32_t dif = cur_pos - pos;
                    if (ABS(dif) > 9) {
                        cur_pos = pos;
                        positionCB(pos, err_cnt);
                        err_cnt = 0;
                    }
                }
            } else
                err_cnt++;
        } else if (cmd_pos == 2 && (res[0] & 0x40)) {
            //static a
            //err_cnt++;
        }

        //Query next position
        SPI_ReceiveDMA(res, sizeof(res));
        CS_LOW;
        SPI_SendDMA(&cmd[cmd_pos], sizeof(res));
        //cmd_pos += sizeof(res);
        cmd_pos++;
        if (cmd_pos >= ITEM_COUNT(cmd))
            cmd_pos = 0;
    }
}

void AS_ReadPositionCB(PositionCB CB) {
    positionCB = CB;
    SPI_DMA_RXC();
}

uint16_t AS_ReadPositionAVG() {
    static const uint8_t cnt = 16;
    
    uint32_t pos = 0;
    for(uint8_t i = 0; i < cnt; i++) {
        pos += AS_ReadPosition();
    }
    return pos / cnt;
}
