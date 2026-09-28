/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_DMA_RTL87X3G_DMA_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_DMA_RTL87X3G_DMA_H_

/*
 * DMA config cell bit field definitions.
 *
 * The third cell of a "dmas" property is a 32-bit mask. Use these
 * definitions to compose the value instead of writing raw hex numbers.
 *
 *
 * All seven fields (direction, src/dst address mode, src/dst data width,
 * src/dst msize, priority) must be specified in every dmas entry.
 *
 * Example:
 *   #include <zephyr/dt-bindings/dma/rtl87x3g-dma.h>
 *   dmas = <&dma0 2 RTL87X3G_DMA_HANDSHAKE_UART2_RX
 *                    (RTL87X3G_DMA_P2M
 *                     | RTL87X3G_DMA_SRC_FIXED
 *                     | RTL87X3G_DMA_DST_INC
 *                     | RTL87X3G_DMA_SRC_WIDTH_8BIT
 *                     | RTL87X3G_DMA_DST_WIDTH_8BIT
 *                     | RTL87X3G_DMA_SRC_MSIZE(RTL87X3G_DMA_MSIZE_1)
 *                     | RTL87X3G_DMA_DST_MSIZE(RTL87X3G_DMA_MSIZE_1))>;
 */

/* Direction — bit [1:0] */
#define RTL87X3G_DMA_M2M		0
#define RTL87X3G_DMA_M2P		1
#define RTL87X3G_DMA_P2M		2

/* Source address increment — bit [3:2] */
#define RTL87X3G_DMA_SRC_INC		(0 << 2)
#define RTL87X3G_DMA_SRC_DEC		(1 << 2)
#define RTL87X3G_DMA_SRC_FIXED		(2 << 2)

/* Destination address increment — bit [5:4] */
#define RTL87X3G_DMA_DST_INC		(0 << 4)
#define RTL87X3G_DMA_DST_DEC		(1 << 4)
#define RTL87X3G_DMA_DST_FIXED		(2 << 4)

/* Source data width — bit [7:6] */
#define RTL87X3G_DMA_SRC_WIDTH_8BIT	(0 << 6)
#define RTL87X3G_DMA_SRC_WIDTH_16BIT	(1 << 6)
#define RTL87X3G_DMA_SRC_WIDTH_32BIT	(2 << 6)

/* Destination data width — bit [9:8] */
#define RTL87X3G_DMA_DST_WIDTH_8BIT	(0 << 8)
#define RTL87X3G_DMA_DST_WIDTH_16BIT	(1 << 8)
#define RTL87X3G_DMA_DST_WIDTH_32BIT	(2 << 8)

/*
 * Burst size (msize) encodings — bits [12:10] for source, [15:13] for dest.
 * The encoding value is, e.g. RTL87X3G_DMA_MSIZE_4 expands to (1 << 10).
 */
#define RTL87X3G_DMA_SRC_MSIZE(n)	((n) << 10)
#define RTL87X3G_DMA_DST_MSIZE(n)	((n) << 13)

/* Named burst-size values for use with SRC_MSIZE / DST_MSIZE */
#define RTL87X3G_DMA_MSIZE_1		0
#define RTL87X3G_DMA_MSIZE_4		1
#define RTL87X3G_DMA_MSIZE_8		2
#define RTL87X3G_DMA_MSIZE_16		3
#define RTL87X3G_DMA_MSIZE_32		4
#define RTL87X3G_DMA_MSIZE_64		5
#define RTL87X3G_DMA_MSIZE_128		6
#define RTL87X3G_DMA_MSIZE_256		7

/* Priority — bit [20:16], valid range 0–15 */
#define RTL87X3G_DMA_PRIORITY(n)	((n) << 16)

/*
 * Handshake (slot) IDs — the second cell of a "dmas" entry.
 *
 * Values match the GDMA_Handshake_ defines in rtl876x_gdma_def.h.
 * Use these in the slot cell instead of raw numbers:
 *
 *   dmas = <&dma0 2 RTL87X3G_DMA_HANDSHAKE_UART2_RX (...)>;
 */
#define RTL87X3G_DMA_HANDSHAKE_UART0_TX		0
#define RTL87X3G_DMA_HANDSHAKE_UART0_RX		1
#define RTL87X3G_DMA_HANDSHAKE_UART2_TX		2
#define RTL87X3G_DMA_HANDSHAKE_UART2_RX		3
#define RTL87X3G_DMA_HANDSHAKE_SPI0_TX		4
#define RTL87X3G_DMA_HANDSHAKE_SPI0_RX		5
#define RTL87X3G_DMA_HANDSHAKE_SPI1_TX		6
#define RTL87X3G_DMA_HANDSHAKE_SPI1_RX		7
#define RTL87X3G_DMA_HANDSHAKE_I2C0_TX		8
#define RTL87X3G_DMA_HANDSHAKE_I2C0_RX		9
#define RTL87X3G_DMA_HANDSHAKE_I2C1_TX		10
#define RTL87X3G_DMA_HANDSHAKE_I2C1_RX		11
#define RTL87X3G_DMA_HANDSHAKE_ADC_RX		12
#define RTL87X3G_DMA_HANDSHAKE_AES_TX		13
#define RTL87X3G_DMA_HANDSHAKE_AES_RX		14
#define RTL87X3G_DMA_HANDSHAKE_UART1_TX		15
#define RTL87X3G_DMA_HANDSHAKE_TDM0_TX		16
#define RTL87X3G_DMA_HANDSHAKE_TDM0_RX		17
#define RTL87X3G_DMA_HANDSHAKE_TDM1_TX		18
#define RTL87X3G_DMA_HANDSHAKE_TDM1_RX		19
#define RTL87X3G_DMA_HANDSHAKE_UART1_RX		20
#define RTL87X3G_DMA_HANDSHAKE_SPIC0_TX		21
#define RTL87X3G_DMA_HANDSHAKE_SPIC0_RX		22
#define RTL87X3G_DMA_HANDSHAKE_CANBUS0_RX	23
#define RTL87X3G_DMA_HANDSHAKE_CANBUS2_RX	24
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH6_TRX	25
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH4		26
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH5		27
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH6		28
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH7		29
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH8		30
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH9		31
#define RTL87X3G_DMA_HANDSHAKE_SPIC1_TX		32
#define RTL87X3G_DMA_HANDSHAKE_SPIC1_RX		33
#define RTL87X3G_DMA_HANDSHAKE_SPIC2_TX		34
#define RTL87X3G_DMA_HANDSHAKE_SPIC2_RX		35
#define RTL87X3G_DMA_HANDSHAKE_I2C2_TX		36
#define RTL87X3G_DMA_HANDSHAKE_I2C2_RX		37
#define RTL87X3G_DMA_HANDSHAKE_SPI2_TX		38
#define RTL87X3G_DMA_HANDSHAKE_SPI2_RX		39
#define RTL87X3G_DMA_HANDSHAKE_AUDIO_RX		40
#define RTL87X3G_DMA_HANDSHAKE_TDM2_RX		42
#define RTL87X3G_DMA_HANDSHAKE_IDU_RX		43
#define RTL87X3G_DMA_HANDSHAKE_IDU_TX		44
#define RTL87X3G_DMA_HANDSHAKE_SM3		46
#define RTL87X3G_DMA_HANDSHAKE_UART3_TX		48
#define RTL87X3G_DMA_HANDSHAKE_UART3_RX		49
#define RTL87X3G_DMA_HANDSHAKE_UART4_TX		50
#define RTL87X3G_DMA_HANDSHAKE_UART4_RX		51
#define RTL87X3G_DMA_HANDSHAKE_UART5_TX		52
#define RTL87X3G_DMA_HANDSHAKE_UART5_RX		53
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH7_TRX	54
#define RTL87X3G_DMA_HANDSHAKE_SPI_SLAVE_TX	55
#define RTL87X3G_DMA_HANDSHAKE_SPI_SLAVE_RX	56
#define RTL87X3G_DMA_HANDSHAKE_CANBUS1_RX	57
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH9_TRX	58
#define RTL87X3G_DMA_HANDSHAKE_TIM1_CH8_TRX	59
#define RTL87X3G_DMA_HANDSHAKE_SPIC3_TX		60
#define RTL87X3G_DMA_HANDSHAKE_SPIC3_RX		61
#define RTL87X3G_DMA_HANDSHAKE_IR_TX		62
#define RTL87X3G_DMA_HANDSHAKE_IR_RX		63

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_DMA_RTL87X3G_DMA_H_ */
