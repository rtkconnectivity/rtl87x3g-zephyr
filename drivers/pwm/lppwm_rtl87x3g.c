/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT realtek_rtl87x3g_lppwm

#include <errno.h>

#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/pinctrl.h>

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
#include <rtl876x_lppwm.h>
#endif
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(lppwm_rtl87x3g, CONFIG_PWM_LOG_LEVEL);

/** PWM data. */
struct lppwm_rtl87x3g_data {
	/** LPPWM clock (Hz). */
	uint32_t lppwm_clk;
};

/** PWM configuration. */
struct lppwm_rtl87x3g_config {
	uint32_t reg;
	uint32_t src_clk_freq;
	const struct pinctrl_dev_config *pcfg;
};

static int lppwm_rtl87x3g_set_cycles(const struct device *dev, uint32_t channel,
				      uint32_t period_cycles, uint32_t pulse_cycles,
				      pwm_flags_t flags)
{
	const struct lppwm_rtl87x3g_config *config = dev->config;
	LPPWM_TypeDef *lppwm = (LPPWM_TypeDef *)config->reg;
	LPPWM_InitTypeDef lppwm_init;

	LOG_DBG("%s, channel=%d, period_cycles=%x, pulse_cycles=%x, flags=%x\n",
		dev->name, channel, period_cycles, pulse_cycles, flags);

	/* LPPWM uses 16-bit counters for high/low period */
	if (period_cycles > UINT16_MAX || pulse_cycles > UINT16_MAX) {
		LOG_ERR("period/pulse cycles exceed 16-bit range");
		return -EINVAL;
	}

	if (pulse_cycles > period_cycles) {
		return -EINVAL;
	}

	LPPWM_Reset(lppwm);
	LPPWM_StructInit(&lppwm_init);
	lppwm_init.LPPWM_Polarity = (flags & PWM_POLARITY_INVERTED) ?
				    LPPWM_POLARITY_INVERT : LPPWM_POLARITY_NORMAL;
	lppwm_init.LPPWM_PeriodHigh = pulse_cycles;
	lppwm_init.LPPWM_PeriodLow = period_cycles - pulse_cycles;
	LPPWM_Init(lppwm, &lppwm_init);
	LPPWM_Cmd(lppwm, ENABLE);

	return 0;
}

static int lppwm_rtl87x3g_get_cycles_per_sec(const struct device *dev,
					      uint32_t channel, uint64_t *cycles)
{
	struct lppwm_rtl87x3g_data *data = dev->data;

	*cycles = data->lppwm_clk;

	LOG_DBG("channel=%d, cycles=%d\n", channel, (uint32_t)*cycles);
	return 0;
}

static const struct pwm_driver_api lppwm_rtl87x3g_driver_api = {
	.set_cycles = lppwm_rtl87x3g_set_cycles,
	.get_cycles_per_sec = lppwm_rtl87x3g_get_cycles_per_sec,
};

static int lppwm_rtl87x3g_init(const struct device *dev)
{
	const struct lppwm_rtl87x3g_config *config = dev->config;
	struct lppwm_rtl87x3g_data *data = dev->data;
	LPPWM_TypeDef *lppwm = (LPPWM_TypeDef *)config->reg;
	int ret;

	data->lppwm_clk = config->src_clk_freq;

	/* apply pin configuration */
	ret = pinctrl_apply_state(config->pcfg, PINCTRL_STATE_DEFAULT);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

#define LPPWM_RTL87X3G_INIT(index)						\
	static struct lppwm_rtl87x3g_data lppwm_rtl87x3g_data_##index;	\
										\
	PINCTRL_DT_INST_DEFINE(index);						\
										\
	static const struct lppwm_rtl87x3g_config				\
		lppwm_rtl87x3g_config_##index = {				\
		.reg = DT_INST_REG_ADDR(index),					\
		.src_clk_freq = DT_INST_PROP_OR(index, src_clk_freq, 32000),	\
		.pcfg = PINCTRL_DT_INST_DEV_CONFIG_GET(index),			\
	};									\
										\
	DEVICE_DT_INST_DEFINE(index, &lppwm_rtl87x3g_init,			\
			      NULL,						\
			      &lppwm_rtl87x3g_data_##index,			\
			      &lppwm_rtl87x3g_config_##index,			\
			      POST_KERNEL, CONFIG_PWM_INIT_PRIORITY,		\
			      &lppwm_rtl87x3g_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LPPWM_RTL87X3G_INIT)
