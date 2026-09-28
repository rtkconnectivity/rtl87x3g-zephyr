/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_RTL87X3G_PINCTRL_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_RTL87X3G_PINCTRL_H_

#include "realtek-rtl87x3g-pinctrl.h"

/**
 * @name RTL87X2G pinctrl pin functions.
 * @{
 */
#define RTL87X3G_IDLE_MODE          0
#define RTL87X3G_UART4_TX           1
#define RTL87X3G_UART4_RX           2
#define RTL87X3G_UART4_CTS          3
#define RTL87X3G_UART4_RTS          4
#define RTL87X3G_I2C0_CLK           5
#define RTL87X3G_I2C0_DAT           6
#define RTL87X3G_I2C1_CLK           7
#define RTL87X3G_I2C1_DAT           8
#define RTL87X3G_PWM9_P             9
#define RTL87X3G_PWM9_N             10
#define RTL87X3G_PWM4               11
#define RTL87X3G_PWM5               12
#define RTL87X3G_PWM6_P             13
#define RTL87X3G_PWM7_P             14
#define RTL87X3G_PWM8_P             15
#define RTL87X3G_UART3_TX           17
#define RTL87X3G_UART3_RX           18
#define RTL87X3G_UART3_CTS          19
#define RTL87X3G_UART3_RTS          20
#define RTL87X3G_QDEC_PHASE_A_X     21
#define RTL87X3G_QDEC_PHASE_B_X     22
#define RTL87X3G_QDEC_PHASE_A_Y     23
#define RTL87X3G_QDEC_PHASE_B_Y     24
#define RTL87X3G_QDEC_PHASE_A_Z     25
#define RTL87X3G_QDEC_PHASE_B_Z     26
#define RTL87X3G_UART1_TX           27
#define RTL87X3G_UART1_RX           28
#define RTL87X3G_UART2_TX           29
#define RTL87X3G_UART2_RX           30
#define RTL87X3G_UART2_CTS          31
#define RTL87X3G_UART2_RTS          32
#define RTL87X3G_IRDA_TX            33
#define RTL87X3G_IRDA_RX            34
#define RTL87X3G_UART0_TX           35
#define RTL87X3G_UART0_RX           36
#define RTL87X3G_UART0_CTS          37
#define RTL87X3G_UART0_RTS          38
#define RTL87X3G_SPI1_SS_N_0_MASTER 39
#define RTL87X3G_SPI1_SS_N_1_MASTER 40
#define RTL87X3G_SPI1_SS_N_2_MASTER 41
#define RTL87X3G_SPI1_CLK_MASTER    42
#define RTL87X3G_SPI1_MO_MASTER     43
#define RTL87X3G_SPI1_MI_MASTER     44
#define RTL87X3G_SPI_SS_N_0_SLAVE   45
#define RTL87X3G_SPI_CLK_SLAVE      46
#define RTL87X3G_SPI_SO_SLAVE       47
#define RTL87X3G_SPI_SI_SLAVE       48
#define RTL87X3G_SPI0_SS_N_0_MASTER 49
#define RTL87X3G_SPI0_CLK_MASTER    50
#define RTL87X3G_SPI0_MO_MASTER     51
#define RTL87X3G_SPI0_MI_MASTER     52
#define RTL87X3G_PWM6_N             53
#define RTL87X3G_PWM7_N             54
#define RTL87X3G_PWM8_N             55
#define RTL87X3G_SWD_CLK            56
#define RTL87X3G_SWD_DIO            57
#define RTL87X3G_KEY_COL_0          58
#define RTL87X3G_KEY_COL_1          59
#define RTL87X3G_KEY_COL_2          60
#define RTL87X3G_KEY_COL_3          61
#define RTL87X3G_KEY_COL_4          62
#define RTL87X3G_KEY_COL_5          63
#define RTL87X3G_KEY_COL_6          64
#define RTL87X3G_KEY_COL_7          65
#define RTL87X3G_KEY_COL_8          66
#define RTL87X3G_KEY_COL_9          67
#define RTL87X3G_KEY_COL_10         68
#define RTL87X3G_KEY_COL_11         69
#define RTL87X3G_KEY_COL_12         70
#define RTL87X3G_KEY_COL_13         71
#define RTL87X3G_KEY_COL_14         72
#define RTL87X3G_KEY_COL_15         73
#define RTL87X3G_KEY_COL_16         74
#define RTL87X3G_KEY_COL_17         75
#define RTL87X3G_KEY_COL_18         76
#define RTL87X3G_KEY_COL_19         77
#define RTL87X3G_KEY_ROW_0          78
#define RTL87X3G_KEY_ROW_1          79
#define RTL87X3G_KEY_ROW_2          80
#define RTL87X3G_KEY_ROW_3          81
#define RTL87X3G_KEY_ROW_4          82
#define RTL87X3G_KEY_ROW_5          83
#define RTL87X3G_KEY_ROW_6          84
#define RTL87X3G_KEY_ROW_7          85
#define RTL87X3G_KEY_ROW_8          86
#define RTL87X3G_KEY_ROW_9          87
#define RTL87X3G_KEY_ROW_10         88
#define RTL87X3G_KEY_ROW_11         89
#define RTL87X3G_DWGPIO             90
#define RTL87X3G_LRC_SPORT1         91
#define RTL87X3G_BCLK_SPORT1        92
#define RTL87X3G_ADCDAT_SPORT1      93
#define RTL87X3G_DACDAT_SPORT1      94
#define RTL87X3G_DMIC1_CLK          96
#define RTL87X3G_DMIC1_DAT          97
#define RTL87X3G_LRC_I_CODEC_SLAVE  98
#define RTL87X3G_BCLK_I_CODEC_SLAVE 99
#define RTL87X3G_SDI_CODEC_SLAVE    100
#define RTL87X3G_SDO_CODEC_SLAVE    101
#define RTL87X3G_LRC_I_PCM          102
#define RTL87X3G_BCLK_I_PCM         103
#define RTL87X3G_SDI_PCM            104
#define RTL87X3G_SDO_PCM            105
#define RTL87X3G_BT_COEX_I_0        106
#define RTL87X3G_BT_COEX_I_1        107
#define RTL87X3G_BT_COEX_I_2        108
#define RTL87X3G_BT_COEX_I_3        109
#define RTL87X3G_BT_COEX_O_0        110
#define RTL87X3G_BT_COEX_O_1        111
#define RTL87X3G_BT_COEX_O_2        112
#define RTL87X3G_BT_COEX_O_3        113
#define RTL87X3G_PTA_I2C_CLK_SLAVE  114
#define RTL87X3G_PTA_I2C_DAT_SLAVE  115
#define RTL87X3G_PTA_I2C_INT_OUT    116
#define RTL87X3G_DSP_GPIO_OUT       117
#define RTL87X3G_DSP_JTCK           118
#define RTL87X3G_DSP_JTDI           119
#define RTL87X3G_DSP_JTDO           120
#define RTL87X3G_DSP_JTMS           121
#define RTL87X3G_DSP_JTRST          122
#define RTL87X3G_LRC_SPORT0         123
#define RTL87X3G_BCLK_SPORT0        124
#define RTL87X3G_ADCDAT_SPORT0      125
#define RTL87X3G_DACDAT_SPORT0      126
#define RTL87X3G_MCLK_M             127
#define RTL87X3G_SPI0_SS_N_1_MASTER 128
#define RTL87X3G_SPI0_SS_N_2_MASTER 129
#define RTL87X3G_SPI2_SS_N_0_MASTER 130
#define RTL87X3G_SPI2_CLK_MASTER    131
#define RTL87X3G_SPI2_MO_MASTER     132
#define RTL87X3G_SPI2_MI_MASTER     133
#define RTL87X3G_I2C2_CLK           134
#define RTL87X3G_I2C2_DAT           135
#define RTL87X3G_ISO7816_RST        136
#define RTL87X3G_ISO7816_CLK        137
#define RTL87X3G_ISO7816_IO         138
#define RTL87X3G_ISO7816_VCC_EN     139
#define RTL87X3G_UART5_TX           140
#define RTL87X3G_UART5_RX           141
#define RTL87X3G_UART5_CTS          142
#define RTL87X3G_UART5_RTS          143
#define RTL87X3G_DMIC2_CLK          144
#define RTL87X3G_DMIC2_DAT          145
#define RTL87X3G_DMIC3_CLK          146
#define RTL87X3G_DMIC3_DAT          147
#define RTL87X3G_DMIC4_CLK          148
#define RTL87X3G_DMIC4_DAT          149
#define RTL87X3G_CAN0_TX            155
#define RTL87X3G_CAN0_RX            156
#define RTL87X3G_CAN1_TX            157
#define RTL87X3G_CAN1_RX            158
#define RTL87X3G_CAN2_TX            159
#define RTL87X3G_CAN2_RX            160
#define RTL87X3G_EN_EXPA            163
#define RTL87X3G_EN_EXLNA           164
#define RTL87X3G_WL_BT_GNT_I        165
#define RTL87X3G_ANT_CTL_O          166
#define RTL87X3G_UART1_CTS          167
#define RTL87X3G_UART1_RTS          168
#define RTL87X3G_LRC_RX_CODEC_SLAVE 175
#define RTL87X3G_LRC_RX_SPORT0      176
#define RTL87X3G_LRC_RX_SPORT1      177
#define RTL87X3G_CLK_CPUDIVIDEDBY2  180
#define RTL87X3G_CLK_DSPDIVIDEDBY2  181
#define RTL87X3G_PDM_DATA           193
#define RTL87X3G_PDM_CLK            194
#define RTL87X3G_KEY_ROW_12         200
#define RTL87X3G_KEY_ROW_13         201
#define RTL87X3G_KEY_ROW_14         202
#define RTL87X3G_KEY_ROW_15         203
#define RTL87X3G_KEY_ROW_16         204
#define RTL87X3G_KEY_ROW_17         205
#define RTL87X3G_ANT_SW0_AOA_AOD    229
#define RTL87X3G_ANT_SW1_AOA_AOD    230
#define RTL87X3G_ANT_SW2_AOA_AOD    231
#define RTL87X3G_ANT_SW3_AOA_AOD    232
#define RTL87X3G_ANT_SW4_AOA_AOD    233
#define RTL87X3G_ANT_SW5_AOA_AOD    234
#define RTL87X3G_DIGI_DEBUG         255

#define RTL87X3G_PINMUX_MAX (RTL87X3G_DIGI_DEBUG + 1)

#define RTL87X3G_LED0     (RTL87X3G_PINMUX_MAX + 0)
#define RTL87X3G_LED1     (RTL87X3G_PINMUX_MAX + 1)
#define RTL87X3G_LED2     (RTL87X3G_PINMUX_MAX + 2)
#define RTL87X3G_LP_PWM   (RTL87X3G_PINMUX_MAX + 3)
#define RTL87X3G_PWR_OFF  (RTL87X3G_PINMUX_MAX + 4)
#define RTL87X3G_SW_MODE  (RTL87X3G_PINMUX_MAX + 5)
#define RTL87X3G_HS_FUNC0 ((RTL87X3G_SW_MODE + 2) & ~1)
#define RTL87X3G_HS_FUNC1 (RTL87X3G_HS_FUNC0 + 1)

#define RTL87X3G_HS_P1_2_SPI2_MOSI        RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P1_3_SPI2_CSN         RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P1_4_SPI2_CLK         RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P1_5_SPI2_MISO        RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P2_3_LCDC_DE          RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P2_4_LCDC_hsync       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P2_5_LCDC_PCLK        RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P2_6_LCDC_DATA0       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P2_7_LCDC_DATA1       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P3_2_GMAC_RXDV        RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P3_3_GMAC_RXERR       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P4_0_LCDC_DATA2       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P4_2_LCDC_DATA4       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P4_3_LCDC_DATA5       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P4_4_LCDC_DATA6       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P4_5_LCDC_DATA7       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P5_0_SDH0_CLK         RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P5_1_SDH0_CMD         RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P5_2_SDH0_DATA0       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P5_3_SDH0_DATA1       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P5_4_SDH0_DATA2       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P5_5_SDH0_DATA3       RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P6_0_LCDC             RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P6_1_LCDC             RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P6_2_LCDC             RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P8_0_LCDC_DATA10      RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P8_1_LCDC_DATA11      RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P8_2_LCDC_DATA12      RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P8_3_LCDC_DATA13      RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P9_0_LCDC_RESETSD     RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P9_1_LCDC_DATA18      RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P9_2_LCDC_DATA19      RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P9_3_LCDC_DATA20      RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P9_4_LCDC_DATA21      RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_P9_5_LCDC_DATA22      RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_SPIC0_SIO2_SPIC0_SIO2 RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_SPIC0_SIO1_SPIC0_SIO1 RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_SPIC0_CSN_SPIC0_CSN   RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_SPIC0_SIO0_SPIC0_SIO0 RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_SPIC0_SCK_SPIC0_SCK   RTL87X3G_HS_FUNC0
#define RTL87X3G_HS_SPIC0_SIO3_SPIC0_SIO3 RTL87X3G_HS_FUNC0

#define RTL87X3G_HS_P1_2_SDH1_CLK         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P1_3_SDH1_CMD         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P1_4_SDH1_DATA0       RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P1_5_SDH1_DATA1       RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P2_3_SDH0_CLK         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P2_4_SDH0_CMD         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P2_5_SDH0_DATA0       RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P2_6_SDH0_DATA1       RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P2_7_SDH0_DATA2       RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P3_2_SDH1_DATA2       RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P3_3_SDH1_DATA3       RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P4_0_SDH0_DATA3       RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P4_2_SPI1_CLK         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P4_3_SPI1_MISO        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P4_4_SPI1_MOSI        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P4_5_SPI1_CSN         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P5_0_GMAC_TXDEN       RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P5_1_GMAC_TXD1        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P5_2_GMAC_TXD0        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P5_3_GMAC_REFCLK      RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P5_4_GMAC_RXD1        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P5_5_GMAC_RXD0        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P6_0_SDH0             RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P6_1_SDH0             RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P6_2_SDH0             RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P8_0_SPI0_CLK         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P8_1_SPI0_MISO        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P8_2_SPI0_MOSI        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P8_3_SPI0_CSN         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P9_0_SPI2_SIO2        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P9_1_SPI2_SIO1        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P9_2_SPI2_CSN         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P9_3_SPI2_SIO0        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P9_4_SPI2_SCK         RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_P9_5_SPI2_SIO3        RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_SPIC0_SIO2_SDH1_CLK   RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_SPIC0_SIO1_SDH1_DATA3 RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_SPIC0_CSN_SDH1_DATA2  RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_SPIC0_SIO0_SDH1_CMD   RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_SPIC0_SCK_SDH1_DATA0  RTL87X3G_HS_FUNC1
#define RTL87X3G_HS_SPIC0_SIO3_SDH1_DATA1 RTL87X3G_HS_FUNC1

/** @} */

#define ADC_0      0   /*!< GPIOA0 */
#define ADC_1      1   /*!< GPIOA1 */
#define ADC_2      2   /*!< GPIOA2 */
#define ADC_3      3   /*!< GPIOA3 */
#define ADC_4      4   /*!< GPIOB21 */
#define ADC_5      5   /*!< GPIOB22 */
#define ADC_6      6   /*!< GPIOB23 */
#define ADC_7      7   /*!< GPIOB24 */
#define P1_0       8   /*!< GPIOA7 */
#define P1_1       9   /*!< GPIOA8 */
#define P1_2       10  /*!< GPIOA9 */
#define P1_3       11  /*!< GPIOA10 */
#define P1_4       12  /*!< GPIOA11 */
#define P1_5       13  /*!< GPIOA12 */
#define P2_0       14  /*!< GPIOA15 */
#define P2_1       15  /*!< GPIOA16 */
#define P2_2       16  /*!< GPIOA17 */
#define P2_3       17  /*!< GPIOA18 */
#define P2_4       18  /*!< GPIOA19 */
#define P2_5       19  /*!< GPIOA20 */
#define P2_6       20  /*!< GPIOA21 */
#define P2_7       21  /*!< GPIOA22 */
#define P3_0       22  /*!< GPIOA23 */
#define P3_1       23  /*!< GPIOA24 */
#define P3_2       24  /*!< GPIOA4 */
#define P3_3       25  /*!< GPIOA5 */
#define P3_4       26  /*!< GPIOA6 */
#define P3_5       27  /*!< GPIOB25 */
#define P4_0       28  /*!< GPIOB7 */
#define P4_1       29  /*!< GPIOB8 */
#define P4_2       30  /*!< GPIOB9 */
#define P4_3       31  /*!< GPIOB10 */
#define P4_4       32  /*!< GPIOB11 */
#define P4_5       33  /*!< GPIOB12 */
#define P4_6       34  /*!< GPIOB13 */
#define P4_7       35  /*!< GPIOB14 */
#define P5_0       36  /*!< GPIOB15 */
#define P5_1       37  /*!< GPIOB16 */
#define P5_2       38  /*!< GPIOB17 */
#define P5_3       39  /*!< GPIOB18 */
#define P5_4       40  /*!< GPIOA13 */
#define P5_5       41  /*!< GPIOA14 */
#define P5_6       42  /*!< GPIOA15 */
#define P6_0       43  /*!< GPIOA25 */
#define P6_1       44  /*!< GPIOA26 */
#define P6_2       45  /*!< GPIOA27 */
#define P6_3       46  /*!< GPIOA28 */
#define P6_4       47  /*!< GPIOA29 */
#define P7_0       48  /*!< GPIOB15 */
#define P7_1       49  /*!< GPIOB16 */
#define P7_2       50  /*!< GPIOB17 */
#define P7_3       51  /*!< GPIOB18 */
#define P7_4       52  /*!< GPIOB19 */
#define P7_5       53  /*!< GPIOB20 */
#define P7_6       54  /*!< GPIOB21 */
#define SPIC1_SIO2 55  /*!< GPIOB23 */
#define SPIC1_SIO1 56  /*!< GPIOB24 */
#define SPIC1_CSN  57  /*!< GPIOB25 */
#define SPIC1_SIO0 58  /*!< GPIOB26 */
#define SPIC1_SCK  59  /*!< GPIOB27 */
#define SPIC1_SIO3 60  /*!< GPIOB28 */
#define P8_0       61  /*!< GPIOB26 */
#define P8_1       62  /*!< GPIOB27 */
#define P8_2       63  /*!< GPIOB28 */
#define P8_3       64  /*!< GPIOB29 */
#define P8_4       65  /*!< GPIOB30 */
#define P8_5       66  /*!< GPIOB31 */
#define P8_6       67  /*!< GPIOB19 */
#define P8_7       68  /*!< GPIOB20 */
#define MIC1_P     69  /*!< GPIOA25 */
#define MIC1_N     70  /*!< GPIOA26 */
#define MIC2_P     71  /*!< GPIOA27 */
#define MIC2_N     72  /*!< GPIOA28 */
#define MICBIAS1   73  /*!< GPIOA29 */
#define MICBIAS2   74  /*!< GPIOB12 */
#define DAOUT1_P   75  /*!< GPIOA30 */
#define DAOUT1_N   76  /*!< GPIOA31 */
#define DAOUT2_P   77  /*!< GPIOB13 */
#define DAOUT2_N   78  /*!< GPIOB14 */
#define P9_0       79  /*!< GPIOB0 */
#define P9_1       80  /*!< GPIOB1 */
#define P9_2       81  /*!< GPIOB2 */
#define P9_3       82  /*!< GPIOB3 */
#define P9_4       83  /*!< GPIOB4 */
#define P9_5       84  /*!< GPIOB5 */
#define P9_6       85  /*!< GPIOB6 */
#define P10_0      86  /*!< GPIOB29 */
#define P10_1      87  /*!< GPIOB30 */
#define P10_2      88  /*!< GPIOB31 */
#define P10_3      89  /*!< GPIOA10 */
#define P10_4      90  /*!< GPIOA11 */
#define P10_5      91  /*!< GPIOA12 */
#define P10_6      92  /*!< GPIOA13 */
#define SPIC3_SIO2 93  /*!< GPIOA14 */
#define SPIC3_SIO1 94  /*!< GPIOB22 */
#define SPIC3_CSN  95  /*!< GPIOA16 */
#define SPIC3_SIO0 96  /*!< GPIOA17 */
#define SPIC3_SCK  97  /*!< GPIOA30 */
#define SPIC3_SIO3 98  /*!< GPIOA31 */
#define LOUT_P     99  /*!< GPIOB7 */
#define LOUT_N     100 /*!< GPIOB8 */
#define ROUT_P     101 /*!< GPIOB9 */
#define ROUT_N     102 /*!< GPIOB10 */

/* No pinmux functions */
#define SPIC0_SIO2 103
#define SPIC0_SIO1 104
#define SPIC0_CSN  105
#define SPIC0_SIO0 106
#define SPIC0_SCK  107
#define SPIC0_SIO3 108

#define RTL87X3G_DIR_IN    0
#define RTL87X3G_DIR_OUT   1
#define RTL87X3G_DRV_LOW   0
#define RTL87X3G_DRV_HIGH  1
#define RTL87X3G_PULL_DOWN 0
#define RTL87X3G_PULL_UP   1
#define RTL87X3G_PULL_NONE 2

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_RTL87X3G_PINCTRL_H_ */
