/****************************************************************************
 * boards/arm/stm32h7/nucleo-h7a3zi-q/include/board.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_ARM_STM32H7_NUCLEO_H7A3ZI_Q_INCLUDE_BOARD_H
#define __BOARDS_ARM_STM32H7_NUCLEO_H7A3ZI_Q_INCLUDE_BOARD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
#  include <stdint.h>
#endif

/* Do not include STM32 H7 header files here */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Clocking *****************************************************************/

/* The Nucleo-144  board provides the following clock sources:
 *
 *   MCO: 8 MHz from MCO output of ST-LINK is used as input clock (default)
 *   X2:  32.768 KHz crystal for LSE
 *   X3:  HSE crystal oscillator (not provided)
 *
 * So we have these clock source available within the STM32
 *
 *   HSI: 16 MHz RC factory-trimmed
 *   LSI: 32 KHz RC
 *   HSE: 8 MHz from MCO output of ST-LINK
 *   LSE: 32.768 kHz
 */

#define STM32_BOARD_XTAL        8000000ul /* ST-LINK MCO */

#define STM32_HSI_FREQUENCY     16000000ul
#define STM32_LSI_FREQUENCY     32000
#define STM32_HSE_FREQUENCY     STM32_BOARD_XTAL
#define STM32_LSE_FREQUENCY     32768

/* Main PLL Configuration.
 *
 * PLL source is HSE = 8,000,000 (from ST-LINK MCO via HSE BYPASS).
 *
 * Reference: STMicroelectronics/STM32CubeH7@ac09376
 *   Projects/NUCLEO-H7A3ZI-Q/Templates/Src/main.c SystemClock_Config()
 *
 * Constraints (RM0455 §8.7.5 + DS13196):
 *
 *     1 <= PLLM <= 63
 *     4 <= PLLN <= 512
 *   150 MHz <= PLL_VCO_MEDIUM <= 420 MHz
 *   128 MHz <= PLL_VCO_WIDE   <= 560 MHz  (H7A3 narrower)
 *   PLLP1   = {2, 4, 6, ..., 128}
 *   CPUCLK <= 280 MHz @ VOS0
 *   HCLK   <= 280 MHz @ VOS0
 *   APBx   <= 140 MHz @ VOS0
 *
 * PLL1 selection (matches ST official Templates):
 *
 *   ref clk  = HSE / PLLM = 8 MHz / 4 = 2 MHz   (PLL1RGE = RANGE_1: 2-4 MHz)
 *   PLL1_VCO = ref_clk * PLLN = 2 * 280 = 560 MHz   (in WIDE)
 *   PLL1P    = PLL1_VCO / 2 = 280 MHz   -> SYSCLK
 *   PLL1Q    = PLL1_VCO / 4 = 140 MHz   -> SPI/USART kernel clock
 *   PLL1R    = PLL1_VCO / 2 = 280 MHz   -> trace clock
 */

#define STM32_BOARD_USEHSE
#define STM32_HSEBYP_ENABLE

#define STM32_PLLCFG_PLLSRC      RCC_PLLCKSELR_PLLSRC_HSE

#define STM32_PLLCFG_PLL1CFG     (RCC_PLLCFGR_PLL1VCOSEL_WIDE | \
                                  RCC_PLLCFGR_PLL1RGE_2_4_MHZ | \
                                  RCC_PLLCFGR_DIVP1EN | \
                                  RCC_PLLCFGR_DIVQ1EN | \
                                  RCC_PLLCFGR_DIVR1EN)
#define STM32_PLLCFG_PLL1M       RCC_PLLCKSELR_DIVM1(4)
#define STM32_PLLCFG_PLL1N       RCC_PLL1DIVR_N1(280)
#define STM32_PLLCFG_PLL1P       RCC_PLL1DIVR_P1(2)
#define STM32_PLLCFG_PLL1Q       RCC_PLL1DIVR_Q1(4)
#define STM32_PLLCFG_PLL1R       RCC_PLL1DIVR_R1(2)

#define STM32_VCO1_FREQUENCY     ((STM32_HSE_FREQUENCY / 4) * 280)
#define STM32_PLL1P_FREQUENCY    (STM32_VCO1_FREQUENCY / 2)
#define STM32_PLL1Q_FREQUENCY    (STM32_VCO1_FREQUENCY / 4)
#define STM32_PLL1R_FREQUENCY    (STM32_VCO1_FREQUENCY / 2)

/* PLL2 */

#define STM32_PLLCFG_PLL2CFG (RCC_PLLCFGR_PLL2VCOSEL_WIDE | \
                              RCC_PLLCFGR_PLL2RGE_4_8_MHZ | \
                              RCC_PLLCFGR_DIVP2EN)
#define STM32_PLLCFG_PLL2M       RCC_PLLCKSELR_DIVM2(2)
#define STM32_PLLCFG_PLL2N       RCC_PLL2DIVR_N2(200)
#define STM32_PLLCFG_PLL2P       RCC_PLL2DIVR_P2(40)
#define STM32_PLLCFG_PLL2Q       0
#define STM32_PLLCFG_PLL2R       0

#define STM32_VCO2_FREQUENCY     ((STM32_HSE_FREQUENCY / 2) * 200)
#define STM32_PLL2P_FREQUENCY    (STM32_VCO2_FREQUENCY / 2)
#define STM32_PLL2Q_FREQUENCY
#define STM32_PLL2R_FREQUENCY

/* PLL3 */

#define STM32_PLLCFG_PLL3CFG 0
#define STM32_PLLCFG_PLL3M   0
#define STM32_PLLCFG_PLL3N   0
#define STM32_PLLCFG_PLL3P   0
#define STM32_PLLCFG_PLL3Q   0
#define STM32_PLLCFG_PLL3R   0

#define STM32_VCO3_FREQUENCY
#define STM32_PLL3P_FREQUENCY
#define STM32_PLL3Q_FREQUENCY
#define STM32_PLL3R_FREQUENCY

/* SYSCLK = PLL1P = 280 MHz
 * CPUCLK = SYSCLK / 1 = 280 MHz   (CDCPRE = DIV1)
 *
 * H7A3 max @ VOS0: SYSCLK 280 MHz, HCLK 280 MHz (NO /2 needed unlike H743),
 * APB 140 MHz.
 */

#define STM32_RCC_D1CFGR_D1CPRE  (RCC_D1CFGR_D1CPRE_SYSCLK)
#define STM32_SYSCLK_FREQUENCY   (STM32_PLL1P_FREQUENCY)
#define STM32_CPUCLK_FREQUENCY   (STM32_SYSCLK_FREQUENCY / 1)

#define STM32_RCC_D1CFGR_HPRE   RCC_D1CFGR_HPRE_SYSCLK
/* HCLK  = SYSCLK = 280 MHz */
#define STM32_ACLK_FREQUENCY    (STM32_SYSCLK_FREQUENCY / 1)
/* ACLK in CD = HCLK */
#define STM32_HCLK_FREQUENCY    (STM32_SYSCLK_FREQUENCY / 1)
/* HCLK = 280 MHz */

#define STM32_RCC_D2CFGR_D2PPRE1  RCC_D2CFGR_D2PPRE1_HCLKd2
/* PCLK1 = HCLK / 2 = 140 MHz */
#define STM32_PCLK1_FREQUENCY     (STM32_HCLK_FREQUENCY/2)

#define STM32_RCC_D2CFGR_D2PPRE2  RCC_D2CFGR_D2PPRE2_HCLKd2
/* PCLK2 = HCLK / 2 = 140 MHz */
#define STM32_PCLK2_FREQUENCY     (STM32_HCLK_FREQUENCY/2)

#define STM32_RCC_D1CFGR_D1PPRE   RCC_D1CFGR_D1PPRE_HCLKd2
/* PCLK3 = HCLK / 2 = 140 MHz */
#define STM32_PCLK3_FREQUENCY     (STM32_HCLK_FREQUENCY/2)

#define STM32_RCC_D3CFGR_D3PPRE   RCC_D3CFGR_D3PPRE_HCLKd2
/* PCLK4 = HCLK / 2 = 140 MHz */
#define STM32_PCLK4_FREQUENCY     (STM32_HCLK_FREQUENCY/2)

/* Timer clock frequencies */

/* Timers driven from APB1 will be twice PCLK1 */

#define STM32_APB1_TIM2_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM3_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM4_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM5_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM6_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM7_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM12_CLKIN  (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM13_CLKIN  (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM14_CLKIN  (2*STM32_PCLK1_FREQUENCY)

/* Timers driven from APB2 will be twice PCLK2 */

#define STM32_APB2_TIM1_CLKIN   (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM8_CLKIN   (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM15_CLKIN  (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM16_CLKIN  (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM17_CLKIN  (2*STM32_PCLK2_FREQUENCY)

/* Kernel Clock Configuration
 *
 * Note: look at Table 54 in ST Manual
 */

/* I2C123 clock source - HSI */

#define STM32_RCC_D2CCIP2R_I2C123SRC RCC_D2CCIP2R_I2C123SEL_HSI

/* I2C4 clock source - HSI */

#define STM32_RCC_D3CCIPR_I2C4SRC    RCC_D3CCIPR_I2C4SEL_HSI

/* SPI123 clock source - PLL1Q */

#define STM32_RCC_D2CCIP1R_SPI123SRC RCC_D2CCIP1R_SPI123SEL_PLL1

/* SPI45 clock source - APB (PCLK2?) */

#define STM32_RCC_D2CCIP1R_SPI45SRC  RCC_D2CCIP1R_SPI45SEL_APB

/* SPI6 clock source - APB (PCLK4) */

#define STM32_RCC_D3CCIPR_SPI6SRC    RCC_D3CCIPR_SPI6SEL_PCLK4

/* USB 1 and 2 clock source - HSI48 */

#define STM32_RCC_D2CCIP2R_USBSRC    RCC_D2CCIP2R_USBSEL_HSI48

/* ADC 1 2 3 clock source - pll2_pclk */

#define STM32_RCC_D3CCIPR_ADCSRC     RCC_D3CCIPR_ADCSEL_PLL2

/* H7A3 power configuration (RM0455 §6 + §4.9.1 Table 17).
 *
 * H7A3 VOS encoding INVERTED relative to H743 (VOS=0b11 -> VOS0 directly,
 * no SYSCFG.ODEN sequence). PWR_D3CR_VOS_SCALE_0 in stm32h7a3xx_pwr.h is
 * already remapped to encode value 0b11 = VOS0 (H7A3 max perf, 280 MHz).
 */

#define STM32_PWR_VOS_SCALE     PWR_D3CR_VOS_SCALE_0
#define STM32_VOS_OVERDRIVE     0

/* FLASH wait states for H7A3 @ VOS0 (RM0455 §4.9.1 Table 17):
 *
 *   HCLK range     | LATENCY (WS) | WRHIGHFREQ
 *   ---------------|--------------|-----------
 *   <=  42 MHz     | 0            | 0b00
 *    42 - 84  MHz  | 1            | 0b00
 *    84 - 126 MHz  | 2            | 0b01
 *   126 - 168 MHz  | 3            | 0b10
 *   168 - 210 MHz  | 4            | 0b10
 *   210 - 252 MHz  | 5            | 0b11
 *   252 - 280 MHz  | 6            | 0b11   <- our config (HCLK = 280 MHz)
 */

#define BOARD_FLASH_WAITSTATES 6
#define BOARD_FLASH_PROGDELAY  3

/* SDMMC definitions ********************************************************/

/* Init 400kHz, PLL1Q/(2*250) */

#define STM32_SDMMC_INIT_CLKDIV     (250 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)

/* Just set these to 25 MHz for now,
 * PLL1Q/(2*4), for default speed 12.5MB/s
 */

#define STM32_SDMMC_MMCXFR_CLKDIV   (4 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)
#define STM32_SDMMC_SDXFR_CLKDIV    (4 << STM32_SDMMC_CLKCR_CLKDIV_SHIFT)

#define STM32_SDMMC_CLKCR_EDGE      STM32_SDMMC_CLKCR_NEGEDGE

/* Ethernet: NUCLEO-H7A3ZI-Q has NO Ethernet PHY/MAGNETICS/RJ45 fitted, and
 * the STM32H7A3 SoC itself has no Ethernet MAC peripheral. Ethernet pin
 * definitions are deliberately omitted; do not attempt to enable
 * STM32H7_HAVE_ETHERNET on this board.
 */

/* LED definitions **********************************************************/

/* The Nucleo-144 board has numerous LEDs but only three, LD1 a Green LED,
 * LD2 a Blue LED and LD3 a Red LED, that can be controlled by software.
 * The following definitions assume the default Solder Bridges are installed.
 *
 * If CONFIG_ARCH_LEDS is not defined, then the user can control the LEDs in
 * any way.
 * The following definitions are used to access individual LEDs.
 */

/* LED index values for use with board_userled() */

#define BOARD_LED1        0
#define BOARD_LED2        1
#define BOARD_LED3        2
#define BOARD_NLEDS       3

#define BOARD_LED_GREEN   BOARD_LED1
#define BOARD_LED_BLUE    BOARD_LED2
#define BOARD_LED_RED     BOARD_LED3

/* LED bits for use with board_userled_all() */

#define BOARD_LED1_BIT    (1 << BOARD_LED1)
#define BOARD_LED2_BIT    (1 << BOARD_LED2)
#define BOARD_LED3_BIT    (1 << BOARD_LED3)

/* If CONFIG_ARCH_LEDS is defined, the usage by the board port is defined in
 * include/board.h and src/stm32_leds.c.
 * The LEDs are used to encode OS-related events as follows:
 *
 *
 *   SYMBOL                     Meaning                      LED state
 *                                                        Red   Green Blue
 *   ----------------------  --------------------------  ------ ------ ---
 */

#define LED_STARTED        0 /* NuttX has been started   OFF    OFF   OFF  */
#define LED_HEAPALLOCATE   1 /* Heap has been allocated  OFF    OFF   ON   */
#define LED_IRQSENABLED    2 /* Interrupts enabled       OFF    ON    OFF  */
#define LED_STACKCREATED   3 /* Idle stack created       OFF    ON    ON   */
#define LED_INIRQ          4 /* In an interrupt          N/C    N/C   GLOW */
#define LED_SIGNAL         5 /* In a signal handler      N/C    GLOW  N/C  */
#define LED_ASSERTION      6 /* An assertion failed      GLOW   N/C   GLOW */
#define LED_PANIC          7 /* The system has crashed   Blink  OFF   N/C  */
#define LED_IDLE           8 /* MCU is is sleep mode     ON     OFF   OFF  */

/* Thus if the Green LED is statically on, NuttX has successfully booted and
 * is, apparently, running normally.  If the Red LED is flashing at
 * approximately 2Hz, then a fatal error has been detected and the system
 * has halted.
 */

/* Button definitions *******************************************************/

/* The NUCLEO board supports one button:  Pushbutton B1, labeled "User", is
 * connected to GPIO PI11.
 * A high value will be sensed when the button is depressed.
 */

#define BUTTON_USER        0
#define NUM_BUTTONS        1
#define BUTTON_USER_BIT    (1 << BUTTON_USER)

/* Alternate function pin selections ****************************************/

/* USART3 (Nucleo Virtual Console) */

#define GPIO_USART3_RX    (GPIO_USART3_RX_3 | GPIO_SPEED_100MHz) /* PD9 */
#define GPIO_USART3_TX    (GPIO_USART3_TX_3 | GPIO_SPEED_100MHz) /* PD8 */

#define DMAMAP_USART3_RX DMAMAP_DMA12_USART3RX_0
#define DMAMAP_USART3_TX DMAMAP_DMA12_USART3TX_1

/* USART6 (Arduino Serial Shield) */

#define GPIO_USART6_RX    (GPIO_USART6_RX_2 | GPIO_SPEED_100MHz) /* PG9 */
#define GPIO_USART6_TX    (GPIO_USART6_TX_2 | GPIO_SPEED_100MHz) /* PG14 */

/* I2C1 Use Nucleo I2C1 pins */

#define GPIO_I2C1_SCL     (GPIO_I2C1_SCL_2 | GPIO_SPEED_50MHz) /* PB8 - D15 */
#define GPIO_I2C1_SDA     (GPIO_I2C1_SDA_2 | GPIO_SPEED_50MHz) /* PB9 - D14 */

/* I2C2 Use Nucleo I2C2 pins */

#define GPIO_I2C2_SCL     (GPIO_I2C2_SCL_2  | GPIO_SPEED_50MHz)
/* PF1 - D69 */
#define GPIO_I2C2_SDA     (GPIO_I2C2_SDA_2  | GPIO_SPEED_50MHz)
/* PF0 - D68 */
#define GPIO_I2C2_SMBA    (GPIO_I2C2_SMBA_2 | GPIO_SPEED_50MHz)
/* PF2 - D70 */

/* SPI3 */

#define GPIO_SPI3_MISO    (GPIO_SPI3_MISO_1 | GPIO_SPEED_50MHz) /* PB4 */
#define GPIO_SPI3_MOSI    (GPIO_SPI3_MOSI_4 | GPIO_SPEED_50MHz) /* PB5 */
#define GPIO_SPI3_SCK     (GPIO_SPI3_SCK_1 | GPIO_SPEED_50MHz)  /* PB3 */
#define GPIO_SPI3_NSS     (GPIO_SPI3_NSS_2 | GPIO_SPEED_50MHz)  /* PA4 */

/* TIM1 */

#define GPIO_TIM1_CH1OUT  (GPIO_TIM1_CH1OUT_2  | GPIO_SPEED_50MHz)
/* PE9  - D6 */
#define GPIO_TIM1_CH1NOUT (GPIO_TIM1_CH1NOUT_3 | GPIO_SPEED_50MHz)
/* PE8  - D42 */
#define GPIO_TIM1_CH2OUT  (GPIO_TIM1_CH2OUT_2  | GPIO_SPEED_50MHz)
/* PE11 - D5 */
#define GPIO_TIM1_CH2NOUT (GPIO_TIM1_CH2NOUT_3 | GPIO_SPEED_50MHz)
/* PE10 - D40 */
#define GPIO_TIM1_CH3OUT  (GPIO_TIM1_CH3OUT_2  | GPIO_SPEED_50MHz)
/* PE13 - D3 */
#define GPIO_TIM1_CH3NOUT (GPIO_TIM1_CH3NOUT_3 | GPIO_SPEED_50MHz)
/* PE12 - D39 */
#define GPIO_TIM1_CH4OUT  (GPIO_TIM1_CH4OUT_2  | GPIO_SPEED_50MHz)
/* PE14 - D38 */

/* OTGFS */

#define GPIO_OTGFS_DM  (GPIO_OTGFS_DM_0  | GPIO_SPEED_100MHz)
#define GPIO_OTGFS_DP  (GPIO_OTGFS_DP_0  | GPIO_SPEED_100MHz)
#define GPIO_OTGFS_ID  (GPIO_OTGFS_ID_0  | GPIO_SPEED_100MHz)

/* DMA **********************************************************************/

#define DMAMAP_SPI3_RX DMAMAP_DMA12_SPI3RX_0 /* DMA1 */
#define DMAMAP_SPI3_TX DMAMAP_DMA12_SPI3TX_0 /* DMA1 */

#define DMAMAP_USART6_RX DMAMAP_DMA12_USART6RX_1
#define DMAMAP_USART6_TX DMAMAP_DMA12_USART6TX_0

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifndef __ASSEMBLY__

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_STM32H7_NUCLEO_H7A3ZI_Q_INCLUDE_BOARD_H */
