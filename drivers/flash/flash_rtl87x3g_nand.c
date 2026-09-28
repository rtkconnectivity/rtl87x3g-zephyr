/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT realtek_rtl87x3g_nand_flash_controller

#define SOC_NV_FLASH_NODE DT_INST(0, soc_nv_flash)
#define FLASH_WRITE_BLK_SZ DT_PROP(SOC_NV_FLASH_NODE, write_block_size)
#define FLASH_ERASE_BLK_SZ DT_PROP(SOC_NV_FLASH_NODE, erase_block_size)
#define FLASH_BASE_ADDR DT_REG_ADDR(SOC_NV_FLASH_NODE)

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/logging/log.h>

#include <fmc_api.h>
#include <fmc_api_ext.h>

LOG_MODULE_REGISTER(flash_rtl87x3g_nand, CONFIG_FLASH_LOG_LEVEL);

struct flash_rtl87x3g_dev_data {
	struct k_sem sem;
#if CONFIG_FLASH_PAGE_LAYOUT
	struct flash_pages_layout *page_layout;
#endif /* CONFIG_FLASH_PAGE_LAYOUT */
	struct flash_parameters *parameters;
};

static int flash_rtl87x3g_read(const struct device *dev, off_t address, void *buffer, size_t length)
{
	bool ret = 0;
	struct flash_rtl87x3g_dev_data *data = dev->data;

	address += FLASH_BASE_ADDR;

	LOG_DBG("flash_rtl87x3g_read address 0x%x, length %d",
		(int)address, (int)length);

	k_sem_take(&data->sem, K_FOREVER);
	ret = fmc_flash_nand_read((uint32_t)address, buffer, (uint32_t)length);
	k_sem_give(&data->sem);
	if (ret == 0) {
		LOG_ERR("flash_rtl87x3g_read failed %d", ret);
		return -EIO;
	}
	return 0;
}


static int flash_rtl87x3g_write(const struct device *dev,
			     off_t address,
			     const void *buffer,
			     size_t length)
{
	bool ret = 0;
	struct flash_rtl87x3g_dev_data *data = dev->data;
	size_t write_block_size = data->parameters->write_block_size;

	address += FLASH_BASE_ADDR;

	LOG_DBG("flash_rtl87x3g_write address 0x%x, length %d",
		(int)address, (int)length);

	if(address % write_block_size != 0) {
		LOG_ERR("flash_rtl87x3g_write address not aligned %d",
			(int)(address % write_block_size));
		return -EINVAL;
	}

	k_sem_take(&data->sem, K_FOREVER);
	ret = fmc_flash_nand_page_write((uint32_t)address, (uint8_t *)buffer, (uint32_t)length);

	k_sem_give(&data->sem);
	if (ret == 0) {
		LOG_ERR("flash_rtl87x3g_write failed %d", ret);
		return -EIO;
	}
	return 0;
}

static int flash_rtl87x3g_erase(const struct device *dev, off_t start, size_t len)
{
	bool ret = 0;
	struct flash_rtl87x3g_dev_data *data = dev->data;
	size_t erase_block_size = data->page_layout->pages_size;

	start += FLASH_BASE_ADDR;

	LOG_DBG("flash_rtl87x3g_erase start 0x%x, len %d",
		(int)start, (int)len);

	if (start % erase_block_size != 0) {
		LOG_ERR("flash_rtl87x3g_erase start not aligned ! addr: %d align: %d",
			(int)(start), (int)erase_block_size);
		return -EINVAL;
	}

	for (uint32_t address = (uint32_t)start;
	     address < (uint32_t)(start + len);
	     address += erase_block_size)
	{
		k_sem_take(&data->sem, K_FOREVER);
		ret = fmc_flash_nand_erase_block(address);
		k_sem_give(&data->sem);
		if (ret == 0) {
			LOG_ERR("flash_rtl87x3g_write failed %d", ret);
			return -EIO;
		}
	}

	return 0;
}

#if CONFIG_FLASH_PAGE_LAYOUT
void flash_rtl87x3g_page_layout(const struct device *dev,
			     const struct flash_pages_layout **layout,
			     size_t *layout_size)
{
	struct flash_rtl87x3g_dev_data *data = dev->data;
	if(data->page_layout != NULL) {
		*layout = data->page_layout;
		*layout_size = 1;
	}
}
#endif /* CONFIG_FLASH_PAGE_LAYOUT */

static const struct flash_parameters *flash_rtl87x3g_get_parameters(const struct device *dev)
{
	ARG_UNUSED(dev);
	struct flash_rtl87x3g_dev_data *data = dev->data;
	if (data->parameters != NULL) {
		return data->parameters;
	}
	return NULL;
}

static int flash_rtl87x3g_init(const struct device *dev)
{
	struct flash_rtl87x3g_dev_data *data = dev->data;
	k_sem_init(&data->sem, 1, 1);
	return 0;
}

static const struct flash_driver_api flash_rtl87x3g_driver_api = {
	.read = flash_rtl87x3g_read,
	.write = flash_rtl87x3g_write,
	.erase = flash_rtl87x3g_erase,
	.get_parameters = flash_rtl87x3g_get_parameters,
#ifdef CONFIG_FLASH_PAGE_LAYOUT
	.page_layout = flash_rtl87x3g_page_layout,
#endif
};



#if CONFIG_FLASH_PAGE_LAYOUT
#define FLASH_RTL87X3G_NAND_DEFINE(index)  \
	static struct flash_pages_layout flash_rtl87x3g_pages_layout##index = {	\
		.pages_count = DT_REG_SIZE(SOC_NV_FLASH_NODE) / FLASH_ERASE_BLK_SZ,	\
		.pages_size = DT_PROP(SOC_NV_FLASH_NODE, erase_block_size),	\
	};	\
	static struct flash_parameters flash_rtl87x3g_parameters_##index = {	\
		.write_block_size = FLASH_WRITE_BLK_SZ,	\
		.erase_value = 0xFF,	\
	};	\
	static struct flash_rtl87x3g_dev_data flash_rtl87x3g_data_##index = {	\
		.page_layout = &flash_rtl87x3g_pages_layout##index,	\
		.parameters = &flash_rtl87x3g_parameters_##index,	\
	};	\
	DEVICE_DT_INST_DEFINE(0, flash_rtl87x3g_init,	\
		NULL,	\
		&flash_rtl87x3g_data_##index, NULL,	\
		POST_KERNEL, CONFIG_FLASH_INIT_PRIORITY,	\
		&flash_rtl87x3g_driver_api);	\

#else
#define FLASH_RTL87X3G_NAND_DEFINE(index)   	\
	static const struct flash_parameters flash_rtl87x3g_parameters_##index = {	\
		.write_block_size = FLASH_WRITE_BLK_SZ,	\
		.erase_value = 0xFF,	\
	};	\
	static struct flash_rtl87x3g_dev_data flash_rtl87x3g_data_##index = {	\
		.page_layout = NULL,	\
		.parameters = &flash_rtl87x3g_parameters_##index,	\
	};	\
	DEVICE_DT_INST_DEFINE(0, flash_rtl87x3g_init,	\
		NULL,	\
		&flash_rtl87x3g_data_##index, NULL,	\
		POST_KERNEL, CONFIG_FLASH_INIT_PRIORITY,	\
		&flash_rtl87x3g_driver_api);	\

#endif /* CONFIG_FLASH_PAGE_LAYOUT */  

DT_INST_FOREACH_STATUS_OKAY(FLASH_RTL87X3G_NAND_DEFINE)

