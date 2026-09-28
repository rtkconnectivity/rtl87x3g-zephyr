/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT realtek_rtl87x3g_adc

#include <errno.h>

#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/rtl87x3g_clock_control.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/reset.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>

#include <zephyr/irq.h>

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
#include "rtl876x_adc.h"
#include "adc_manager.h"
#endif

#define ADC_CONTEXT_USES_KERNEL_TIMER
#include "adc_context.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(adc_rtl87x3g, CONFIG_ADC_LOG_LEVEL);

struct adc_rtl87x3g_config {
	uint32_t reg;
	uint8_t channels;
	const struct pinctrl_dev_config *pcfg;
};

typedef struct {
	uint32_t adc_reg[15];
} ADCStoreReg_TypeDef;

struct adc_rtl87x3g_data {
	struct adc_context ctx;
	const struct device *dev;
	uint8_t is_bypass_mode;
	uint8_t adc_mgr_index;
	uint8_t seq_map;
	int32_t *buffer;
	int32_t *repeat_buffer;
#ifdef CONFIG_PM_DEVICE
	ADCStoreReg_TypeDef store_buf;
#endif
};

static void adc_mgr_callback(void *pvPara, uint32_t int_status)
{
	const struct device *dev = DEVICE_DT_GET(DT_DRV_INST(0));
	struct adc_rtl87x3g_data *data = dev->data;
	const struct adc_rtl87x3g_config *cfg = dev->config;

	uint16_t raw_data[16];
	int32_t voltage;
	uint8_t index = 0;

	adc_mgr_read_data_req(data->adc_mgr_index, raw_data, data->seq_map);

	for (uint32_t i = 0; i < cfg->channels; i++) {
		if (data->seq_map & BIT(i)) {
			if (data->is_bypass_mode & BIT(i)) {
				voltage =
					ADC_GetHighBypassRes(raw_data[index], EXT_SINGLE_ENDED(i));
			} else {
				voltage = ADC_GetRes(raw_data[index], EXT_SINGLE_ENDED(i));
			}
			*data->buffer++ = voltage;
			index++;
		}
	}

	adc_mgr_free_chann(data->adc_mgr_index);
	adc_context_on_sampling_done(&data->ctx, dev);
}

static int adc_rtl87x3g_start_read(const struct device *dev, const struct adc_sequence *sequence)
{

	struct adc_rtl87x3g_data *data = dev->data;
	const struct adc_rtl87x3g_config *cfg = dev->config;

	if (sequence->resolution != 12U) {
		LOG_ERR("resolution is not valid");
		return -ENOTSUP;
	}

	data->seq_map = sequence->channels;
	for (uint8_t i = 0; i < cfg->channels; i++) {
		if (sequence->channels & BIT(i)) {
			if (data->is_bypass_mode & BIT(i)) {
				ADC_HighBypassCmd(i, ENABLE);
			} else {
				ADC_HighBypassCmd(i, DISABLE);
			}
		}
	}

	data->buffer = sequence->buffer;
	adc_context_start_read(&data->ctx, sequence);

	return adc_context_wait_for_completion(&data->ctx);
}

static int adc_rtl87x3g_read(const struct device *dev, const struct adc_sequence *seq)
{
	struct adc_rtl87x3g_data *data = dev->data;
	int error;

	adc_context_lock(&data->ctx, false, NULL);
	error = adc_rtl87x3g_start_read(dev, seq);
	adc_context_release(&data->ctx, error);

	return error;
}

#ifdef CONFIG_ADC_ASYNC
static int adc_rtl87x3g_read_async(const struct device *dev, const struct adc_sequence *sequence,
				   struct k_poll_signal *async)
{
	struct adc_rtl87x3g_data *data = dev->data;
	int error;

	adc_context_lock(&data->ctx, true, async);
	error = adc_rtl87x3g_start_read(dev, sequence);
	adc_context_release(&data->ctx, error);

	return error;
}
#endif /* CONFIG_ADC_ASYNC */

static int adc_rtl87x3g_channel_setup(const struct device *dev,
				      const struct adc_channel_cfg *chan_cfg)
{
	const struct adc_rtl87x3g_config *cfg = dev->config;

	if (chan_cfg->gain != ADC_GAIN_1) {
		LOG_ERR("Gain is not valid");
		return -ENOTSUP;
	}

	if (chan_cfg->reference != ADC_REF_INTERNAL) {
		LOG_ERR("Reference is not valid");
		return -ENOTSUP;
	}

	if (chan_cfg->acquisition_time != ADC_ACQ_TIME_DEFAULT) {
		LOG_ERR("Unsupported acquisition_time '%d'", chan_cfg->acquisition_time);
		return -ENOTSUP;
	}

	if (chan_cfg->channel_id >= cfg->channels) {
		LOG_ERR("Invalid channel (%u)", chan_cfg->channel_id);
		return -EINVAL;
	}

	if (chan_cfg->differential) {
		LOG_ERR("Differential sampling not supported");
		return -ENOTSUP;
	}

	return 0;
}

static void adc_context_start_sampling(struct adc_context *ctx)
{
	struct adc_rtl87x3g_data *data = CONTAINER_OF(ctx, struct adc_rtl87x3g_data, ctx);
	const struct device *dev = data->dev;
	const struct adc_rtl87x3g_config *cfg = dev->config;

	data->repeat_buffer = data->buffer;

	ADC_InitTypeDef adc_init_struct;

	ADC_StructInit(&adc_init_struct);
	adc_init_struct.bitmap = data->seq_map;
	for (uint32_t i = 0; i < cfg->channels; i++) {
		adc_init_struct.schIndex[i] = EXT_SINGLE_ENDED(i);
	}
	if (!adc_mgr_register_req(&adc_init_struct, (adc_callback_function_t)adc_mgr_callback,
				  &(data->adc_mgr_index))) {
		LOG_ERR("adc mgr request fail");
		return;
	}
	adc_mgr_enable_req(data->adc_mgr_index);
}

static void adc_context_update_buffer_pointer(struct adc_context *ctx, bool repeat_sampling)
{
	struct adc_rtl87x3g_data *data = CONTAINER_OF(ctx, struct adc_rtl87x3g_data, ctx);

	if (repeat_sampling) {
		data->buffer = data->repeat_buffer;
	}
}

#ifdef CONFIG_PM_DEVICE
static int adc_rtl87x3g_pm_action(const struct device *dev, enum pm_device_action action)
{
	struct adc_rtl87x3g_data *data = dev->data;
	const struct adc_rtl87x3g_config *cfg = dev->config;
	ADC_TypeDef *adc = (ADC_TypeDef *)cfg->reg;
	int err;

	extern void ADC_DLPSEnter(void *peri_reg, void *p_store_buf);
	extern void ADC_DLPSExit(void *peri_reg, void *p_store_buf);

	switch (action) {
	case PM_DEVICE_ACTION_SUSPEND:

		ADC_DLPSEnter(adc, &data->store_buf);

		/* Move pins to sleep state */
		err = pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_SLEEP);
		if ((err < 0) && (err != -ENOENT)) {
			return err;
		}
		break;
	case PM_DEVICE_ACTION_RESUME:
		/* Set pins to active state */
		err = pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_DEFAULT);
		if (err < 0) {
			return err;
		}

		ADC_DLPSExit(adc, &data->store_buf);

		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}
#endif /* CONFIG_PM_DEVICE */

static const struct adc_driver_api adc_rtl87x3g_driver_api = {
	.channel_setup = adc_rtl87x3g_channel_setup,
	.read = adc_rtl87x3g_read,
#ifdef CONFIG_ADC_ASYNC
	.read_async = adc_rtl87x3g_read_async,
#endif /* CONFIG_ADC_ASYNC */
};

static int adc_rtl87x3g_init(const struct device *dev)
{
	struct adc_rtl87x3g_data *data = dev->data;
	const struct adc_rtl87x3g_config *cfg = dev->config;
	int ret;

	data->dev = dev;

	ret = pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_DEFAULT);
	if (ret < 0) {
		return ret;
	}
	adc_context_unlock_unconditionally(&data->ctx);

	return 0;
}

#define RTL87X3G_ADC_INIT(index)                                                                   \
	PINCTRL_DT_INST_DEFINE(index);                                                             \
	static struct adc_rtl87x3g_data adc_rtl87x3g_data_##index = {                              \
		ADC_CONTEXT_INIT_TIMER(adc_rtl87x3g_data_##index, ctx),                            \
		ADC_CONTEXT_INIT_LOCK(adc_rtl87x3g_data_##index, ctx),                             \
		ADC_CONTEXT_INIT_SYNC(adc_rtl87x3g_data_##index, ctx),                             \
		.is_bypass_mode = DT_INST_PROP_OR(index, is_bypass_mode, 0),                       \
	};                                                                                         \
	const static struct adc_rtl87x3g_config adc_rtl87x3g_config_##index = {                    \
		.reg = DT_INST_REG_ADDR(index),                                                    \
		.channels = DT_INST_PROP(index, channels),                                         \
		.pcfg = PINCTRL_DT_INST_DEV_CONFIG_GET(index),                                     \
	};                                                                                         \
	PM_DEVICE_DT_INST_DEFINE(index, adc_rtl87x3g_pm_action);                                   \
	DEVICE_DT_INST_DEFINE(index, &adc_rtl87x3g_init, PM_DEVICE_DT_INST_GET(index),             \
			      &adc_rtl87x3g_data_##index, &adc_rtl87x3g_config_##index,            \
			      POST_KERNEL, CONFIG_ADC_INIT_PRIORITY, &adc_rtl87x3g_driver_api);

DT_INST_FOREACH_STATUS_OKAY(RTL87X3G_ADC_INIT)
