/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_REALTEK_RTL87X3G_PINCTRL_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_REALTEK_RTL87X3G_PINCTRL_H_

/*
 * The whole RTL87X3G pin configuration information is encoded in a 32-bit bitfield
 * organized as follows:
 *
 * - 31..16: Pin function.
 * - 15:     Reserved.
 * - 14:     Pin direction configuration.
 * - 13:     Pin output drive configuration.
 * - 12..11: Pin pull configuration.
 * - 10..0:  Pin number (combination of port and pin).
 */

/**
 * @name RTL87X3G pin configuration bit field positions and masks.
 * @{
 */

/** Position of the function field. */
#define RTL87X3G_FUN_POS   16U
/** Mask for the function field. */
#define RTL87X3G_FUN_MSK   0xFFFFU
/** Position of the direction field. */
#define RTL87X3G_DIR_POS   14U
/** Mask for the low direction field. */
#define RTL87X3G_DIR_MSK   0x1U
/** Position of the drive configuration field. */
#define RTL87X3G_DRIVE_POS 13U
/** Mask for the drive configuration field. */
#define RTL87X3G_DRIVE_MSK 0x1U
/** Position of the pull configuration field. */
#define RTL87X3G_PULL_POS  11U
/** Mask for the pull configuration field. */
#define RTL87X3G_PULL_MSK  0x3U
/** Position of the pin field. */
#define RTL87X3G_PIN_POS   0U
/** Mask for the pin field. */
#define RTL87X3G_PIN_MSK   0x7FFU

/** @} */

/**
 * @brief Utility macro to build RTL87X3G psels property entry.
 *
 * @param fun Pin function configuration (see RTL87X3G_FUNC_{name} macros).
 * @param pin Pin .
 */
#define RTL87X3G_PSEL(fun, pin, dir, drive, pull)                                                  \
	(((((pin) & RTL87X3G_PIN_MSK) << RTL87X3G_PIN_POS) |                                       \
	  (((RTL87X3G_##fun) & RTL87X3G_FUN_MSK) << RTL87X3G_FUN_POS)) |                           \
	 ((((RTL87X3G_##dir) & RTL87X3G_DIR_MSK) << RTL87X3G_DIR_POS) |                            \
	  (((RTL87X3G_##drive) & RTL87X3G_DRIVE_MSK) << RTL87X3G_DRIVE_POS) |                      \
	  (((RTL87X3G_##pull) & RTL87X3G_PULL_MSK) << RTL87X3G_PULL_POS)))

/**
 * @brief Utility macro to build RTL87X3G psels property entry when a pin is disconnected.
 *
 * This can be useful in situations where code running before Zephyr, e.g. a bootloader
 * configures pins that later needs to be disconnected.
 *
 * @param fun Pin function configuration (see RTL87X3G_FUNC_{name} macros).
 */
#define RTL87X3G_PSEL_DISCONNECTED(fun)                                                            \
	(RTL87X3G_PIN_DISCONNECTED << RTL87X3G_PIN_POS |                                           \
	 ((RTL87X3G_##fun & RTL87X3G_FUN_MSK) << RTL87X3G_FUN_POS))

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_REALTEK_RTL87X3G_PINCTRL_H_ */
