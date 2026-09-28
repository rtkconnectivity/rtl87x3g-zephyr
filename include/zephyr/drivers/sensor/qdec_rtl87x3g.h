/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_SENSOR_QDEC_RTL87X3G_H_
#define ZEPHYR_INCLUDE_DRIVERS_SENSOR_QDEC_RTL87X3G_H_

#include <zephyr/drivers/sensor.h>

enum sensor_attribute_qdec_rtl87x3g {
	SENSOR_ATTR_QDEC_PRIV_START = SENSOR_ATTR_PRIV_START,
#if CONFIG_RTL87X3G_QDEC_X_AXIS_ENABLE
	/** Angular rotation of x axis, in degrees */
	SENSOR_ATTR_QDEC_X_ROTATION,
#endif
#if CONFIG_RTL87X3G_QDEC_Y_AXIS_ENABLE
	/** Angular rotation of y axis, in degrees */
	SENSOR_ATTR_QDEC_Y_ROTATION,
#endif
#if CONFIG_RTL87X3G_QDEC_Z_AXIS_ENABLE
	/** Angular rotation of z axis, in degrees */
	SENSOR_ATTR_QDEC_Z_ROTATION,
#endif
};

#endif /* ZEPHYR_INCLUDE_DRIVERS_SENSOR_QDEC_RTL87X3G_H_ */
