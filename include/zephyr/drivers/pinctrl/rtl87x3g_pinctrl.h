/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_PINCTRL_RTL87X3G_H_
#define ZEPHYR_INCLUDE_DRIVERS_PINCTRL_RTL87X3G_H_

#include <zephyr/drivers/pinctrl.h>

/*
 * Custom pinctrl state used by the RTL87X3G peripheral drivers (I2C, SPI,
 * SDHC) to park their pins while the bus is turned off at runtime. The value
 * only has to be unique among the custom states of a single device, so all
 * drivers can share the same ID. It is offset by 2 so it does not collide with
 * the SDHC driver's PINCTRL_STATE_INTERRUPT (PINCTRL_STATE_PRIV_START + 1).
 */
#define PINCTRL_STATE_BUS_OFF (PINCTRL_STATE_PRIV_START + 2)

#endif /* ZEPHYR_INCLUDE_DRIVERS_PINCTRL_RTL87X3G_H_ */
