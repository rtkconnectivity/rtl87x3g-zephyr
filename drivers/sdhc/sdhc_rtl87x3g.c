/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT realtek_rtl87x3g_sdhc

#include <zephyr/kernel.h>
#include <zephyr/drivers/sdhc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <soc.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/pinctrl/rtl87x3g_pinctrl.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/rtl87x3g_clock_control.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>
#include <math.h>

#include "rtl876x_sdio.h"
#include "clock.h"
#include "trace.h"

LOG_MODULE_REGISTER(sdhc, CONFIG_SDHC_LOG_LEVEL);

#define PINCTRL_STATE_INTERRUPT (PINCTRL_STATE_PRIV_START + 1)

BUILD_ASSERT(CONFIG_HEAP_MEM_POOL_SIZE > (2 * sizeof(SDIO_ADMA2TypeDef)));

#define SDIO_HOST_INIT_TIMEOUT ((uint32_t)0xFFFFFF)

#define SD_BUS_WIDTH_1B 0x00
#define SD_BUS_WIDTH_4B 0x02
#define SDH_RSP0        ((uint32_t)0x00000000)
#define SDH_RSP2        ((uint32_t)0x00000004)
#define SDH_RSP4        ((uint32_t)0x00000008)
#define SDH_RSP6        ((uint32_t)0x0000000C)
#define SDH_TRANS_INT                           SDIO_INT_TF_CMPL

#define SW_RST_FOR_DAT_LINE_MASK        BIT2
#define SW_RST_FOR_CMD_LINE_MASK        BIT1
#define SW_RST_FOR_ALL_MASK             BIT0

#define COMMAND_INHIBIT_DAT_MASK        BIT1
#define COMMAND_INHIBIT_CMD_MASK        BIT0

#define DAT3_0_LINE_SIGNAL_LEVEL_MASK   (BIT23 | BIT22 | BIT21 | BIT20)

extern void SDH_ClockCmd(SDIO_TypeDef *SDIOx, FunctionalState NewState);
extern void SDH_PlugAndErrorIntConfig(SDIO_TypeDef *SDIOx);
static int sdhc_rtl87x3g_reset(const struct device *dev);

static uint32_t div_values[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
static uint32_t div_params[] = {SDIO_CLOCK_DIV_1,  SDIO_CLOCK_DIV_2,   SDIO_CLOCK_DIV_4,
				SDIO_CLOCK_DIV_8,  SDIO_CLOCK_DIV_16,  SDIO_CLOCK_DIV_32,
				SDIO_CLOCK_DIV_64, SDIO_CLOCK_DIV_128, SDIO_CLOCK_DIV_256};
struct gpio_callback sdio_int_gpio_cb;

#define DEVICE_DT_GET_AND_COMMA(node_id) DEVICE_DT_GET(node_id),
static const struct device *const devices[] = {
	DT_FOREACH_STATUS_OKAY(DT_DRV_COMPAT, DEVICE_DT_GET_AND_COMMA)};

typedef enum {
	SD_RESP_NO,
	SD_RSP_LEN_136,
	SD_RSP_LEN_48,
	SD_RSP_LEN_48_CHK_BUSY
} T_SD_RSP_TYPE;

typedef struct {
	uint32_t cmd;
	uint32_t argument;
	bool with_data;
	bool cmd_idx_check;
	bool cmd_crc_check;
	uint32_t resp_len;
} T_SDH_CMD_CFG;

typedef struct {
	bool is_read_dir;
	bool cmd12_auto_enable;
	uint32_t buf;
	uint16_t blk_size;
	uint16_t blk_count;
} T_SDH_DATA_CFG;

#ifdef CONFIG_PM_DEVICE
typedef struct {
	uint32_t sdhc_reg[15];
} SDHCStoreReg_Typedef;
#endif

struct sdhc_rtl87x3g_config {
	const SDIO_TypeDef *sdio_base;
	const uint16_t clkid;
	const struct pinctrl_dev_config *pcfg;
	const struct gpio_dt_spec pwr_gpio;
	const struct gpio_dt_spec int_gpio;
	void (*sd_irq_connect)(void);
	void (*sd_irq_enable)(void);
	void (*sd_irq_disable)(void);
	uint8_t pin_group;
	struct sdhc_host_props props;
};

struct sdhc_rtl87x3g_data {
	uint8_t bus_width;
	uint32_t src_clock;
	uint32_t bus_clock;
	enum sdhc_power power_mode;
	enum sdhc_timing_mode timing;
	struct k_mutex s_request_mutex;
	sdhc_interrupt_cb_t cb;
	void *user_data;
	bool sdio_int_en;
	bool initialized;
	bool tuned;
	bool tuning;
	uint32_t bus_delay;
	uint16_t sdio_err_status;
	bool recovery_failed;
	uint8_t cur_cmd;
#ifdef CONFIG_PM_DEVICE
	SDHCStoreReg_Typedef store_buf;
#endif
};

typedef struct {
	SDIO_ADMA2TypeDef *SDIO_ADMA2_DescTab;
} T_SDH_PROP;

extern T_SDH_PROP sdh_prop[2];

static void SDH_SendCommand(SDIO_TypeDef *SDIOx, T_SDH_CMD_CFG *p_sdh_cmd)
{
	SDIO_CmdInitTypeDef SDIO_CmdInitStructure;

	SDIO_CmdInitStructure.SDIO_Argument = p_sdh_cmd->argument;
	SDIO_CmdInitStructure.SDIO_CmdIndex = p_sdh_cmd->cmd;
	SDIO_CmdInitStructure.SDIO_CmdType = NORMAL;
	SDIO_CmdInitStructure.SDIO_DataPresent = p_sdh_cmd->with_data;
	SDIO_CmdInitStructure.SDIO_CmdIdxCheck = p_sdh_cmd->cmd_idx_check;
	SDIO_CmdInitStructure.SDIO_CmdCrcCheck = p_sdh_cmd->cmd_crc_check;
	SDIO_CmdInitStructure.SDIO_ResponseType = p_sdh_cmd->resp_len;
	SDIO_SendCommand(SDIOx, &SDIO_CmdInitStructure);
}

static int SDH_WaitCmdLineIdle(SDIO_TypeDef *SDIOx, uint32_t timeout_ms)
{
	int64_t start_time = k_uptime_get();

	while (SDIO_GetFlagStatus(SDIOx, SDIO_FLAG_CMD_INHIBIT) == SET) {
		if ((k_uptime_get() - start_time) >= timeout_ms) {
			return -EIO;
		}
	}

	return 0;
}

static int SDH_WaitCmdDatComplete(const struct device *dev, uint32_t timeout_ms)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	struct sdhc_rtl87x3g_data *data = dev->data;
	SDIO_TypeDef *SDIOx = (SDIO_TypeDef *)cfg->sdio_base;
	int64_t start_time = k_uptime_get();

	/* Wait for DAT-inhibit to clear. A data CRC still completes, so only a
	 * genuine stall (inhibit never clears) hits the timeout and fails.
	 */
	while (SDIO_GetFlagStatus(SDIOx, SDIO_FLAG_CMD_DAT_INHIBIT) == SET) {
		if ((k_uptime_get() - start_time) >= timeout_ms) {
			LOG_ERR("WaitCmdDat: TIMEOUT cmd %u after %ums, no idle "
				"(present 0x%x)", data->cur_cmd, timeout_ms,
				(unsigned int)SDIOx->PRESENT_STATE);
			return -EIO;
		}
	}

	return 0;
}

static int SDH_CheckCmd(SDIO_TypeDef *SDIOx, uint32_t cmd)
{
	if ((SDIO_GetResponse(SDIOx, SDIO_RSP2) & 0xFF) != cmd) {
		return -EIO;
	}

	return 0;
}

#define SDHC_ADMA2_DESC_MAX             128  /* max 254 blocks * 512B / 1KB = 127, +1 for alignment */

static int SDH_DataConfig(SDIO_TypeDef *SDIOx, T_SDH_DATA_CFG *p_sdh_data)
{
	SDIOID_Typedef sdio_id = SDIO_ID_GET(SDIOx);
	uint32_t total_bytes = (uint32_t)p_sdh_data->blk_count * p_sdh_data->blk_size;
	uint32_t remaining = total_bytes;
	uint32_t cur_addr = p_sdh_data->buf;
	uint16_t desc_idx = 0;

	if (sdh_prop[sdio_id].SDIO_ADMA2_DescTab == NULL) {
		sdh_prop[sdio_id].SDIO_ADMA2_DescTab = malloc(SDHC_ADMA2_DESC_MAX  * sizeof(SDIO_ADMA2TypeDef));
		if (sdh_prop[sdio_id].SDIO_ADMA2_DescTab == NULL) {
			LOG_ERR("SDH_DataConfig malloc fail");
			return -EIO;
		}
	}

	while (remaining > 0)
	{
		if (desc_idx >= SDHC_ADMA2_DESC_MAX)
		{
			return -EIO;
		}

		/* bytes until the next 1KB boundary */
		uint32_t next_boundary = (cur_addr + 0x400u) & ~0x3FFu;
		uint32_t chunk = next_boundary - cur_addr;
		if (chunk > remaining)
		{
			chunk = remaining;
		}

		sdh_prop[sdio_id].SDIO_ADMA2_DescTab[desc_idx].SDIO_Address = cur_addr;
		sdh_prop[sdio_id].SDIO_ADMA2_DescTab[desc_idx].SDIO_Length = (uint16_t)chunk;
		sdh_prop[sdio_id].SDIO_ADMA2_DescTab[desc_idx].SDIO_Attribute.SDIO_Valid = 1;
		sdh_prop[sdio_id].SDIO_ADMA2_DescTab[desc_idx].SDIO_Attribute.SDIO_End   = 0;
		sdh_prop[sdio_id].SDIO_ADMA2_DescTab[desc_idx].SDIO_Attribute.SDIO_Int   = 0;
		sdh_prop[sdio_id].SDIO_ADMA2_DescTab[desc_idx].SDIO_Attribute.SDIO_Act1  = 0;
		sdh_prop[sdio_id].SDIO_ADMA2_DescTab[desc_idx].SDIO_Attribute.SDIO_Act2  = 1;

		cur_addr  += chunk;
		remaining -= chunk;
		desc_idx++;
	}

	sdh_prop[sdio_id].SDIO_ADMA2_DescTab[desc_idx - 1].SDIO_Attribute.SDIO_End = 1;

	SDIO_DataInitTypeDef SDIO_DataInitStruct;

	SDIO_DataStructInit(&SDIO_DataInitStruct);
	SDIO_DataInitStruct.SDIO_Address = (uint32_t)sdh_prop[sdio_id].SDIO_ADMA2_DescTab;
	SDIO_DataInitStruct.SDIO_BlockCount = p_sdh_data->blk_count;
	SDIO_DataInitStruct.SDIO_BlockSize = p_sdh_data->blk_size;
	SDIO_DataInitStruct.SDIO_TransferDir =
		(p_sdh_data->is_read_dir) ? SDIO_TransferDir_READ : SDIO_TransferDir_WRITE;
	SDIO_DataInitStruct.SDIO_DMAEn = ENABLE;
	SDIO_DataInitStruct.SDIO_AutoCMD12En =
		(p_sdh_data->cmd12_auto_enable) ? SDIO_AUTO_CMD12_EN : 0x00;

	__DSB();
	SDIO_DataConfig(SDIOx, &SDIO_DataInitStruct);
	return 0;
}

static int SDH_BusDelaySel(SDIO_TypeDef *SDIOx, uint32_t bus_wide, uint8_t bus_delay)
{
	if (bus_delay <= 0xF) {
		SDIO_BusDelaySel(SDIOx, bus_wide, bus_delay);
	} else {
		return -EIO;
	}

	return 0;
}

typedef union {
	uint32_t d32;
	struct {
		uint32_t AcessMode: 4;
		uint32_t CommandSystem: 4;
		uint32_t DriveStrength: 4;
		uint32_t PowerLimit: 4;
		uint32_t RsvdForGroup5: 4;
		uint32_t RsvdForGroup6: 4;
		uint32_t Rsvd: 7;
		uint32_t Mode: 1;
	} b;
} T_CMD6_ARG;

/* Recover after a transfer error: SW-reset the faulted CMD/DAT line, CMD12 to
 * abort, confirm idle. Error bits come from sdio_err_status (latched by ISR).
 */
static int sdhc_rtl87x3g_err_recovery(const struct device *dev)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	struct sdhc_rtl87x3g_data *data = dev->data;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	uint16_t err_status = data->sdio_err_status;
	uint32_t retry;
	int ret = 0;

	/* disable error interrupt signal */
	SDIO_ErrrorINTConfig(sdio_base, SDIO_INT_ALL_ERR, DISABLE);

	if (err_status == 0) {
		goto exit_recovery;
	}

	if (err_status & 0xf) { /* CMD-line error bits (0-3) */
		sdio_base->SW_RESET |= SW_RST_FOR_CMD_LINE_MASK;
		retry = 0xFFFF;
		while ((sdio_base->SW_RESET & SW_RST_FOR_CMD_LINE_MASK) && retry--) {
		}
		if (retry == 0) {
			ret = -EIO;
			LOG_ERR("SDHC err recovery: CMD line SW-reset timeout");
			goto exit_recovery;
		}
	}

	if (err_status & 0x70) { /* DAT-line error bits (4-6) */
		sdio_base->SW_RESET |= SW_RST_FOR_DAT_LINE_MASK;
		retry = 0xFFFF;
		while ((sdio_base->SW_RESET & SW_RST_FOR_DAT_LINE_MASK) && retry--) {
		}
		if (retry == 0) {
			ret = -EIO;
			LOG_ERR("SDHC err recovery: DAT line SW-reset timeout");
			goto exit_recovery;
		}
	}

	data->sdio_err_status = 0;

	SDIO_CmdInitTypeDef SDIO_CmdInitStructure;

	SDIO_CmdInitStructure.SDIO_Argument     = 0;
	SDIO_CmdInitStructure.SDIO_CmdIndex     = 12;  /* STOP_TRANSMISSION */
	SDIO_CmdInitStructure.SDIO_CmdType      = NORMAL;
	SDIO_CmdInitStructure.SDIO_DataPresent  = 0;
	SDIO_CmdInitStructure.SDIO_CmdIdxCheck  = ENABLE;
	SDIO_CmdInitStructure.SDIO_CmdCrcCheck  = ENABLE;
	SDIO_CmdInitStructure.SDIO_ResponseType = RSP_LEN_48;
	SDIO_SendCommand(sdio_base, &SDIO_CmdInitStructure);

	retry = 385000UL * 20;
	while ((sdio_base->PRESENT_STATE &
		(COMMAND_INHIBIT_DAT_MASK | COMMAND_INHIBIT_CMD_MASK)) && retry--) {
	}
	if (retry == 0) {
		ret = -EIO;
		LOG_ERR("SDHC err recovery: CMD12 inhibit-clear timeout");
		goto exit_recovery;
	}

	if (data->sdio_err_status & 0x1f) {
		ret = -EIO;
		LOG_ERR("SDHC err recovery: residual error 0x%x after CMD12",
			data->sdio_err_status);
		goto exit_recovery;
	}

	k_busy_wait(100); /* need more than 40us for the card to settle */

	if ((sdio_base->PRESENT_STATE & DAT3_0_LINE_SIGNAL_LEVEL_MASK) !=
	    DAT3_0_LINE_SIGNAL_LEVEL_MASK) {
		ret = -EIO;
		LOG_ERR("SDHC err recovery: DAT[3:0] not idle after CMD12");
		goto exit_recovery;
	}

exit_recovery:
	data->sdio_err_status = 0;

	/* enable error interrupt signal */
	SDIO_ErrrorINTConfig(sdio_base, SDIO_INT_ALL_ERR, ENABLE);
	return ret;
}

static void sdio_int_gpio_cb_func(const struct device *dev_in, struct gpio_callback *gpio_cb,
				  uint32_t pins)
{
	const struct sdhc_rtl87x3g_config *config;
	struct sdhc_rtl87x3g_data *data;

	for (int i = 0; i < ARRAY_SIZE(devices); i++) {
		data = (struct sdhc_rtl87x3g_data *)(devices[i]->data);
		config = (struct sdhc_rtl87x3g_config *)(devices[i]->config);
		if (dev_in == config->int_gpio.port && pins & BIT(config->int_gpio.pin)) {
			if (data->cb) {
				data->cb(devices[i], SDHC_INT_SDIO, data->user_data);
			}
		}
	}
}

static int sdhc_rtl87x3g_enable_interrupt_pin(const struct device *dev)
{
	const struct sdhc_rtl87x3g_config *config = dev->config;
	int ret;

	ret = gpio_pin_configure(config->int_gpio.port, config->int_gpio.pin,
				 (config->int_gpio.dt_flags | GPIO_INPUT | GPIO_PULL_UP));
	if (ret < 0) {
		return ret;
	}

	return gpio_pin_interrupt_configure(config->int_gpio.port, config->int_gpio.pin,
					    GPIO_INT_LEVEL_LOW);
}

static int sdhc_rtl87x3g_disable_interrupt_pin(const struct device *dev)
{
	LOG_DBG("[%s]", __func__);
	const struct sdhc_rtl87x3g_config *config = dev->config;
	int ret;

	ret = gpio_pin_configure(config->int_gpio.port, config->int_gpio.pin, GPIO_DISCONNECTED);
	if (ret < 0) {
		return ret;
	}

	ret = gpio_pin_interrupt_configure(config->int_gpio.port, config->int_gpio.pin,
					   GPIO_INT_DISABLE);
	if (ret < 0) {
		return ret;
	}

	return pinctrl_apply_state(config->pcfg, PINCTRL_STATE_INTERRUPT);
}

static int sdhc_rtl87x3g_send_wait_cmd(const struct device *dev, T_SDH_CMD_CFG *sdh_cmd, uint32_t timeout_ms)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;

	SDH_SendCommand(sdio_base, sdh_cmd);
	if (SDH_WaitCmdLineIdle(sdio_base, timeout_ms)) {
		return -ETIMEDOUT;
	}

	return 0;
}

static int sdhc_rtl87x3g_send_wait_cmd_data(const struct device *dev, T_SDH_CMD_CFG *sdh_cmd, uint32_t timeout_ms)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;

	SDH_SendCommand(sdio_base, sdh_cmd);
	if (SDH_WaitCmdDatComplete(dev, timeout_ms)) {
		return -ETIMEDOUT;
	}

	return 0;
}

static int sdhc_rtl87x3g_send_wait_check_cmd(const struct device *dev, T_SDH_CMD_CFG *sdh_cmd, uint32_t timeout_ms)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	int ret = 0;

	ret = sdhc_rtl87x3g_send_wait_cmd(dev, sdh_cmd, timeout_ms);
	if (ret) {
		return ret;
	}

	ret = SDH_CheckCmd(sdio_base, sdh_cmd->cmd);
	if (ret) {
		return -EIO;
	}

	return 0;
}

/* Switch the SDIO source clock to PLL1 100MHz. */
static void sdhc_rtl87x3g_set_src_pll1_100m(SDIO_TypeDef *sdio_base,
					    struct sdhc_rtl87x3g_data *data)
{
	CLKRequestType clk_type = (sdio_base == SDIO0) ? CLOCK_SDIO0 : CLOCK_SDIO1;

	clk_register_pre_hook(clk_type, 200);
	RCC_SDIOClockConfig(sdio_base, SDIO_CLOCK_SOURCE_PLL1, SDIO_CLK_DIV_2);
	clk_register_post_hook(clk_type, 200, &data->src_clock);
	data->src_clock = data->src_clock / 2 * 1000000;
}

/* Switch the SDIO source clock to the fixed 40MHz source. */
static void sdhc_rtl87x3g_set_src_40m(SDIO_TypeDef *sdio_base, struct sdhc_rtl87x3g_data *data,
				      uint16_t post_mhz)
{
	CLKRequestType clk_type = (sdio_base == SDIO0) ? CLOCK_SDIO0 : CLOCK_SDIO1;

	clk_register_pre_hook(clk_type, 40);
	RCC_SDIOClockConfig(sdio_base, SDIO_CLOCK_SOURCE_40MHZ, SDIO_CLK_DIV_1);
	clk_register_post_hook(clk_type, post_mhz, &data->src_clock);
	data->src_clock = data->src_clock * 1000000;
}

/* Re-derive the bus clock divider from the current source clock so the bus
 * clock stays as close as possible to its previous value (bus = src / (2 * div)).
 */
static void sdhc_rtl87x3g_reselect_bus_clock(SDIO_TypeDef *sdio_base,
					     struct sdhc_rtl87x3g_data *data)
{
	uint32_t prev_bus_clock = data->bus_clock;
	uint32_t best_diff = UINT32_MAX;
	int div_idx = ARRAY_SIZE(div_values) - 1;

	for (int i = 0; prev_bus_clock != 0 && i < (int)ARRAY_SIZE(div_values); i++) {
		uint32_t cand = data->src_clock / (div_values[i] << 1);
		uint32_t diff = (cand > prev_bus_clock) ? cand - prev_bus_clock
							: prev_bus_clock - cand;

		if (diff < best_diff) {
			best_diff = diff;
			div_idx = i;
		}
	}

	SDIO_SetClock(sdio_base, div_params[div_idx]);
	data->bus_clock = (uint32_t)(data->src_clock / (div_values[div_idx] << 1));
}

static int sdhc_rtl87x3g_do_transaction(const struct device *dev, struct sdhc_command *cmd,
					struct sdhc_data *data)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	struct sdhc_rtl87x3g_data *dev_data = dev->data;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	int ret = 0;
	bool blk_io;
	T_SDH_DATA_CFG sdh_data;
	T_SDH_CMD_CFG sdh_cmd;

	if (dev_data->sdio_int_en && dev_data->bus_width == 4) {
		sdhc_rtl87x3g_disable_interrupt_pin(dev);
	}

	/* Clear any error latched by a previous transaction. */
	dev_data->sdio_err_status = 0;
	dev_data->recovery_failed = false;

	if (!(cmd->opcode == MMC_SEND_OP_COND && dev_data->src_clock != 100000000)) {
		SDH_ClockCmd(sdio_base, ENABLE);
		if (SDH_WaitCmdLineIdle(sdio_base, cmd->timeout_ms)) {
			ret = -ETIMEDOUT;
			goto recover;
		}
	}

	sdh_cmd.cmd = cmd->opcode;
	sdh_cmd.argument = cmd->arg;
	sdh_cmd.with_data = data != NULL;

	/* Record the in-flight command for logging. */
	dev_data->cur_cmd = cmd->opcode;

	switch (cmd->opcode) {
	case SD_GO_IDLE_STATE:
		sdh_cmd.cmd_idx_check = DISABLE;
		sdh_cmd.cmd_crc_check = DISABLE;
		sdh_cmd.resp_len = SD_RESP_NO;

		if (sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms)) {
			ret = -ETIMEDOUT;
		}

		break;
	case SD_SEND_IF_COND:
		if (sdh_cmd.argument != 0) {
			sdh_cmd.cmd_idx_check = ENABLE;
			sdh_cmd.cmd_crc_check = ENABLE;
			sdh_cmd.resp_len = SD_RSP_LEN_48;

			ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
			if (ret) {
				break;
			}

			cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
			break;
		} else {
			sdh_data.is_read_dir = true;
			sdh_data.buf = (uint32_t)data->data;
			sdh_data.blk_size = data->block_size;
			sdh_data.blk_count = data->blocks;
			sdh_data.cmd12_auto_enable = true;

			sdh_cmd.cmd_idx_check = ENABLE;
			sdh_cmd.cmd_crc_check = ENABLE;
			sdh_cmd.resp_len = SD_RSP_LEN_48;

			if (SDH_DataConfig(sdio_base, &sdh_data)) {
				ret = -EIO;
				break;
			}

			if (sdhc_rtl87x3g_send_wait_cmd_data(dev, &sdh_cmd, cmd->timeout_ms + data->timeout_ms)) {
				ret = -ETIMEDOUT;
			}

			break;
		}
	case MMC_SEND_OP_COND:
		sdh_cmd.cmd_idx_check = DISABLE;
		sdh_cmd.cmd_crc_check = DISABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		/* Set src clk 100MHz */
		if (dev_data->src_clock != 100000000) {
			SDH_BusDelaySel(sdio_base, SDIO_BusWide_1b, 0xf);
			SDH_ClockCmd(sdio_base, DISABLE);

			sdhc_rtl87x3g_set_src_pll1_100m(sdio_base, dev_data);
			sdhc_rtl87x3g_reselect_bus_clock(sdio_base, dev_data);

			SDH_ClockCmd(sdio_base, ENABLE);

			if (SDH_WaitCmdLineIdle(sdio_base, cmd->timeout_ms)) {
				ret = -ETIMEDOUT;
				break;
			}
		}

		ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		break;
	case SD_VOL_SWITCH:
		sdh_cmd.cmd_idx_check = DISABLE;
		sdh_cmd.cmd_crc_check = DISABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;
		ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}
		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		break;
	case SD_APP_CMD:
		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		ret = sdhc_rtl87x3g_send_wait_check_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		break;
	case SD_SEND_STATUS:
		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		ret = sdhc_rtl87x3g_send_wait_check_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		break;
	case SD_SET_BLOCK_SIZE:
		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		ret = sdhc_rtl87x3g_send_wait_check_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		break;
	case SD_SEND_RELATIVE_ADDR:
		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		ret = sdhc_rtl87x3g_send_wait_check_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			goto cmd3_exit;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
cmd3_exit:
		/* Set src clk 40MHz */
		if (dev_data->src_clock != 40000000) {
			SDH_BusDelaySel(sdio_base, SDIO_BusWide_1b, 2);
			SDH_ClockCmd(sdio_base, DISABLE);

			sdhc_rtl87x3g_set_src_40m(sdio_base, dev_data, 40);
			sdhc_rtl87x3g_reselect_bus_clock(sdio_base, dev_data);

			SDH_ClockCmd(sdio_base, ENABLE);
		}

		break;
	case SD_APP_SEND_OP_COND:
		sdh_cmd.cmd = SD_APP_SEND_OP_COND;
		sdh_cmd.cmd_idx_check = DISABLE;
		sdh_cmd.cmd_crc_check = DISABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		break;
	case SDIO_SEND_OP_COND:
		sdh_cmd.cmd_idx_check = DISABLE;
		sdh_cmd.cmd_crc_check = DISABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		LOG_DBG("CMD5 rep 0x%x", cmd->response[0]);
		break;
	case SD_SELECT_CARD:
		if (sdh_cmd.argument != 0) {
			sdh_cmd.cmd_idx_check = ENABLE;
			sdh_cmd.cmd_crc_check = ENABLE;
			sdh_cmd.resp_len = SD_RSP_LEN_48_CHK_BUSY;

			ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
			if (ret) {
				break;
			}

			cmd->response[0] = SDIO_GetResponse(sdio_base, SDIO_RSP0);
		} else {
			sdh_cmd.cmd_idx_check = DISABLE;
			sdh_cmd.cmd_crc_check = DISABLE;
			sdh_cmd.resp_len = SD_RESP_NO;

			ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
			if (ret) {
				break;
			}
		}

		break;
	case SD_ALL_SEND_CID:
		sdh_cmd.cmd_idx_check = DISABLE;
		sdh_cmd.cmd_crc_check = DISABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_136;

		ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		cmd->response[1] = SDIO_GetResponse(sdio_base, SDH_RSP2);
		cmd->response[2] = SDIO_GetResponse(sdio_base, SDH_RSP4);
		cmd->response[3] = SDIO_GetResponse(sdio_base, SDH_RSP6);
		break;
	case SD_SEND_CSD:
		sdh_cmd.cmd_idx_check = DISABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_136;

		ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		cmd->response[1] = SDIO_GetResponse(sdio_base, SDH_RSP2);
		cmd->response[2] = SDIO_GetResponse(sdio_base, SDH_RSP4);
		cmd->response[3] = SDIO_GetResponse(sdio_base, SDH_RSP6);

		int i = sizeof(cmd->response) / sizeof(cmd->response[0]) - 1;
		while (i) {
			cmd->response[i] = cmd->response[i] << 8;
			cmd->response[i] |= cmd->response[i - 1] >> 24;
			i--;
		}

		cmd->response[0] = (cmd->response[0] << 8) & 0xFFFFFF00;
		break;
	case SD_SWITCH:
		if (sdh_cmd.with_data) {
			sdh_data.is_read_dir = true;
			sdh_data.buf = (uint32_t)data->data;
			sdh_data.blk_size = data->block_size;
			sdh_data.blk_count = data->blocks;
			sdh_data.cmd12_auto_enable = true;

			sdh_cmd.cmd_idx_check = ENABLE;
			sdh_cmd.cmd_crc_check = ENABLE;
			sdh_cmd.resp_len = SD_RSP_LEN_48;

			if (SDH_DataConfig(sdio_base, &sdh_data)) {
				ret = -EIO;
				break;
			}

			if (sdhc_rtl87x3g_send_wait_cmd_data(dev, &sdh_cmd, cmd->timeout_ms + data->timeout_ms)) {
				ret = -ETIMEDOUT;
			}

			break;
		} else {
			if (sdh_cmd.with_data != true) {
				sdh_cmd.cmd_idx_check = ENABLE;
				sdh_cmd.cmd_crc_check = ENABLE;
				sdh_cmd.resp_len = SD_RSP_LEN_48;

				ret = sdhc_rtl87x3g_send_wait_check_cmd(dev, &sdh_cmd, cmd->timeout_ms);
				if (ret) {
					break;
				}

				uint32_t err_int_status = SDIO_GetAllErrINTStatus(sdio_base);
				if (err_int_status) {
					ret = -EIO;
					SDIO_ClearErrorINTPendingBit(sdio_base, SDIO_INT_ALL_ERR);
					break;
				}

				break;
			}
		}

	case SDIO_RW_DIRECT:
		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		ret = sdhc_rtl87x3g_send_wait_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		LOG_DBG("CMD52 rep 0x%x", cmd->response[0]);
		break;
	case SDIO_RW_EXTENDED:
		sdh_data.is_read_dir = (cmd->arg & BIT(SDIO_CMD_ARG_RW_SHIFT)) ? false : true;
		sdh_data.buf = (uint32_t)data->data;
		sdh_data.blk_size = data->block_size;
		sdh_data.blk_count = data->blocks;
		sdh_data.cmd12_auto_enable = false;

		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		if (SDH_DataConfig(sdio_base, &sdh_data)) {
			ret = -EIO;
			break;
		}

		if (sdhc_rtl87x3g_send_wait_cmd_data(dev, &sdh_cmd, cmd->timeout_ms + data->timeout_ms)) {
			ret = -ETIMEDOUT;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		break;
	case SD_APP_SEND_SCR:
	case SD_APP_SEND_NUM_WRITTEN_BLK:
		sdh_data.is_read_dir = true;
		sdh_data.buf = (uint32_t)data->data;
		sdh_data.blk_size = data->block_size;
		sdh_data.blk_count = data->blocks;
		sdh_data.cmd12_auto_enable = true;

		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		if (SDH_DataConfig(sdio_base, &sdh_data)) {
			ret = -EIO;
			break;
		}

		if (sdhc_rtl87x3g_send_wait_cmd_data(dev, &sdh_cmd, cmd->timeout_ms + data->timeout_ms)) {
			ret = -ETIMEDOUT;
		}

		SCB_InvalidateDCache_by_Addr(data->data, data->blocks * data->block_size);

		break;
	case SD_READ_SINGLE_BLOCK:
	case SD_READ_MULTIPLE_BLOCK:
		sdh_data.is_read_dir = true;
		sdh_data.buf = (uint32_t)data->data;
		sdh_data.blk_size = data->block_size;
		sdh_data.blk_count = data->blocks;
		sdh_data.cmd12_auto_enable = true;

		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		if (SDH_DataConfig(sdio_base, &sdh_data)) {
			ret = -EIO;
			break;
		}

		if (sdhc_rtl87x3g_send_wait_cmd_data(dev, &sdh_cmd, cmd->timeout_ms + data->timeout_ms)) {
			ret = -ETIMEDOUT;
		}

		SCB_InvalidateDCache_by_Addr(data->data, data->blocks * data->block_size);

		break;
	case SD_WRITE_SINGLE_BLOCK:
	case SD_WRITE_MULTIPLE_BLOCK:
		sdh_data.is_read_dir = false;
		sdh_data.buf = (uint32_t)data->data;
		sdh_data.blk_size = data->block_size;
		sdh_data.blk_count = data->blocks;
		sdh_data.cmd12_auto_enable = true;

		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		if (SDH_DataConfig(sdio_base, &sdh_data)) {
			ret = -EIO;
			break;
		}

		if (sdhc_rtl87x3g_send_wait_cmd_data(dev, &sdh_cmd, cmd->timeout_ms + data->timeout_ms)) {
			ret = -ETIMEDOUT;
		}

		if (SDIO_GetResponse(sdio_base, SDH_RSP0) & BIT26) {
			ret = -EIO;
		}
		break;
	case SD_ERASE_BLOCK_START: /* CMD32: SD erase range start */
	case SD_ERASE_BLOCK_END:   /* CMD33: SD erase range end */
	case 35: /* MMC ERASE_GROUP_START */
	case 36: /* MMC ERASE_GROUP_END */
		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		ret = sdhc_rtl87x3g_send_wait_check_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		break;
	case SD_ERASE_BLOCK_OPERATION: /* CMD38 ERASE: R1 only, don't HW-wait busy;
					* mmc_erase polls CMD13 so the long erase does
					* not tie up the CPU in a tight busy spin.
					*/
		sdh_cmd.cmd_idx_check = ENABLE;
		sdh_cmd.cmd_crc_check = ENABLE;
		sdh_cmd.resp_len = SD_RSP_LEN_48;

		ret = sdhc_rtl87x3g_send_wait_check_cmd(dev, &sdh_cmd, cmd->timeout_ms);
		if (ret) {
			break;
		}

		cmd->response[0] = SDIO_GetResponse(sdio_base, SDH_RSP0);
		break;
	default:
		ret = -ENOTSUP;
	}

recover:
	/* Recover on stall/timeout, or a data error on block R/W; tolerate a CRC
	 * on init reads (EXT_CSD/SCR) so a marginal card still gets recognized.
	 */
	blk_io = dev_data->cur_cmd == SD_READ_SINGLE_BLOCK ||
		dev_data->cur_cmd == SD_READ_MULTIPLE_BLOCK ||
		dev_data->cur_cmd == SD_WRITE_SINGLE_BLOCK ||
		dev_data->cur_cmd == SD_WRITE_MULTIPLE_BLOCK;

	if (dev_data->sdio_err_status != 0) {
		ret = -EIO;
	}

	if (!dev_data->tuning && data != NULL &&
	    ((ret != 0 && ret != -ENOTSUP) || (blk_io && dev_data->sdio_err_status != 0))) {
		if (ret == 0) {
			ret = -EIO;
		}
		/* Non-recoverable: stop retrying this command. */
		if (sdhc_rtl87x3g_err_recovery(dev) != 0) {
			dev_data->recovery_failed = true;
		}
	}

	SDH_ClockCmd(sdio_base, DISABLE);

	if (dev_data->sdio_int_en && dev_data->bus_width == 4) {
		sdhc_rtl87x3g_enable_interrupt_pin(dev);
	}

	return ret;
}

static int sdhc_rtl87x3g_set_clk_src(const struct device *dev, uint32_t clk)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	struct sdhc_rtl87x3g_data *data = dev->data;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;

	if (clk == 0) {
		/* Set pll1 clk 0MHz */
		sdhc_rtl87x3g_set_src_40m(sdio_base, data, 0);
	} else if (clk > 20000000) {
		/* Set pll1 clk 100MHz */
		sdhc_rtl87x3g_set_src_pll1_100m(sdio_base, data);
	} else {
		/* Set src clk 40MHz */
		sdhc_rtl87x3g_set_src_40m(sdio_base, data, 40);
	}

	LOG_DBG("SDHC %s src clk freq is %d", dev->name, data->src_clock);
	return 0;
}

static int sdhc_rtl87x3g_driver_init(const struct device *dev)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	struct sdhc_rtl87x3g_data *data = dev->data;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	int ret;
	uint32_t time_out = SDIO_HOST_INIT_TIMEOUT;

	if (!data->initialized) {
		ret = clock_control_on(RTL87X3G_CLOCK_CONTROLLER, (clock_control_subsys_t)&cfg->clkid);

		if (ret != 0) {
			LOG_ERR("Error enabling SDHC clock");
			return ret;
		}

		SDIO_PinGroupConfig(sdio_base, cfg->pin_group);
		SDIO_DataPinConfig(data->bus_width == 1 ? SDIO_BusWide_1b : SDIO_BusWide_4b, ENABLE);
		SDIO_PinOutputCmd(ENABLE);

		if (sdhc_rtl87x3g_reset(dev)) {
			LOG_ERR("Fail to reset SDHC");
			return -EFAULT;
		}

		/* Enable internal clcok */
		SDIO_InternalClockCmd(sdio_base, ENABLE);
		time_out = SDIO_HOST_INIT_TIMEOUT;
		while (SDIO_GetInternalClockStatus(sdio_base) == RESET) {
			time_out--;
			if (time_out == 0) {
				LOG_ERR("Fail to enable SDHC internal clock");
				return -EFAULT;
			}
		}

		/* Initialize the SDIO host peripheral */
		SDIO_InitTypeDef SDIO_InitStructure;
		SDIO_StructInit(&SDIO_InitStructure);
		SDIO_InitStructure.SDIO_TimeOut = 0x0E;
		SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
		SDIO_InitStructure.SDIO_ClockDiv = SDIO_CLOCK_DIV_128;
		SDIO_Init(sdio_base, &SDIO_InitStructure);

		/* Enable SD bus power */
		SDIO_SetBusPower(sdio_base, SDIO_PowerState_ON);

		cfg->sd_irq_connect();

		SDH_PlugAndErrorIntConfig(sdio_base);

		SDH_ClockCmd(sdio_base, DISABLE);

		SDH_BusDelaySel(sdio_base, data->bus_width == 1 ? SDIO_BusWide_1b : SDIO_BusWide_4b, data->bus_delay);

		data->initialized = true;
	}
	return 0;
}

static int sdhc_rtl87x3g_driver_deinit(const struct device *dev)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	struct sdhc_rtl87x3g_data *data = dev->data;

	clock_control_off(RTL87X3G_CLOCK_CONTROLLER, (clock_control_subsys_t)&cfg->clkid);
	data->initialized = false;
	return 0;
}

/*
 * Set SDHC io properties
 */
static int sdhc_rtl87x3g_set_io(const struct device *dev, struct sdhc_io *ios)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	struct sdhc_rtl87x3g_data *data = dev->data;
	uint8_t bus_width;
	uint16_t clk_div;
	int ret;

	LOG_DBG("SDHC I/O: dev: %s, bus width %d, clock %dHz, card power %s, voltage %s", dev->name,
		ios->bus_width, ios->clock, ios->power_mode == SDHC_POWER_ON ? "ON" : "OFF",
		ios->signal_voltage == SD_VOL_1_8_V ? "1.8V" : "3.3V");

	sdhc_rtl87x3g_set_clk_src(dev, ios->clock);

	/* Toggle card power supply */
	if ((data->power_mode != ios->power_mode)) {
		if (ios->power_mode == SDHC_POWER_OFF) {
			if (cfg->pwr_gpio.port) {
				gpio_pin_set_dt(&cfg->pwr_gpio, 0);
			}

			pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_BUS_OFF);
		} else if (ios->power_mode == SDHC_POWER_ON) {
			if (cfg->pwr_gpio.port) {
				gpio_pin_set_dt(&cfg->pwr_gpio, 1);
			}

			pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_DEFAULT);
			pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_INTERRUPT);
		}

		data->power_mode = ios->power_mode;
	}


	if (ios->clock) {
		ret = sdhc_rtl87x3g_driver_init(dev);
		if (ret) {
			LOG_ERR("SDHC host init fail");
			return ret;
		}

		/* Check for frequency boundaries supported by host */
		if (ios->clock > cfg->props.f_max || ios->clock < cfg->props.f_min) {
			LOG_ERR("SDHC host supports clock between %dHz to %dHz", cfg->props.f_min,
				cfg->props.f_max);
		}

		if (data->bus_clock != (uint32_t)ios->clock) {
			static int8_t clk_values_last =
				sizeof(div_values) / sizeof(div_values[0]) - 1;

			/* Try setting new clock */
			/* output clk is half of divided clk, so double the desired output clk */
			clk_div = data->src_clock / (ios->clock << 1);

			int8_t i;

			if (clk_div >= div_values[clk_values_last]) {
				SDIO_SetClock(sdio_base, div_params[clk_values_last]);

				data->bus_clock =
					(uint32_t)(data->src_clock / (div_values[clk_values_last]));

				i = clk_values_last - 1;
			} else {
				for (i = clk_values_last; i >= 0; i--) {
					if (div_values[i] < clk_div) {
						break;
					}
				}

				data->bus_clock =
					((uint32_t)((data->src_clock) / (div_values[i + 1]))) >> 1;

				if (data->bus_clock > ios->clock) {
					i++;
					data->bus_clock = data->bus_clock >> 1;
				}

				SDIO_SetClock(sdio_base, div_params[i + 1]);

				data->bus_clock =
					((uint32_t)((data->src_clock) / (div_values[i + 1]))) >> 1;
			}

			LOG_DBG("Bus clock set to %d Hz, div_values=%d, div_params=0x%x",
				data->bus_clock, div_values[i + 1], div_params[i + 1]);
		}

		if (ios->bus_width) {
			/* Set bus width */
			switch (ios->bus_width) {
			case SDHC_BUS_WIDTH1BIT:
				bus_width = 1;
				break;
			case SDHC_BUS_WIDTH4BIT:
				bus_width = 4;
				break;
			default:
				return -ENOTSUP;
			}

			if (data->bus_width != bus_width) {
				SDIO_SetBusWide(sdio_base,
						bus_width == 1 ? SDIO_BusWide_1b : SDIO_BusWide_4b);

				LOG_DBG("Bus width set to %d bit", bus_width);

				data->bus_width = bus_width;
			}
		}
	} else {
		sdhc_rtl87x3g_driver_deinit(dev);
		data->bus_clock = (uint32_t)ios->clock;
	}

	if (ios->timing) {
		/* Set I/O timing */
		if (data->timing != ios->timing) {
			switch (ios->timing) {
			case SDHC_TIMING_LEGACY:
			case SDHC_TIMING_HS:
				break;
			case SDHC_TIMING_SDR12:
			case SDHC_TIMING_SDR25:
			case SDHC_TIMING_DDR50:
			case SDHC_TIMING_DDR52:
			case SDHC_TIMING_SDR50:
			case SDHC_TIMING_HS400:
			case SDHC_TIMING_SDR104:
			case SDHC_TIMING_HS200:
			default:
				LOG_ERR("Timing mode not supported for this device");
				return -ENOTSUP;
				break;
			}

			LOG_DBG("Bus timing successfully changed to %d", ios->timing);
			data->timing = ios->timing;
		}
	}

	return 0;
}

/*
 * Send CMD or CMD/DATA via SDHC
 */
static int sdhc_rtl87x3g_request(const struct device *dev, struct sdhc_command *cmd,
				 struct sdhc_data *data)
{
	LOG_DBG("[%s] opcode=%d arg=0x%x data=0x%x", __func__, cmd->opcode, cmd->arg,
		(uint32_t)data);
	struct sdhc_rtl87x3g_data *dev_data = (struct sdhc_rtl87x3g_data *)dev->data;
	int retries = (int)(cmd->retries + 1);
	int ret = 0;

	if (k_mutex_lock(&dev_data->s_request_mutex, K_MSEC(cmd->timeout_ms)) != 0) {
		return -ETIMEDOUT;
	}

	do {
		ret = sdhc_rtl87x3g_do_transaction(dev, cmd, data);
		if (!ret) {
			break;
		}
		/* Recovery deemed the controller non-recoverable; stop retrying. */
		if (dev_data->recovery_failed) {
			break;
		}
	} while (--retries);

	if (ret) {
		LOG_ERR("SDHC send command %d error %d", cmd->opcode, ret);
	}

	k_mutex_unlock(&dev_data->s_request_mutex);
	return ret;
}

/*
 * Reset SDHC controller
 */
static int sdhc_rtl87x3g_reset(const struct device *dev)
{
	LOG_DBG("[%s]", __func__);
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	uint32_t time_out = SDIO_HOST_INIT_TIMEOUT;

	SDIO_SoftwareReset(sdio_base);
	while (SDIO_GetSoftwareResetStatus(sdio_base) == SET) {
		time_out--;
		if (time_out == 0) {
			LOG_ERR("SDHC reset fail");
			return -EFAULT;
		}
	}

	return 0;
}

/*
 * Get card presence
 */
static int sdhc_rtl87x3g_get_card_present(const struct device *dev)
{
	return 1;
}

/*
 * Return 0 if card is not busy, 1 if it is
 */
static int sdhc_rtl87x3g_card_busy(const struct device *dev)
{
	LOG_DBG("[%s]", __func__);
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	uint32_t status = sdio_base->PRESENT_STATE;

	return (status & SDIO_FLAG_CMD_INHIBIT) || (status & SDIO_FLAG_CMD_DAT_INHIBIT);
}

/*
 * Get host properties
 */
static int sdhc_rtl87x3g_get_host_props(const struct device *dev, struct sdhc_host_props *props)
{
	LOG_DBG("[%s]", __func__);
	const struct sdhc_rtl87x3g_config *cfg = dev->config;

	memcpy(props, &cfg->props, sizeof(struct sdhc_host_props));
	return 0;
}

/*
 * Tune with default speed
 */
void sdhc_rtl87x3g_ds_tuning(const struct device *dev, int try_ds_func(void *data), void *usr_data)
{
	LOG_DBG("[%s]", __func__);
	struct sdhc_rtl87x3g_data *data = dev->data;
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	uint32_t sdh_bus_width = data->bus_width == 1 ? SDIO_BusWide_1b : SDIO_BusWide_4b;
	int ret = 0;
	uint8_t bus_delay = 0;
	uint32_t err_int_status;

	if (data->tuned) {
		SDH_BusDelaySel(sdio_base, sdh_bus_width, data->bus_delay);

		return;
	}

	/* Gate off do_transaction's recovery for the probes below. */
	data->tuning = true;

	while (bus_delay <= 15) {
		SDH_BusDelaySel(sdio_base, sdh_bus_width, bus_delay);

		cfg->sd_irq_disable();
		if (try_ds_func) {
			ret = try_ds_func(usr_data);
		}
		err_int_status = SDIO_GetAllErrINTStatus(sdio_base);
		if (err_int_status) {
			ret = -EIO;
			/* Failures are expected; clear and try the next delay. */
			SDIO_ClearErrorINTPendingBit(sdio_base, SDIO_INT_ALL_ERR);
		}
		cfg->sd_irq_enable();
		if (ret == 0) {
			break;
		}

		bus_delay++;
	}

	data->tuning = false;

	printk("default speed tuning: bus_delay = %d src_clock=%d bus_clock=%d\n", bus_delay, data->src_clock, data->bus_clock);

	SDH_BusDelaySel(sdio_base, sdh_bus_width, bus_delay);

	data->bus_delay = bus_delay;
}

/*
 * Tune with high speed
 */
void sdhc_rtl87x3g_hs_tuning(const struct device *dev, int try_hs_func(void *data), void *usr_data)
{
	LOG_DBG("[%s]", __func__);
	struct sdhc_rtl87x3g_data *data = dev->data;
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	uint32_t sdh_bus_width = data->bus_width == 1 ? SDIO_BusWide_1b : SDIO_BusWide_4b;
	int ret = 0;
	uint32_t err_int_status;
	uint8_t bus_delay = 0;
	uint8_t bus_delay_fail_cnt = 0;
	uint8_t delay_fail_cnt[16] = {0};
	uint16_t fail_cnt_all = 0;
	float select_delay = 0;
	uint8_t fail_min_delay = 0;
	uint8_t fail_max_delay = 0;
	uint8_t fail_min_delay_found = 0;

	if (data->tuned) {
		SDH_BusDelaySel(sdio_base, sdh_bus_width, data->bus_delay);

		return;
	}

	/* Gate off do_transaction's recovery for the probes below. */
	data->tuning = true;

	while (bus_delay <= 15) {
		SDH_BusDelaySel(sdio_base, sdh_bus_width, bus_delay);

		for (uint8_t i = 0; i < 10; i++) {
			cfg->sd_irq_disable();
			if (try_hs_func) {
				ret = try_hs_func(usr_data);
			}
			err_int_status = SDIO_GetAllErrINTStatus(sdio_base);
			if (err_int_status) {
				ret = -EIO;
				/* Failures are expected; clear and try the next delay. */
				SDIO_ClearErrorINTPendingBit(sdio_base, SDIO_INT_ALL_ERR);
			}
			cfg->sd_irq_enable();
			if (ret != 0) {
				bus_delay_fail_cnt++;
			}
		}

		delay_fail_cnt[bus_delay] = bus_delay_fail_cnt;
		fail_cnt_all = fail_cnt_all + bus_delay_fail_cnt;

		bus_delay++;
		bus_delay_fail_cnt = 0;
	}

	data->tuning = false;

	/* ROM printk mis-walks its va_list with many args, so print one per call. */
	printk("high speed tuning: fail_cnt_all %d, delay_fail_cnt:", fail_cnt_all);
	for (uint8_t delay = 0; delay <= 15; delay++) {
		printk(" %d", delay_fail_cnt[delay]);
	}
	printk("\n");

	for (uint8_t delay = 0; delay <= 15; delay++) {
		if (delay_fail_cnt[delay] != 0) {
			select_delay += delay * delay_fail_cnt[delay];

			if (!fail_min_delay_found) {
				fail_min_delay = delay;
				fail_min_delay_found = 1;
			}

			if (delay > fail_max_delay) {
				fail_max_delay = delay;
			}
		}
	}
	select_delay = select_delay / fail_cnt_all;

	bus_delay = round(select_delay);

	if (bus_delay < 5) {
		bus_delay += 5;
	} else {
		bus_delay -= 5;
	}

	/* Add handling for cases where failure delays occur multiple times, for example, failure
	 * delays from 0 to 11. */
	if (delay_fail_cnt[bus_delay] != 0) {
		if (fail_min_delay >= 5) {
			bus_delay = fail_min_delay - 5;
		} else if (fail_max_delay <= 10) {
			bus_delay = fail_max_delay + 5;
		} else {
			if (fail_min_delay < (15 - fail_max_delay)) {
				bus_delay = 15;
			} else {
				bus_delay = 0;
			}
		}
	}

	printk("high speed tuning: bus_delay = %d src_clock=%d bus_clock=%d\n", bus_delay, data->src_clock, data->bus_clock);

	SDH_BusDelaySel(sdio_base, sdh_bus_width, bus_delay);

	data->bus_delay = bus_delay;

	data->tuned = true;
}

/*
 * Change delay
 */
void sdhc_rtl87x3g_change_delay(const struct device *dev, uint8_t bus_width, uint8_t delay)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	uint32_t sdh_bus_width = bus_width == 1 ? SDIO_BusWide_1b : SDIO_BusWide_4b;

	SDH_BusDelaySel(sdio_base, sdh_bus_width, delay);
}

static int sdhc_rtl87x3g_enable_interrupt(const struct device *dev, sdhc_interrupt_cb_t callback,
					  int sources, void *user_data)
{
	LOG_DBG("[%s] line%d", __func__, __LINE__);
	struct sdhc_rtl87x3g_data *data = dev->data;
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	int ret;

	data->cb = callback;
	data->user_data = user_data;

	if (data->sdio_int_en) {
		return 0;
	}

	if (sources & SDHC_INT_SDIO) {
		if (data->bus_width == 1) {
			SDIO_ClearINTPendingBit(sdio_base, SDIO_INT_CARD);
			SDIO_INTStatusConfig(sdio_base, SDIO_INT_CARD, ENABLE);
			SDIO_INTConfig(sdio_base, SDIO_INT_CARD, ENABLE);
			data->sdio_int_en = true;
		} else {
			ret = sdhc_rtl87x3g_enable_interrupt_pin(dev);
			if (ret) {
				LOG_ERR("Enable interrupt fail. int-gpio should be configured in 4 "
					"bit mode");
				return -EIO;
			}
			data->sdio_int_en = true;
		}

	} else {
		LOG_ERR("Enable interrupt fail. Only support SDHC_INT_SDIO");
		return -ENOTSUP;
	}

	return 0;
}

static int sdhc_rtl87x3g_disable_interrupt(const struct device *dev, int sources)
{
	LOG_DBG("[%s] line%d", __func__, __LINE__);
	struct sdhc_rtl87x3g_data *data = dev->data;
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	int ret;

	if (sources & SDHC_INT_SDIO) {
		if (data->bus_width == 1) {
			SDIO_ClearINTPendingBit(sdio_base, SDIO_INT_CARD);
			SDIO_INTStatusConfig(sdio_base, SDIO_INT_CARD, DISABLE);
			SDIO_INTConfig(sdio_base, SDIO_INT_CARD, DISABLE);
			data->sdio_int_en = false;
		} else {
			ret = sdhc_rtl87x3g_disable_interrupt_pin(dev);
			if (ret) {
				LOG_ERR("dISable interrupt fail. interrupt pinctrl should be "
					"configured in 4 bit mode");
				return -EIO;
			}
			data->sdio_int_en = false;
		}
	} else {
		LOG_ERR("Disable interrupt fail. Only support SDHC_INT_SDIO");
		return -ENOTSUP;
	}

	data->cb = NULL;
	data->user_data = NULL;

	return 0;
}

/**
 * @brief SDHC interrupt handler
 *
 * All communication is handled by the hardware automatically,
 * so the isr just handles error status.
 */
static void sdio_rtl87x3g_isr(void *arg)
{
	LOG_DBG("[%s]", __func__);
	const struct device *dev = (const struct device *)arg;
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	struct sdhc_rtl87x3g_data *data = dev->data;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;

	uint16_t nor_int_status;
	uint16_t err_int_status;

	nor_int_status = SDIO_GetAllNorINTStatus(sdio_base);

	if (nor_int_status) {
		if (nor_int_status & BIT8) {
			if (data->cb) {
				data->cb(dev, SDHC_INT_SDIO, data->user_data);
			}
			SDIO_ClearINTPendingBit(sdio_base, SDIO_INT_CARD);
		}
	}

	err_int_status = SDIO_GetAllErrINTStatus(sdio_base);
	if (err_int_status) {
		LOG_ERR("sdio_handler: err 0x%x on cmd %u, BUS_DLY_SEL=0x%x", err_int_status, data->cur_cmd, sdio_base->BUS_DLY_SEL);
		/* Latch for thread-context recovery before IrqErrIntCback clears it. */
		data->sdio_err_status = err_int_status;
		extern void SDIO_IrqErrIntCback(void *SDIOx, void *int_status);
		SDIO_IrqErrIntCback(sdio_base, &err_int_status);
	}
}

/*
 * Perform early system init for SDHC
 */
static int sdhc_rtl87x3g_init(const struct device *dev)
{
	const struct sdhc_rtl87x3g_config *cfg = dev->config;
	struct sdhc_rtl87x3g_data *data = dev->data;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)cfg->sdio_base;
	int ret;

	LOG_DBG("[%s] %s initializing line%d", __func__, dev->name, __LINE__);

	/* Set power GPIO high, so card starts powered */
	if (cfg->pwr_gpio.port) {
		ret = gpio_pin_configure_dt(&cfg->pwr_gpio, GPIO_OUTPUT_ACTIVE);

		if (ret) {
			LOG_ERR("Failed to configure SDHC power pins");
			return ret;
		}
	}

	/* Pin configuration */
	RCC_SDIOClockConfig(sdio_base, SDIO_CLOCK_SOURCE_40MHZ, SDIO_CLK_DIV_1);
	ret = pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_BUS_OFF);

	if (ret < 0) {
		ret = pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_DEFAULT);
		if (ret < 0) {
			LOG_ERR("Failed to configure SDHC pins");
			return ret;
		}
		pinctrl_apply_state(cfg->pcfg, PINCTRL_STATE_INTERRUPT);
	}

	if (cfg->int_gpio.port) {
		gpio_init_callback(&sdio_int_gpio_cb, sdio_int_gpio_cb_func,
				   BIT(cfg->int_gpio.pin));
		gpio_add_callback(cfg->int_gpio.port, &sdio_int_gpio_cb);
	}

	data->initialized = false;

	k_mutex_init(&data->s_request_mutex);

	return 0;
}

#ifdef CONFIG_PM_DEVICE
static int sdhc_rtl87x3g_pm_action(const struct device *dev, enum pm_device_action action)
{
	const struct sdhc_rtl87x3g_config *config = dev->config;
	struct sdhc_rtl87x3g_data *data = dev->data;
	SDIO_TypeDef *sdio_base = (SDIO_TypeDef *)config->sdio_base;
	int err;

	extern void SDIO_DLPSEnter(void *PeriReg, void *StoreBuf);
	extern void SDIO_DLPSExit(void *PeriReg, void *StoreBuf);

	switch (action) {
	case PM_DEVICE_ACTION_SUSPEND:
		if (data->initialized) {
			SDIO_DLPSEnter(sdio_base, &data->store_buf);
		}

		if (data->power_mode == SDHC_POWER_ON) {
			/* Move pins to sleep state */
			err = pinctrl_apply_state(config->pcfg, PINCTRL_STATE_SLEEP);
			if ((err < 0) && (err != -ENOENT)) {
				return err;
			}
		}

		break;
	case PM_DEVICE_ACTION_RESUME:
		if (data->initialized) {
			(void)clock_control_on(RTL87X3G_CLOCK_CONTROLLER,
						(clock_control_subsys_t)&config->clkid);

			sdhc_rtl87x3g_set_clk_src(dev, data->bus_clock);
		}

		if (data->power_mode == SDHC_POWER_ON) {
			/* Set pins to active state */
			err = pinctrl_apply_state(config->pcfg, PINCTRL_STATE_DEFAULT);
			if (err < 0) {
				return err;
			}

			if (!data->sdio_int_en) {
				pinctrl_apply_state(config->pcfg, PINCTRL_STATE_INTERRUPT);
			}
		}

		if (data->initialized) {
			SDIO_DLPSExit(sdio_base, &data->store_buf);
		}

		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}
#endif /* CONFIG_PM_DEVICE */

static const struct sdhc_driver_api sdhc_api = {
	.reset = sdhc_rtl87x3g_reset,
	.request = sdhc_rtl87x3g_request,
	.set_io = sdhc_rtl87x3g_set_io,
	.get_card_present = sdhc_rtl87x3g_get_card_present,
	.card_busy = sdhc_rtl87x3g_card_busy,
	.get_host_props = sdhc_rtl87x3g_get_host_props,
	.enable_interrupt = sdhc_rtl87x3g_enable_interrupt,
	.disable_interrupt = sdhc_rtl87x3g_disable_interrupt,
};

#define SDHC_RTL87X3G_INIT(n)                                                                      \
                                                                                                   \
	PINCTRL_DT_DEFINE(DT_DRV_INST(n));                                                         \
	static void sdio_rtl87x3g_irq_enable_##n(void)                                             \
	{                                                                                          \
		irq_enable(DT_INST_IRQN(n));                                                       \
	}                                                                                          \
	static void sdio_rtl87x3g_irq_disable_##n(void)                                            \
	{                                                                                          \
		irq_disable(DT_INST_IRQN(n));                                                      \
	}                                                                                          \
	static void sdio_rtl87x3g_irq_connect_##n(void)                                            \
	{                                                                                          \
		IRQ_CONNECT(DT_INST_IRQN(n), DT_INST_IRQ(n, priority), sdio_rtl87x3g_isr,          \
			    DEVICE_DT_INST_GET(n), 0);                                             \
		irq_enable(DT_INST_IRQN(n));                                                       \
	}                                                                                          \
	static const struct sdhc_rtl87x3g_config sdhc_rtl87x3g_##n##_config = {                    \
		.sdio_base = (SDIO_TypeDef *)DT_INST_REG_ADDR(n),                                  \
		.clkid = DT_INST_CLOCKS_CELL(n, id),                                               \
		.sd_irq_connect = sdio_rtl87x3g_irq_connect_##n,                                   \
		.sd_irq_enable = sdio_rtl87x3g_irq_enable_##n,                                     \
		.sd_irq_disable = sdio_rtl87x3g_irq_disable_##n,                                   \
		.pin_group = DT_INST_PROP(n, pin_group),                                           \
		.pcfg = PINCTRL_DT_DEV_CONFIG_GET(DT_DRV_INST(n)),                                 \
		.pwr_gpio = GPIO_DT_SPEC_INST_GET_OR(n, pwr_gpios, {0}),                           \
		.int_gpio = GPIO_DT_SPEC_INST_GET_OR(n, int_gpios, {0}),                           \
		.props = {.is_spi = false,                                                         \
			  .f_max = DT_INST_PROP(n, max_bus_freq),                                  \
			  .f_min = DT_INST_PROP(n, min_bus_freq),                                  \
			  .max_current_330 = DT_INST_PROP(n, max_current_330),                     \
			  .max_current_180 = DT_INST_PROP(n, max_current_180),                     \
			  .power_delay = DT_INST_PROP_OR(n, power_delay_ms, 0),                    \
			  .host_caps = {.vol_180_support = false,                                  \
					.vol_300_support = false,                                  \
					.vol_330_support = true,                                   \
					.suspend_res_support = false,                              \
					.sdma_support = false,                                     \
					.high_spd_support = true,                                  \
					.adma_2_support = false,                                   \
					.max_blk_len = 0,                                          \
					.ddr50_support = false,                                    \
					.sdr104_support = false,                                   \
					.sdr50_support = false,                                    \
					.uhs_2_support = false,                                    \
					.bus_8_bit_support = false,                                \
					.bus_4_bit_support =                                       \
						(DT_INST_PROP(n, bus_width) == 4) ? true : false,  \
					.hs200_support = false,                                    \
					.hs400_support = false}}};                                 \
                                                                                                   \
	static struct sdhc_rtl87x3g_data sdhc_rtl87x3g_##n##_data = {                              \
		.bus_width = DT_INST_PROP(n, bus_width),                                           \
		.src_clock = 40000000,                                                             \
		.bus_clock = 5000000,                                                              \
		.power_mode = SDHC_POWER_ON,                                                       \
		.timing = SDHC_TIMING_LEGACY,                                                      \
	};                                                                                         \
                                                                                                   \
	PM_DEVICE_DT_INST_DEFINE(n, sdhc_rtl87x3g_pm_action);                                      \
	DEVICE_DT_INST_DEFINE(n, &sdhc_rtl87x3g_init, PM_DEVICE_DT_INST_GET(n),                    \
			      &sdhc_rtl87x3g_##n##_data, &sdhc_rtl87x3g_##n##_config, POST_KERNEL, \
			      CONFIG_SDHC_INIT_PRIORITY, &sdhc_api);

DT_INST_FOREACH_STATUS_OKAY(SDHC_RTL87X3G_INIT)
