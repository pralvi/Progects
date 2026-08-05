/**
  ******************************************************************************
  * @file    game500.h
  * @brief   Application (cassette / ROM emulator) - STM32F103C8T6 port.
  *          Ported from the original WCH CH32V103 (RISC-V) project.
  *
  *  Pin map (identical wiring to the original CH32V103 board):
  *
  *    Data bus  (GPIOA, push-pull outputs)
  *      DATA0 PA3   DATA1 PA5   DATA2 PA4   DATA3 PA6
  *      DATA4 PA7   DATA5 PA8   DATA6 PA11  DATA7 PA12
  *
  *    Address bus (GPIOB, inputs, pull-down)
  *      ADDR0 PB9   ADDR1 PB8   ADDR2 PB7   ADDR3 PB6
  *      ADDR4 PB5   ADDR5 PB4   ADDR6 PB10  ADDR7 PB11
  *      ADDR12 PB0  (external strobe / clock)
  *
  *    Bit-banged SPI flash (GPIOB)
  *      SPI_CS PB12  SPI_CLK PB13  SPI_DO PB14 (in)  SPI_DI PB15 (out)
  *
  *    Misc
  *      AUDIO PB1 (out)      PLAY PA0 (in)
  *      LED_R PC13           LED_G PA1           LED_B PA2
  *
  *  IMPORTANT (STM32 only): ADDR5 uses PB4, which is NJTRST by default.
  *  The JTAG-DP is disabled (SW-DP kept) via SWJ "no-JTAG" remap in
  *  MX_GPIO_Init() so PB4 works as a plain GPIO. SWD debugging still works.
  ******************************************************************************
  */
#ifndef __GAME500_H
#define __GAME500_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx.h"
#include <stdint.h>

/* ---- Data bus : GPIOA ---------------------------------------------------- */
#define DATA0   (1U << 3)    /* PA3  */
#define DATA1   (1U << 5)    /* PA5  */
#define DATA2   (1U << 4)    /* PA4  */
#define DATA3   (1U << 6)    /* PA6  */
#define DATA4   (1U << 7)    /* PA7  */
#define DATA5   (1U << 8)    /* PA8  */
#define DATA6   (1U << 11)   /* PA11 */
#define DATA7   (1U << 12)   /* PA12 */

/* ---- Address bus : GPIOB ------------------------------------------------- */
#define ADDR0   (1U << 9)    /* PB9  */
#define ADDR1   (1U << 8)    /* PB8  */
#define ADDR2   (1U << 7)    /* PB7  */
#define ADDR3   (1U << 6)    /* PB6  */
#define ADDR4   (1U << 5)    /* PB5  */
#define ADDR5   (1U << 4)    /* PB4  */
#define ADDR6   (1U << 10)   /* PB10 */
#define ADDR7   (1U << 11)   /* PB11 */
#define ADDR12  (1U << 0)    /* PB0  */

/* ---- LEDs ---------------------------------------------------------------- */
#define LED_R   (1U << 13)   /* PC13 */
#define LED_G   (1U << 1)    /* PA1  */
#define LED_B   (1U << 2)    /* PA2  */

/* ---- Bit-banged SPI flash : GPIOB --------------------------------------- */
#define SPI_CLK (1U << 13)   /* PB13 */
#define SPI_DI  (1U << 15)   /* PB15 */
#define SPI_DO  (1U << 14)   /* PB14 */
#define SPI_CS  (1U << 12)   /* PB12 */

/* ---- Misc ---------------------------------------------------------------- */
#define AUDIO   (1U << 1)    /* PB1  */
#define PLAY    (1U << 0)    /* PA0  */

/* Tape speed. More -> lower speed, less -> higher speed (microseconds). */
#define NW      170

/* ---- Public API ---------------------------------------------------------- */
void game500_run(void);                 /* application entry (never returns) */

/* Timing (DWT cycle-counter based, true microseconds) */
void Delay_Init(void);
void Delay_Us(uint32_t n);
void Delay_Ms(uint32_t n);

#ifdef __cplusplus
}
#endif

#endif /* __GAME500_H */
