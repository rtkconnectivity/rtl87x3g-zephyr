/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT realtek_rtl87x3g_pwm

#include <errno.h>

#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/rtl87x3g_clock_control.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
#include <rtl876x_tim.h>
#include <rtl876x_rcc.h>
#endif

#ifdef CONFIG_PM_DEVICE
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
#include <rtl876x_pinmux.h>
#endif
#endif
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(pwm_rtl87x3g, CONFIG_PWM_LOG_LEVEL);

#ifdef CONFIG_PM_DEVICE
#if defined(CONFIG_SOC_SERIES_RTL87X3G)
typedef struct {
	uint32_t timer_reg[11];
} TIMERStoreReg_Typedef;
#endif
#endif

/** PWM data. */
struct pwm_rtl87x3g_data {
	/** Timer clock (Hz). */
	uint32_t tim_clk;
#ifdef CONFIG_PM_DEVICE
	TIMERStoreReg_Typedef store_buf;
	bool is_high_duty;
	/** True when duty is 0% or 100%, using GPIO pad output instead of TIM. */
	bool force_pad_mode;
	/** Pin number for GPIO output control during pad mode. */
	uint8_t pin;
#endif
};

/** PWM configuration. */
struct pwm_rtl87x3g_config {
	uint32_t reg;
	uint8_t channels;
	uint16_t prescaler;
	uint16_t clkid;
	const struct pinctrl_dev_config *pcfg;
};

/** Initialize TIM base (clock source, divider, mode, PWM enable).
 *  Shared between init and pad-mode-to-PWM-mode transition in set_cycles.
 */
static void pwm_rtl87x3g_tim_base_init(void *timer_base, uint16_t prescaler)
{
	uint8_t clock_div;

	switch (prescaler) {
	case 1:
		clock_div = TIM_CLOCK_DIVIDER_1;
		break;
	case 2:
		clock_div = TIM_CLOCK_DIVIDER_2;
		break;
	case 4:
		clock_div = TIM_CLOCK_DIVIDER_4;
		break;
	case 8:
		clock_div = TIM_CLOCK_DIVIDER_8;
		break;
	case 16:
		clock_div = TIM_CLOCK_DIVIDER_16;
		break;
	case 32:
		clock_div = TIM_CLOCK_DIVIDER_32;
		break;
	case 40:
		clock_div = TIM_CLOCK_DIVIDER_40;
		break;
	case 64:
		clock_div = TIM_CLOCK_DIVIDER_64;
		break;
	default:
		clock_div = TIM_CLOCK_DIVIDER_1;
		break;
	}

	TIM_TimeBaseInitTypeDef timer_init_struct;

	TIM_StructInit(&timer_init_struct);
	timer_init_struct.TIM_ClockSrc = CK_40M_TIMER;
	timer_init_struct.TIM_ClockDiv = clock_div;
	timer_init_struct.TIM_Mode = TIM_Mode_UserDefine;
	timer_init_struct.PWM_En = ENABLE;
	timer_init_struct.PWM_HighCount = 0;
	timer_init_struct.TIM_Period = UINT32_MAX;
	TIM_TimeBaseInit((TIM_TypeDef *)timer_base, &timer_init_struct);
}

static int pwm_rtl87x3g_set_cycles(const struct device *dev, uint32_t channel,
				   uint32_t period_cycles, uint32_t pulse_cycles, pwm_flags_t flags)
{
	const struct pwm_rtl87x3g_config *config = dev->config;

	LOG_DBG("%s, timer_base0x%x, channel=%d, period_cycles=%x, pulse_cycles=%x, flags=%x\n",
		dev->name, (uint32_t)config->reg, channel, period_cycles, pulse_cycles, flags);

#ifdef CONFIG_PM_DEVICE
	struct pwm_rtl87x3g_data *data = dev->data;
#endif
	void *timer_base = (void *)config->reg;

	if (channel > config->channels) {
		LOG_ERR("Invalid channel (%d)", channel);
		return -EINVAL;
	}

#ifdef CONFIG_PM_DEVICE
	{
		bool constant_duty = false;
		int pad_level = 0;

		/* Detect constant duty (0% or 100%) where output never toggles */
		if (flags & PWM_POLARITY_INVERTED) {
			if (period_cycles == 0U || pulse_cycles == 0U) {
				/* Inverted 0% = always high */
				constant_duty = true;
				pad_level = 1;
			} else if (period_cycles == pulse_cycles) {
				/* Inverted 100% = always low */
				constant_duty = true;
				pad_level = 0;
			}
		} else {
			if (period_cycles == 0U || pulse_cycles == 0U) {
				/* Normal 0% = always low */
				constant_duty = true;
				pad_level = 0;
			} else if (period_cycles == pulse_cycles) {
				/* Normal 100% = always high */
				constant_duty = true;
				pad_level = 1;
			}
		}

		if (constant_duty) {
			/* Switch to pad (GPIO) mode: disable TIM, set pad to fixed output */
			TIM_Cmd((TIM_TypeDef *)timer_base, DISABLE);
			Pad_Config(data->pin, PAD_SW_MODE, PAD_IS_PWRON, 0,
				   PAD_OUT_ENABLE, pad_level);
			data->force_pad_mode = true;
			data->is_high_duty = pad_level;
			return 0;
		}

		if (data->force_pad_mode) {
			/* Switching from pad mode to PWM mode: restore pinmux and init TIM */
			int ret;

			ret = pinctrl_apply_state(config->pcfg, PINCTRL_STATE_DEFAULT);

			if (ret < 0) {
				return ret;
			}
			/* Re-init TIM base */
			pwm_rtl87x3g_tim_base_init(timer_base, config->prescaler);
			data->force_pad_mode = false;
		}
	}
#endif

	if (flags & PWM_POLARITY_INVERTED) {
		if (period_cycles == 0U || pulse_cycles == 0U) {
			/* duty = 0 */
			TIM_PWMChangeFreqAndDuty((TIM_TypeDef *)timer_base, UINT32_MAX, UINT32_MAX);
		} else if (period_cycles == pulse_cycles) {
			/* duty = 100 */
			TIM_PWMChangeFreqAndDuty((TIM_TypeDef *)timer_base, UINT32_MAX, 0);
		} else {
			TIM_PWMChangeFreqAndDuty((TIM_TypeDef *)timer_base, period_cycles,
						 period_cycles - pulse_cycles);
		}
	} else {
		if (period_cycles == 0U || pulse_cycles == 0U) {
			/* duty = 0 */
			TIM_PWMChangeFreqAndDuty((TIM_TypeDef *)timer_base, UINT32_MAX, 0);
		} else if (period_cycles == pulse_cycles) {
			/* duty = 100 */
			TIM_PWMChangeFreqAndDuty((TIM_TypeDef *)timer_base, UINT32_MAX, UINT32_MAX);
		} else {
			TIM_PWMChangeFreqAndDuty((TIM_TypeDef *)timer_base, period_cycles,
						 pulse_cycles);
		}
	}
	TIM_Cmd((TIM_TypeDef *)timer_base, DISABLE);
	TIM_Cmd((TIM_TypeDef *)timer_base, ENABLE);

#ifdef CONFIG_PM_DEVICE
	data->is_high_duty = (pulse_cycles > (period_cycles >> 1));
	if ((flags & PWM_POLARITY_INVERTED)) {
		data->is_high_duty = !data->is_high_duty;
	}
#endif

	return 0;
}

static int pwm_rtl87x3g_get_cycles_per_sec(const struct device *dev, uint32_t channel,
					   uint64_t *cycles)
{
	struct pwm_rtl87x3g_data *data = dev->data;
	const struct pwm_rtl87x3g_config *config = dev->config;

	*cycles = (uint64_t)(data->tim_clk / config->prescaler);

	LOG_DBG("channel=%d, cycles=%d\n", channel, (uint32_t)*cycles);
	return 0;
}

#ifdef CONFIG_PM_DEVICE
static void TIMER_DLPSEnter(void *PeriReg, void *StoreBuf)
{
	TIMERStoreReg_Typedef *store_buf = (TIMERStoreReg_Typedef *)StoreBuf;
	TIM_TypeDef *TIMx = (TIM_TypeDef *)PeriReg;
	store_buf->timer_reg[0] = TIMx->TIMER_CUR_CNT;
	store_buf->timer_reg[1] = TIMx->TIMER_MODE_CFG;
	store_buf->timer_reg[2] = TIMx->TIMER_MAX_CNT;
	store_buf->timer_reg[3] = TIMx->TIMER_CCR;
	store_buf->timer_reg[4] = TIMx->TIMER_CCR_FIFO;
	store_buf->timer_reg[5] = TIMx->TIMER_PWM_CFG;
	store_buf->timer_reg[6] = TIMx->TIMER_PWM_SHIFT_CNT;
	store_buf->timer_reg[7] = TIMx->TIMER_DMA_CFG;
	store_buf->timer_reg[8] = TIMER1_SHARE->TIMER_EN_CTRL;
	store_buf->timer_reg[9] = TIMER1_SHARE->TIMER_INTR_EN_CTRL;
	store_buf->timer_reg[10] = TIMER1_SHARE->TIMER_PAUSE_INTR_EN_CTRL;
}

static void TIMER_DLPSExit(void *PeriReg, void *StoreBuf)
{
	TIMERStoreReg_Typedef *store_buf = (TIMERStoreReg_Typedef *)StoreBuf;
	TIM_TypeDef *TIMx = (TIM_TypeDef *)PeriReg;
	uint8_t id = ((uint32_t)TIMx - TIMER1_BASE) / 0X50;

	TIMx->TIMER_MODE_CFG = store_buf->timer_reg[1];
	TIMx->TIMER_MAX_CNT = store_buf->timer_reg[2];
	if ((void *)TIMx >= (void *)TIM1_CH4) {
		TIMx->TIMER_PWM_CFG = store_buf->timer_reg[5];
		TIMx->TIMER_CCR = store_buf->timer_reg[3];
	}
	if ((void *)TIMx >= (void *)TIM1_CH6) {
		TIMx->TIMER_CCR_FIFO = store_buf->timer_reg[4];
		TIMx->TIMER_PWM_SHIFT_CNT = store_buf->timer_reg[6];
		TIMx->TIMER_DMA_CFG = store_buf->timer_reg[7];
	}

	TIMER1_SHARE->TIMER_PAUSE_INTR_EN_CTRL |= (store_buf->timer_reg[10] & BIT(id));
	TIMER1_SHARE->TIMER_INTR_EN_CTRL |= (store_buf->timer_reg[9] & BIT(id));
	TIMER1_SHARE->TIMER_EN_CTRL |= (store_buf->timer_reg[8] & BIT(id));
}

static int pwm_rtl87x3g_pm_action(const struct device *dev, enum pm_device_action action)
{
	const struct pwm_rtl87x3g_config *config = dev->config;
	struct pwm_rtl87x3g_data *data = dev->data;
	void *timer_base = (void *)config->reg;
	int err;

	switch (action) {
	case PM_DEVICE_ACTION_SUSPEND:

		TIMER_DLPSEnter(timer_base, &data->store_buf);

		/* Move pins to sleep state */
		err = pinctrl_apply_state(config->pcfg, PINCTRL_STATE_SLEEP);
		if (err == -ENOENT) {
			/* sleep status pinctrl is not configured.
			 * In pad mode (duty 0%/100%): TIM is already disabled, pad is GPIO output
			 * with fixed level, so no further action needed.
			 * Otherwise, maintain the level manually.
			 */
			if (data->force_pad_mode) {
				return 0;
			}
			const struct pinctrl_state *state;
			err = pinctrl_lookup_state(config->pcfg, PINCTRL_STATE_DEFAULT, &state);
			if (err < 0) {
				return err;
			}
			Pad_Config(state->pins[0].pin, PAD_SW_MODE, PAD_IS_PWRON, 0, PAD_OUT_ENABLE,
				   data->is_high_duty);
		} else if (err < 0) {
			return err;
		}

		break;
	case PM_DEVICE_ACTION_RESUME:

		if (data->force_pad_mode) {
			/* In pad mode: pad is GPIO output with fixed level.
			 * Ensure clock is on for potential future mode switch,
			 * but skip pinctrl and TIM restore.
			 */
			(void)clock_control_on(RTL87X3G_CLOCK_CONTROLLER,
					       (clock_control_subsys_t)&config->clkid);
			return 0;
		}

		/* Set pins to active state */
		err = pinctrl_apply_state(config->pcfg, PINCTRL_STATE_DEFAULT);
		if (err < 0) {
			return err;
		}

		(void)clock_control_on(RTL87X3G_CLOCK_CONTROLLER,
				       (clock_control_subsys_t)&config->clkid);

		TIMER_DLPSExit(timer_base, &data->store_buf);

		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}
#endif /* CONFIG_PM_DEVICE */

static const struct pwm_driver_api pwm_rtl87x3g_driver_api = {
	.set_cycles = pwm_rtl87x3g_set_cycles,
	.get_cycles_per_sec = pwm_rtl87x3g_get_cycles_per_sec,
};

static int pwm_rtl87x3g_init(const struct device *dev)
{
	const struct pwm_rtl87x3g_config *config = dev->config;
	struct pwm_rtl87x3g_data *data = dev->data;
	void *timer_base = (void *)config->reg;
	int ret;

	data->tim_clk = 40000000;

#ifdef CONFIG_PM_DEVICE
	data->is_high_duty = false;
	data->force_pad_mode = false;
	{
		const struct pinctrl_state *state;

		if (pinctrl_lookup_state(config->pcfg, PINCTRL_STATE_DEFAULT,
					 &state) == 0) {
			data->pin = state->pins[0].pin;
		}
	}
#endif

	/* apply pin configuration */
	ret = pinctrl_apply_state(config->pcfg, PINCTRL_STATE_DEFAULT);
	if (ret < 0) {
		return ret;
	}

	(void)clock_control_on(RTL87X3G_CLOCK_CONTROLLER, (clock_control_subsys_t)&config->clkid);
	pwm_rtl87x3g_tim_base_init(timer_base, config->prescaler);

	return 0;
}

#define PWM_RTL87X3G_INIT(index)                                                                   \
	static struct pwm_rtl87x3g_data pwm_rtl87x3g_data_##index;                                 \
                                                                                                   \
	PINCTRL_DT_INST_DEFINE(index);                                                             \
                                                                                                   \
	static const struct pwm_rtl87x3g_config pwm_rtl87x3g_config_##index = {                    \
		.reg = DT_REG_ADDR(DT_INST_PARENT(index)),                                         \
		.clkid = DT_CLOCKS_CELL(DT_INST_PARENT(index), id),                                \
		.prescaler = DT_PROP(DT_INST_PARENT(index), prescaler),                            \
		.channels = DT_PROP(DT_INST_PARENT(index), channels),                              \
		.pcfg = PINCTRL_DT_INST_DEV_CONFIG_GET(index),                                     \
	};                                                                                         \
                                                                                                   \
	PM_DEVICE_DT_INST_DEFINE(index, pwm_rtl87x3g_pm_action);                                   \
	DEVICE_DT_INST_DEFINE(index, &pwm_rtl87x3g_init, PM_DEVICE_DT_INST_GET(index),             \
			      &pwm_rtl87x3g_data_##index, &pwm_rtl87x3g_config_##index,            \
			      POST_KERNEL, CONFIG_PWM_INIT_PRIORITY, &pwm_rtl87x3g_driver_api);

DT_INST_FOREACH_STATUS_OKAY(PWM_RTL87X3G_INIT)
