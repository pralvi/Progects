/**
  ******************************************************************************
  * @file    game500.c
  * @brief   Cassette / ROM emulator application, ported to STM32F103C8T6.
  *
  *  Original target : WCH CH32V103C8T6 (RISC-V, RV32IMAC)
  *  New target      : STMicroelectronics STM32F103C8T6 (ARM Cortex-M3)
  *
  *  Register translation applied during the port:
  *      CH32  GPIOx->OUTDR   ->  STM32  GPIOx->ODR
  *      CH32  GPIOx->INDR    ->  STM32  GPIOx->IDR
  *      GPIO_SetBits(p,m)    ->  p->BSRR = (m)
  *      GPIO_ResetBits(p,m)  ->  p->BRR  = (m)
  *      GPIO_ReadInputDataBit(p,m) -> ((p->IDR & (m)) != 0)
  *      CH32 SysTick delays  ->  Cortex-M3 DWT cycle-counter delays
  *
  *  This file is NOT touched by STM32CubeMX code generation, so all
  *  application logic lives here and survives an .ioc regeneration.
  ******************************************************************************
  */
#include "game500.h"

/* --------------------------------------------------------------------------
 *  DWT (Data Watchpoint & Trace) cycle-counter based microsecond delays.
 *  Accurate and independent of the core clock value (uses SystemCoreClock).
 * ------------------------------------------------------------------------ */
static uint32_t cycles_per_us = 72;   /* recomputed in Delay_Init() */

void Delay_Init(void)
{
    /* Enable the trace subsystem and the cycle counter (DWT->CYCCNT). */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    cycles_per_us = SystemCoreClock / 1000000U;
    if (cycles_per_us == 0U)
    {
        cycles_per_us = 72U;          /* safe fallback: 72 MHz */
    }
}

void Delay_Us(uint32_t n)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = n * cycles_per_us;
    while ((DWT->CYCCNT - start) < ticks)
    {
    }
}

void Delay_Ms(uint32_t n)
{
    while (n--)
    {
        Delay_Us(1000U);
    }
}

/* --------------------------------------------------------------------------
 *  Application state (was a set of globals in the original game500.h)
 * ------------------------------------------------------------------------ */
static uint8_t  PU_addr    = 0;
static uint16_t PU_addr_in = 0;
static uint8_t  PU_data    = 0;
static uint16_t PU_data_out = 0;
static uint8_t  file_addrH = 0;
static uint8_t  file_addrL = 0;
static uint16_t file_addr  = 0;
static uint8_t  file_byte  = 0;
static uint16_t byte_addr  = 0;
static uint8_t  MODE1_FLAG = 0;
static uint8_t  MODE2_FLAG = 0;

/* --------------------------------------------------------------------------
 *  loader1  - CASSETTE image (bit-banged out on the AUDIO pin)
 * ------------------------------------------------------------------------ */
static const uint8_t loader1[2000] = {
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // pilottone
        0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55, // pilottone
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // pilottone
        0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55, // pilottone
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // pilottone
        0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55, // pilottone
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // pilottone
        0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55, // pilottone
//200
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // block marker
//216
        0x55,0x55,0x55,0x55,0xe6, // subblock0 marker
        0x00,0x00,0x00,0x00,
        0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, //name
        0x00,0x00,
        0x01,0x01,0x01,0x23, // MSB address, number of block, current number, CRC0
//254
        0x00,0x00,0x00,0x0,0xe6, // subblock1 marker
        0x80,0x23, // number of subblock1, CRC0
        0xF3,0x3E,0x82,0xD3,0x04,0x01,0x80,0x00,0x11,0x1A,0x01,0x21,0x00,0x80,0x1A,0x77, // data
        0x13,0x23,0x05,0xC2,0x0E,0x01,0xC3,0x00,0x80,0x00,0x21,0x00,0x00,0x7D,0xD3,0x07, // data
        0xd3, // CRC1
//294
        0x00,0x00,0x00,0x0,0xe6, // subblock2 marker
        0x81,0x23, // number of subblock2, CRC0
        0x7C,0xD3,0x05,0xDB,0x06,0x77,0x23,0x7C,0xFE,0x80,0xC2,0x03,0x80,0xC3,0x00,0x00, // data
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0x75, // CRC2
//334
        0x00,0x00,0x00,0x0,0xe6, // subblock3 marker
        0x82,0x23, // number of subblock3, CRC0
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0xa5, // CRC3
//374
        0x00,0x00,0x00,0x0,0xe6, // subblock4 marker
        0x83,0x23, // number of subblock4, CRC0
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0xa6, // CRC4
//414
        0x00,0x00,0x00,0x0,0xe6, // subblock5 marker
        0x84,0x23, // number of subblock5, CRC0
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0xa7, // CRC5
//454
        0x00,0x00,0x00,0x0,0xe6, // subblock6 marker
        0x85,0x23, // number of subblock6, CRC0
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0xa8, // CRC6
//494
        0x00,0x00,0x00,0x0,0xe6, // subblock7 marker
        0x86,0x23, // number of subblock7, CRC0
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0xa9, // CRC7
//534
        0x00,0x00,0x00,0x0,0xe6, // subblock8 marker
        0x87,0x23, // number of subblock8, CRC0
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // data
        0xaa // CRC8
//574
};

/* --------------------------------------------------------------------------
 *  ROM_menu  - loader2 ROM image (bit-banged out on the parallel data bus)
 * ------------------------------------------------------------------------ */
static const uint8_t ROM_menu[256] = {
        0xF3, 0x0E, 0xFF, 0x11, 0x15, 0x00, 0x21, 0x00, 0xE0, 0x1A, 0x77, 0x13, 0x23, 0x0D, 0xC2, 0x09,
        0x00, 0xC3, 0x00, 0xE0, 0x00, 0x31, 0x00, 0xE8, 0x3E, 0x82, 0xD3, 0x04, 0x3E, 0x00, 0xD3, 0x05,
        0x3E, 0xAA, 0xD3, 0x07, 0x13, 0x13, 0x13, 0x13, 0x3E, 0x55, 0xD3, 0x07, 0x13, 0x13, 0x13, 0x13,
        0x3E, 0x00, 0xD3, 0x07, 0x3E, 0x10, 0xD3, 0x05, 0x13, 0x13, 0x13, 0x13, 0x3E, 0x00, 0xD3, 0x05,
        0x13, 0x13, 0x13, 0x13, 0x3E, 0x00, 0xD3, 0x07, 0x3E, 0x10, 0xD3, 0x05, 0x13, 0x13, 0x13, 0x13,
        0x3E, 0x00, 0xD3, 0x05, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x21, 0x00,
        0x00, 0x3E, 0x10, 0xD3, 0x05, 0x13, 0x13, 0x13, 0x13, 0x3E, 0x00, 0xD3, 0x05, 0x13, 0x13, 0x13,
        0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0xDB, 0x06, 0x77, 0x23,
        0x7C, 0xFE, 0x80, 0xC2, 0x4C, 0xE0, 0x3E, 0x81, 0xD3, 0x07, 0x13, 0x13, 0x13, 0x13, 0x13, 0x3E,
        0x10, 0xD3, 0x05, 0x13, 0x13, 0x13, 0x13, 0x13, 0x3E, 0x00, 0xD3, 0x05, 0x13, 0x13, 0x13, 0x13,
        0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x3E, 0x66, 0xD3, 0x07, 0x3E,
        0x10, 0xD3, 0x05, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x3E, 0x00,
        0xD3, 0x05, 0xC3, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
};

/* ==========================================================================
 *  Subroutines
 * ======================================================================== */

/* Play one byte on the AUDIO pin (MSB first). */
static void play_byte(uint8_t byte)
{
    uint8_t x;
    for (x = 0; x < 8; x++)
    {
        if ((byte & 0x80) == 0)
        {
            GPIOB->BSRR = AUDIO;
            Delay_Us(NW);
            GPIOB->BRR  = AUDIO;
            Delay_Us(NW);
        }
        else
        {
            GPIOB->BRR  = AUDIO;
            Delay_Us(NW);
            GPIOB->BSRR = AUDIO;
            Delay_Us(NW);
        }
        byte = byte << 1;
    }
}

/* Play the cassette loader (first 580 bytes of loader1). */
static void play_loader(void)
{
    uint8_t  send_byte;
    uint16_t b;

    GPIOA->BSRR = LED_G;
    GPIOA->BRR  = LED_B;
    for (b = 0; b < 580; b++)
    {
        send_byte = loader1[b];
        play_byte(send_byte);
    }
    GPIOA->BSRR = LED_G;
    GPIOA->BRR  = LED_B;
}

/* Reset the external SPI flash (ENABLE-RESET 0x66, then RESET 0x99). */
static void flash_init1(void)
{
    int i;
    uint8_t tx_data8;

    GPIOB->BRR  = SPI_DI;
    GPIOB->BRR  = SPI_CLK;
    GPIOB->BSRR = SPI_CS;
    Delay_Ms(1);

    /* send ENABLE RESET command (0x66) */
    i = 8;
    tx_data8 = 0x66;
    GPIOB->BRR = SPI_CS;
    while (i != 0)
    {
        if ((tx_data8 & 0x80) != 0) { GPIOB->BSRR = SPI_DI; }
        else                        { GPIOB->BRR  = SPI_DI; }
        tx_data8 = tx_data8 << 1;
        i = i - 1;
        GPIOB->BSRR = SPI_CLK;
        GPIOB->BRR  = SPI_CLK;
    }
    i = 0;
    while (i < 5) { i = i + 1; }
    GPIOB->BSRR = SPI_CS;

    /* send RESET command (0x99) */
    i = 8;
    tx_data8 = 0x99;
    GPIOB->BRR = SPI_CS;
    while (i != 0)
    {
        if ((tx_data8 & 0x80) != 0) { GPIOB->BSRR = SPI_DI; }
        else                        { GPIOB->BRR  = SPI_DI; }
        tx_data8 = tx_data8 << 1;
        i = i - 1;
        GPIOB->BSRR = SPI_CLK;
        GPIOB->BRR  = SPI_CLK;
    }
    GPIOB->BSRR = SPI_CS;
    GPIOB->BRR  = SPI_DI;
    GPIOB->BRR  = SPI_CLK;
    Delay_Ms(2);
}

/* Read one byte from the external SPI flash.
 *   f_addr - file slot (each slot is 0x2000 bytes)
 *   b_addr - byte offset inside the file
 */
static uint8_t read_flash(uint16_t f_addr, uint16_t b_addr)
{
    int i;
    uint8_t  tx_data8;
    uint32_t tx_data32 = 0;
    uint8_t  rx_data;

    tx_data32 = 0x2000 * (uint32_t)f_addr;        /* calculate flash address */
    tx_data32 = tx_data32 + (uint32_t)b_addr;     /* calculate flash address */

    GPIOB->BRR = SPI_CS;

    /* send CMD (0x03 = read data) */
    i = 8;
    tx_data8 = 0x03;
    while (i != 0)
    {
        if ((tx_data8 & 0x80) != 0) { GPIOB->BSRR = SPI_DI; }
        else                        { GPIOB->BRR  = SPI_DI; }
        tx_data8 = tx_data8 << 1;
        i = i - 1;
        GPIOB->BSRR = SPI_CLK;
        GPIOB->BRR  = SPI_CLK;
    }

    /* send 24-bit ADDR */
    i = 24;
    tx_data32 = tx_data32 << 8;
    while (i != 0)
    {
        if ((tx_data32 & 0x80000000) != 0) { GPIOB->BSRR = SPI_DI; }
        else                               { GPIOB->BRR  = SPI_DI; }
        tx_data32 = tx_data32 << 1;
        i = i - 1;
        GPIOB->BSRR = SPI_CLK;
        GPIOB->BRR  = SPI_CLK;
    }

    /* read DATA */
    i = 8;
    rx_data = 0;
    while (i != 0)
    {
        rx_data = rx_data << 1;
        if ((GPIOB->IDR & SPI_DO) != 0)
        {
            rx_data = rx_data | 0x01;
        }
        i = i - 1;
        GPIOB->BSRR = SPI_CLK;
        GPIOB->BRR  = SPI_CLK;
    }
    GPIOB->BSRR = SPI_CS;
    return rx_data;
}

/* Place a data byte onto the parallel data bus (GPIOA), keeping LED_G on. */
static void write_PU2(uint8_t write_data)
{
    uint16_t out = 0;
    out |= ((write_data & 0x01) << 3);
    out |= ((write_data & 0x02) << 4);
    out |= ((write_data & 0x04) << 2);
    out |= ((write_data & 0x08) << 3);
    out |= ((write_data & 0x10) << 3);
    out |= ((write_data & 0x20) << 3);
    out |= ((write_data & 0x40) << 5);
    out |= ((write_data & 0x80) << 5);
    out |= LED_G;

    GPIOA->ODR = (uint32_t)out;
}

/* ==========================================================================
 *  Application entry point (was main() on the CH32V103).
 * ======================================================================== */
void game500_run(void)
{
    Delay_Init();
    Delay_Ms(1);
    flash_init1();

    GPIOC->BSRR = LED_R;
    GPIOA->BSRR = LED_G;
    GPIOA->BSRR = LED_B;

// ENTER MODE1
MODE1:
    MODE2_FLAG = 0;
    while (MODE2_FLAG != 2)
    {
        if ((GPIOA->IDR & PLAY) == 0) play_loader();
        PU_addr = 0;
        PU_addr_in = (uint16_t)GPIOB->IDR;
        PU_addr |= (uint8_t)((PU_addr_in & ADDR0) >> 9);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR1) >> 7);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR2) >> 5);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR3) >> 3);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR4) >> 1);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR5) << 1);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR6) >> 4);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR7) >> 4);
        if (PU_addr == 0xaa)
        {
            MODE2_FLAG = 1;
        }
        else if (PU_addr == 0x55)
        {
            if (MODE2_FLAG == 1) { MODE2_FLAG = 2; }
            else                 { MODE2_FLAG = 0; }
        }
        else
        {
            MODE2_FLAG = 0;
        }
        PU_data = ROM_menu[PU_addr];
        PU_data_out = 0;
        PU_data_out |= ((PU_data & 0x01) << 3);
        PU_data_out |= ((PU_data & 0x02) << 4);
        PU_data_out |= ((PU_data & 0x04) << 2);
        PU_data_out |= ((PU_data & 0x08) << 3);
        PU_data_out |= ((PU_data & 0x10) << 3);
        PU_data_out |= ((PU_data & 0x20) << 3);
        PU_data_out |= ((PU_data & 0x40) << 5);
        PU_data_out |= ((PU_data & 0x80) << 5);
        PU_data_out |= LED_B;
        GPIOA->ODR = (uint32_t)PU_data_out;
    }
    GPIOC->BSRR = LED_R;
    GPIOA->BSRR = LED_G;
    GPIOA->BRR  = LED_B;

// MODE2
    MODE1_FLAG = 0;
    byte_addr = 0;
    while (((uint16_t)GPIOB->IDR & ADDR12) == 0) { }   // wait clock
    while (((uint16_t)GPIOB->IDR & ADDR12) != 0) { }

    PU_addr = 0;                                        // read H byte
    PU_addr_in = (uint16_t)GPIOB->IDR;
    PU_addr |= (uint8_t)((PU_addr_in & ADDR0) >> 9);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR1) >> 7);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR2) >> 5);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR3) >> 3);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR4) >> 1);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR5) << 1);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR6) >> 4);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR7) >> 4);
    file_addrH = PU_addr;

    while (((uint16_t)GPIOB->IDR & ADDR12) == 0) { }   // wait clock
    while (((uint16_t)GPIOB->IDR & ADDR12) != 0) { }

    PU_addr = 0;                                        // read L byte
    PU_addr_in = (uint16_t)GPIOB->IDR;
    PU_addr |= (uint8_t)((PU_addr_in & ADDR0) >> 9);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR1) >> 7);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR2) >> 5);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR3) >> 3);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR4) >> 1);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR5) << 1);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR6) >> 4);
    PU_addr |= (uint8_t)((PU_addr_in & ADDR7) >> 4);
    file_addrL = PU_addr;
    file_addr = file_addrH * 0x100 + file_addrL;

    while (byte_addr != 0xe000)
    {
        while (((uint16_t)GPIOB->IDR & ADDR12) == 0) { }   // wait clock
        while (((uint16_t)GPIOB->IDR & ADDR12) != 0) { }
        PU_addr = 0;
        PU_addr_in = (uint16_t)GPIOB->IDR;
        PU_addr |= (uint8_t)((PU_addr_in & ADDR0) >> 9);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR1) >> 7);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR2) >> 5);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR3) >> 3);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR4) >> 1);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR5) << 1);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR6) >> 4);
        PU_addr |= (uint8_t)((PU_addr_in & ADDR7) >> 4);
        file_byte = read_flash(file_addr, byte_addr);
        write_PU2(file_byte);
        if (PU_addr == 0x81)
        {
            MODE1_FLAG = 1;
        }
        else if ((PU_addr == 0x66) && (MODE1_FLAG == 1))
        {
            MODE1_FLAG = 2;
        }
        else
        {
            MODE1_FLAG = 0;
        }
        byte_addr = byte_addr + 1;
        if (MODE1_FLAG == 2)
        {
            GPIOA->BRR  = LED_G;
            GPIOA->BSRR = LED_B;
            goto MODE1;
        }
    }

    while (1)
    {
        GPIOC->BRR  = LED_R;
        GPIOA->BSRR = LED_G;
        GPIOA->BSRR = LED_B;
    }
}
