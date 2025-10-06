#include "stm32.h"
#include "init.h"
#include "bridge.h"
#include "cdc.h"


struct cdc_tx_buf cdc_tx_buf;

void cdc_rx_callback() {
    static uint8_t cmdStart = 0;
    static uint8_t cmdIdx = 0;
    static uint8_t buf[CDC_RX_BUF_SIZE];

    uint8_t cnt = cdc_read_size();
    if (cnt) {
        uint8_t buf_size = sizeof(buf) - cmdIdx;
        if (buf_size < cnt) {
            cmdStart = cmdIdx = 0;
            buf_size = sizeof(buf);
        }

        cnt = cdc_read(&buf[cmdIdx], buf_size);
        while(cnt--) {
            if ((buf[cmdIdx] == 0x0D) || (buf[cmdIdx] == 0x0A)) {
                uint8_t cmd_size = cmdIdx - cmdStart + 1;
                if (cmd_size > 1)
                    ExecuteTextCommand((char*)&buf[cmdStart], cmd_size);
                cmdStart = cmdIdx + 1;
            }
            cmdIdx++;
        }

        if (cmdStart == cmdIdx)
            cmdStart = cmdIdx = 0;
    }
}

void cdc_tx_callback() {
    uint_fast8_t cnt = CDC_TX_COUNT();
    if (cnt) {
        if (cnt > CDC_TX_DATA_SZ)
            cnt = CDC_TX_DATA_SZ;

        uint8_t buf[CDC_TX_DATA_SZ];
        for(uint8_t i = 0; i < cnt; i++) {
            RBUF_READ(cdc_tx_buf, buf[i]);
        }

        cdc_write(buf, cnt);
    } else
        cdc_write(0, 0);
}

__WEAK void ProcessCMD(uint8_t *cmd, uint8_t size) {}
