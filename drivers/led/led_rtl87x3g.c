/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define DT_DRV_COMPAT realtek_rtl87x3g_led
/**
 * @brief Driver for sleep led on RTL87X3G family processor.
 * @note  Please validate for newly added series.
 */
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/dt-bindings/led/led.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <rtl876x_sleep_led.h>
#include <trace.h>
#include "rtl876x_pinmux.h"

LOG_MODULE_REGISTER(led_rtl87x3g, CONFIG_LED_LOG_LEVEL);
#define RTL87X3G_SLEEP_LED_SRC_CLK_HZ 32000
struct rtl87x3g_led_info {
	const struct led_info info;
	const struct pinctrl_dev_config *pcfg;
	const bool active_low;
	const uint16_t prescale;
	const uint32_t total_cnt;
};

struct rtl87x3g_config {
	uint8_t num_leds;
	const struct rtl87x3g_led_info *leds_info;
};

static SLEEP_LED_CHANNEL rtl87x3g_sleep_channels[] = {LED_CHANNEL_0, LED_CHANNEL_1, LED_CHANNEL_2};

static const struct led_info *rtl87x3g_led_to_info(const struct rtl87x3g_config *config,
						   uint32_t led)
{
	if (led < config->num_leds) {
		return &config->leds_info[led].info;
	}
	return NULL;
}

static int rtl87x3g_set_brightness(const struct device *dev, uint32_t led, uint8_t value)
{
	const struct rtl87x3g_config *config = dev->config;
	const struct led_info *led_info = rtl87x3g_led_to_info(config, led);
	SLEEP_LED_InitTypeDef Led_Initsturcture;

	if (!led_info) {
		LOG_ERR("no valid led info");
		return -ENODEV;
	}
	if (value > 100) {
		LOG_ERR("brightness should be between 0 and 100");
		return -EINVAL;
	}

	SleepLed_StructInit(&Led_Initsturcture);
	if (value == 0 || value == 100) {
		Led_Initsturcture.period_high[0] = 0;
		Led_Initsturcture.period_low[0] = 0;
	} else {
		Led_Initsturcture.period_high[0] = config->leds_info[led].total_cnt * value / 100;
		Led_Initsturcture.period_low[0] =
			config->leds_info[led].total_cnt - Led_Initsturcture.period_high[0];
	}
	Led_Initsturcture.mode = LED_BLINK_MODE;
	SleepLed_Reset();
	if (led_info->num_colors == 1) {
		Led_Initsturcture.polarity =
			config->leds_info[led].active_low ? LED_OUTPUT_INVERT : LED_OUTPUT_NORMAL;
		Led_Initsturcture.prescale = config->leds_info[led].prescale;
		if (value == 0) {
			SleepLed_SetIdleMode(rtl87x3g_sleep_channels[led],
					     config->leds_info[led].active_low ? LED_IDLE_HIGH
									       : LED_IDLE_LOW);
		} else if (value == 100) {
			SleepLed_SetIdleMode(rtl87x3g_sleep_channels[led],
					     config->leds_info[led].active_low ? LED_IDLE_LOW
									       : LED_IDLE_HIGH);
		}
		SleepLed_Init(rtl87x3g_sleep_channels[led], &Led_Initsturcture);
		SleepLed_Cmd(rtl87x3g_sleep_channels[led], ENABLE);
	} else if (led_info->num_colors == 3) {
		for (uint8_t i = 0; i < led_info->num_colors; i++) {
			Led_Initsturcture.polarity = config->leds_info[led].active_low
							     ? LED_OUTPUT_INVERT
							     : LED_OUTPUT_NORMAL;
			Led_Initsturcture.prescale = config->leds_info->prescale;
			if (value == 0) {
				SleepLed_SetIdleMode(rtl87x3g_sleep_channels[i],
						     config->leds_info[led].active_low
							     ? LED_IDLE_HIGH
							     : LED_IDLE_LOW);
			} else if (value == 100) {
				SleepLed_SetIdleMode(rtl87x3g_sleep_channels[i],
						     config->leds_info[led].active_low
							     ? LED_IDLE_LOW
							     : LED_IDLE_HIGH);
			}
			SleepLed_Init(rtl87x3g_sleep_channels[i], &Led_Initsturcture);
		}
		SleepLed_Cmd(rtl87x3g_sleep_channels[0] | rtl87x3g_sleep_channels[1] |
				     rtl87x3g_sleep_channels[2],
			     ENABLE);
	} else {
		LOG_ERR("color num should be 1 or 3");
		return -ENOTSUP;
	}
	return 0;
}

static int rtl87x3g_led_on(const struct device *dev, uint32_t led)
{
	return rtl87x3g_set_brightness(dev, led, 100);
}

static int rtl87x3g_led_off(const struct device *dev, uint32_t led)
{
	return rtl87x3g_set_brightness(dev, led, 0);
}

static int rtl87x3g_blink(const struct device *dev, uint32_t led, uint32_t delay_on,
			  uint32_t delay_off)
{
	const struct rtl87x3g_config *config = dev->config;
	const struct led_info *led_info = rtl87x3g_led_to_info(config, led);
	SLEEP_LED_InitTypeDef Led_Initsturcture;
	uint32_t prescale;

	if (delay_on == 0) {
		return rtl87x3g_led_off(dev, led);
	} else if (delay_off == 0) {
		return rtl87x3g_led_on(dev, led);
	}
	if (!led_info) {
		LOG_ERR("no valid led info");
		return -ENODEV;
	}
	uint32_t max = MAX(delay_on, delay_off);
	uint32_t min = MIN(delay_on, delay_off);

	/* Since the values for delay_on and delay_off may be provided in different scales
	 * and period_high and period_low are uint8_t, the prescaler needs to be dynamically
	 * adjusted according to these values.
	 */

	if (max / min > 0xff) {
		LOG_ERR("delay_on and delay_off should not differ so much. ");
		return false;
	}
	SleepLed_Reset();
	SleepLed_StructInit(&Led_Initsturcture);

	if (max <= 0xff) {
		uint32_t cnt_per_ms;

		prescale = 32;
		cnt_per_ms = RTL87X3G_SLEEP_LED_SRC_CLK_HZ / prescale / MSEC_PER_SEC;
		Led_Initsturcture.period_high[0] = delay_on * cnt_per_ms;
		Led_Initsturcture.period_low[0] = delay_off * cnt_per_ms;
	} else {
		uint32_t ms_per_cnt;

		prescale = (max * MSEC_PER_SEC / RTL87X3G_SLEEP_LED_SRC_CLK_HZ) << 3;
		Led_Initsturcture.period_high[0] =
			delay_on * RTL87X3G_SLEEP_LED_SRC_CLK_HZ / (prescale * MSEC_PER_SEC);
		Led_Initsturcture.period_low[0] =
			delay_off * RTL87X3G_SLEEP_LED_SRC_CLK_HZ / (prescale * MSEC_PER_SEC);
	}

	Led_Initsturcture.polarity =
		config->leds_info[led].active_low ? LED_OUTPUT_INVERT : LED_OUTPUT_NORMAL;
	Led_Initsturcture.prescale = prescale;
	Led_Initsturcture.mode = LED_BLINK_MODE;
	if (led_info->num_colors == 1) {
		SleepLed_Init(rtl87x3g_sleep_channels[led], &Led_Initsturcture);
		SleepLed_Cmd(rtl87x3g_sleep_channels[led], ENABLE);
	} else if (led_info->num_colors == 3) {
		for (uint8_t i = 0; i < led_info->num_colors; i++) {
			SleepLed_Init(rtl87x3g_sleep_channels[i], &Led_Initsturcture);
		}
		SleepLed_Cmd(rtl87x3g_sleep_channels[0] | rtl87x3g_sleep_channels[1] |
				     rtl87x3g_sleep_channels[2],
			     ENABLE);
	} else {
		LOG_ERR("color num should be 1 or 3");
		return -ENOTSUP;
	}
	return 0;
}

static int rtl87x3g_get_info(const struct device *dev, uint32_t led, const struct led_info **info)
{
	const struct rtl87x3g_config *config = dev->config;
	const struct led_info *led_info = rtl87x3g_led_to_info(config, led);
	if (!led_info) {
		LOG_ERR("no valid led info");
		return -EINVAL;
	}
	*info = led_info;
	return 0;
}

static int rtl87x3g_set_color(const struct device *dev, uint32_t led, uint8_t num_colors,
			      const uint8_t *color)
{
	const struct rtl87x3g_config *config = dev->config;
	const struct led_info *led_info = rtl87x3g_led_to_info(config, led);
	SLEEP_LED_InitTypeDef Led_Initsturcture;
	uint8_t color_val;
	if (!led_info) {
		LOG_ERR("no valid led info");
		return -ENODEV;
	}
	if (led_info->num_colors != 3) {
		LOG_ERR("color num should be 3");
		return -ENOTSUP;
	}
	if (num_colors != 3) {
		LOG_ERR("color num should be 3");
		return -EINVAL;
	}
	SleepLed_StructInit(&Led_Initsturcture);
	Led_Initsturcture.mode = LED_BLINK_MODE;
	SleepLed_Reset();
	for (uint8_t i = 0; i < led_info->num_colors; i++) {
		switch (led_info->color_mapping[i]) {
		case LED_COLOR_ID_RED:
			color_val = color[0];
			break;
		case LED_COLOR_ID_GREEN:
			color_val = color[1];
			break;
		case LED_COLOR_ID_BLUE:
			color_val = color[2];
			break;
		default:
			return -ENODEV;
		}
		Led_Initsturcture.polarity =
			config->leds_info[led].active_low ? LED_OUTPUT_INVERT : LED_OUTPUT_NORMAL;
		Led_Initsturcture.prescale = config->leds_info[led].prescale;

		if (color_val == 0) {
			Led_Initsturcture.period_high[0] = 0;
			Led_Initsturcture.period_low[0] = 0;
			SleepLed_SetIdleMode(rtl87x3g_sleep_channels[i],
					     config->leds_info[led].active_low ? LED_IDLE_HIGH
									       : LED_IDLE_LOW);
		} else if (color_val == 0xff) {
			Led_Initsturcture.period_high[0] = 0;
			Led_Initsturcture.period_low[0] = 0;
			SleepLed_SetIdleMode(rtl87x3g_sleep_channels[i],
					     config->leds_info[led].active_low ? LED_IDLE_LOW
									       : LED_IDLE_HIGH);
		} else {
			Led_Initsturcture.period_high[0] =
				config->leds_info[led].total_cnt * color_val / 0xff;
			Led_Initsturcture.period_low[0] =
				config->leds_info[led].total_cnt - Led_Initsturcture.period_high[0];
		}

		SleepLed_Init(rtl87x3g_sleep_channels[i], &Led_Initsturcture);
	}
	SleepLed_Cmd(rtl87x3g_sleep_channels[0] | rtl87x3g_sleep_channels[1] |
			     rtl87x3g_sleep_channels[2],
		     ENABLE);
	return 0;
}

static const struct led_driver_api rtl87x3g_led_api = {
	.on = rtl87x3g_led_on,
	.off = rtl87x3g_led_off,
	.blink = rtl87x3g_blink,
	.get_info = rtl87x3g_get_info,
	.set_brightness = rtl87x3g_set_brightness,
	.set_color = rtl87x3g_set_color,
};

static int rtl87x3g_led_init(const struct device *dev)
{
	const struct rtl87x3g_config *config = dev->config;
	SLEEP_LED_InitTypeDef Led_Initsturcture;

	SleepLed_StructInit(&Led_Initsturcture);
	SleepLed_Reset();
	for (uint8_t i = 0; i < config->num_leds; i++) {
		/* init each led node */
		if (config->leds_info[i].info.num_colors == 1) {
			pinctrl_apply_state(config->leds_info[i].pcfg, PINCTRL_STATE_DEFAULT);
			SleepLed_Init(rtl87x3g_sleep_channels[config->leds_info[i].info.index],
				      &Led_Initsturcture);
			SleepLed_SetIdleMode(
				rtl87x3g_sleep_channels[config->leds_info[i].info.index],
				config->leds_info[i].active_low ? LED_IDLE_HIGH : LED_IDLE_LOW);
			SleepLed_Cmd(rtl87x3g_sleep_channels[config->leds_info[i].info.index],
				     DISABLE);
		} else if (config->leds_info[i].info.num_colors == 3) {
			pinctrl_apply_state(config->leds_info[i].pcfg, PINCTRL_STATE_DEFAULT);
			for (uint8_t j = 0; j < config->leds_info[i].info.num_colors; j++) {
				SleepLed_Init(rtl87x3g_sleep_channels[j], &Led_Initsturcture);
				SleepLed_SetIdleMode(rtl87x3g_sleep_channels[j],
						     config->leds_info[i].active_low
							     ? LED_IDLE_HIGH
							     : LED_IDLE_LOW);
			}
			SleepLed_Cmd(rtl87x3g_sleep_channels[0] | rtl87x3g_sleep_channels[1] |
					     rtl87x3g_sleep_channels[2],
				     DISABLE);
		}
	}

	return 0;
}

#define COLOR_MAPPING(led_node_id)                                                                 \
	static const uint8_t color_mapping_##led_node_id[] = DT_PROP(led_node_id, color_mapping);
#define PINCTRL_DEFINE(led_node_id) PINCTRL_DT_DEFINE(led_node_id);
#define LED_INFO(led_node_id)                                                                      \
	{.info =                                                                                   \
		 {                                                                                 \
			 .label = DT_PROP(led_node_id, label),                                     \
			 .index = DT_PROP(led_node_id, index),                                     \
			 .num_colors = DT_PROP_LEN(led_node_id, color_mapping),                    \
			 .color_mapping = color_mapping_##led_node_id,                             \
		 },                                                                                \
	 .pcfg = PINCTRL_DT_DEV_CONFIG_GET(led_node_id),                                           \
	 .active_low = DT_PROP(led_node_id, active_low),                                           \
	 .prescale = DT_PROP(led_node_id, prescale),                                               \
	 .total_cnt = RTL87X3G_SLEEP_LED_SRC_CLK_HZ / DT_PROP(led_node_id, prescale) /             \
		      DT_PROP(led_node_id, frequency_hz)},
#define RTL87X3G_LED_INIT(id)                                                                      \
	DT_INST_FOREACH_CHILD(id, COLOR_MAPPING)                                                   \
	DT_INST_FOREACH_CHILD(id, PINCTRL_DEFINE)                                                  \
	static const struct rtl87x3g_led_info rtl87x3g_leds_##id[] = {                             \
		DT_INST_FOREACH_CHILD(id, LED_INFO)};                                              \
	static const struct rtl87x3g_config rtl87x3g_config_##id = {                               \
		.num_leds = ARRAY_SIZE(rtl87x3g_leds_##id),                                        \
		.leds_info = rtl87x3g_leds_##id,                                                   \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(id, &rtl87x3g_led_init, NULL, NULL, &rtl87x3g_config_##id,           \
			      POST_KERNEL, CONFIG_LED_INIT_PRIORITY, &rtl87x3g_led_api);
DT_INST_FOREACH_STATUS_OKAY(RTL87X3G_LED_INIT)
