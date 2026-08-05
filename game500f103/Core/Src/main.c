/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * game500 - cassette / ROM emulator, ported to STM32F103C8T6 (ARM Cortex-M3)
  * from the original WCH CH32V103 (RISC-V) firmware.
  *
  * Clock : HSE 8 MHz -> PLL x9 -> SYSCLK 72 MHz (APB1 = 36 MHz)
  * Driver: pure CMSIS register access - no HAL, no LL required.
  *         Compiles in ANY STM32F103 CubeIDE project (only needs stm32f1xx.h).
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "game500.h"
/* USER CODE END Includes */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* F1 GPIO 4-bit CNF+MODE nibbles (per pin, in CRL/CRH) */
#define GP_OUT_PP_50   0x3U   /* output push-pull, 50 MHz            */
#define GP_IN_FLOAT    0x4U   /* input, floating                     */
#define GP_IN_PUPD     0x8U   /* input with pull-up/down (ODR picks) */

static void cfg_pin(GPIO_TypeDef *port, uint32_t pin, uint32_t cnf_mode)
{
  volatile uint32_t *reg = (pin < 8U) ? &port->CRL : &port->CRH;
  uint32_t shift = (pin & 7U) * 4U;
  *reg = (*reg & ~(0xFU << shift)) | (cnf_mode << shift);
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Configure the system clock (HSE 8 MHz -> 72 MHz) */
  SystemClock_Config();

  /* Initialize all configured GPIO */
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  game500_run();          /* application main loop - never returns */
  /* USER CODE END 2 */

  while (1)
  {
  }
}

/**
  * @brief System Clock Configuration : HSE 8 MHz -> PLL x9 -> 72 MHz
  * @retval None
  */
void SystemClock_Config(void)
{
  /* Flash: enable prefetch + 2 wait states for 72 MHz */
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;

  /* Start HSE */
  RCC->CR |= RCC_CR_HSEON;
  while ((RCC->CR & RCC_CR_HSERDY) == 0U)
  {
  }

  /* PLL source = HSE (no /2), PLL x9  -> 8 MHz * 9 = 72 MHz */
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
  RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;

  /* Bus prescalers: AHB = /1 (72 MHz), APB1 = /2 (36 MHz), APB2 = /1 (72 MHz) */
  RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

  /* Enable PLL */
  RCC->CR |= RCC_CR_PLLON;
  while ((RCC->CR & RCC_CR_PLLRDY) == 0U)
  {
  }

  /* Switch SYSCLK to PLL */
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
  {
  }

  SystemCoreClock = 72000000U;
}

/**
  * @brief GPIO Initialization Function (pure CMSIS, F1 CRL/CRH scheme)
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  uint32_t i;

  /* Enable GPIOA/B/C + AFIO clocks */
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN |
                  RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;

  /* Disable JTAG, keep SW-DP -> frees PB3/PB4/PA15.
     ADDR5 is on PB4 (NJTRST) and needs this to work as a GPIO.
     SWD debugging on PA13/PA14 stays available. */
  AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

  /* ---- GPIOA : data bus + green/blue LED = push-pull outputs ------------ */
  {
    const uint8_t out_a[] = {1, 2, 3, 4, 5, 6, 7, 8, 11, 12};
    for (i = 0; i < sizeof(out_a); i++) cfg_pin(GPIOA, out_a[i], GP_OUT_PP_50);
  }
  /* PLAY (PA0) = input floating */
  cfg_pin(GPIOA, 0, GP_IN_FLOAT);
  /* NB: PA13/PA14 left untouched -> remain SWDIO/SWCLK */

  /* ---- GPIOB : AUDIO + SPI (CS/CLK/DI) = push-pull outputs -------------- */
  {
    const uint8_t out_b[] = {1, 12, 13, 15};
    for (i = 0; i < sizeof(out_b); i++) cfg_pin(GPIOB, out_b[i], GP_OUT_PP_50);
  }
  /* Address bus + strobe = inputs with pull-down (ODR bit = 0) */
  {
    const uint8_t in_b[] = {0, 4, 5, 6, 7, 8, 9, 10, 11};
    for (i = 0; i < sizeof(in_b); i++)
    {
      cfg_pin(GPIOB, in_b[i], GP_IN_PUPD);
      GPIOB->ODR &= ~(1U << in_b[i]);   /* 0 -> pull-down */
    }
  }
  /* SPI_DO (PB14) = input floating */
  cfg_pin(GPIOB, 14, GP_IN_FLOAT);

  /* ---- GPIOC : red LED (PC13) = push-pull output ----------------------- */
  cfg_pin(GPIOC, 13, GP_OUT_PP_50);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;
}
#endif /* USE_FULL_ASSERT */
