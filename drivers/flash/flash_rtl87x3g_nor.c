/*
 * Copyright(c) 2026, Realtek Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/logging/log.h>

#include <fmc_api.h>

#define DT_DRV_COMPAT realtek_rtl87x3g_nor_flash_controller

#define RTL87X3G_NOR_FLASH_COMPAT(node_id)                                                              \
	COND_CODE_1(DT_NODE_HAS_COMPAT(node_id, soc_nv_flash), (node_id), ())

#define RTL87X3G_NOR_FLASH_NODE(node_id) DT_INST_FOREACH_CHILD_STATUS_OKAY(node_id, RTL87X3G_NOR_FLASH_COMPAT)

#define FLASH_ERASE_BLK_SZ DT_PROP(RTL87X3G_NOR_FLASH_NODE(0), erase_block_size)
#define FLASH_WRITE_BLK_SZ DT_PROP(RTL87X3G_NOR_FLASH_NODE(0), write_block_size)

/* Erase block must be 4KB (4096) - 64KB support TBD */
BUILD_ASSERT(FLASH_ERASE_BLK_SZ == 4096, "FLASH_ERASE_BLK_SZ must be 4KB (64KB TBD)");
/* Write has no alignment restriction, set to 1 byte */
BUILD_ASSERT(FLASH_WRITE_BLK_SZ == 1, "FLASH_WRITE_BLK_SZ must be 1");

#define FLASH_SIZE DT_REG_SIZE(RTL87X3G_NOR_FLASH_NODE(0))
#define FLASH_ADDR DT_REG_ADDR(RTL87X3G_NOR_FLASH_NODE(0))

LOG_MODULE_REGISTER(flash_rtl87x3g_nor, CONFIG_FLASH_LOG_LEVEL);

#ifdef CONFIG_FLASH_PAGE_LAYOUT
static const struct flash_pages_layout flash_pages_layout_rtl87x3g_nor[] =
{
    {
	.pages_size = FLASH_ERASE_BLK_SZ,
	.pages_count = FLASH_SIZE / FLASH_ERASE_BLK_SZ
    }
};
static void flash_rtl87x3g_nor_page_layout(const struct device *dev,
				       const struct flash_pages_layout **layout,
				       size_t *layout_size)
{
    *layout = flash_pages_layout_rtl87x3g_nor;

    /*
     * For flash memories which have uniform page sizes, this routine
     * returns an array of length 1, which specifies the page size and
     * number of pages in the memory.
     */
    *layout_size = 1;
}
#endif

static const struct flash_parameters flash_rtl87x3g_nor_parameters =
{
    .write_block_size = FLASH_WRITE_BLK_SZ,
    .erase_value = 0xff,
};

/**
 * @brief Check if offset and len are valid (protect against negative values, overflows, and
 * out-of-bounds)
 */
static int flash_rtl87x3g_nor_check_bounds(off_t offset, size_t len)
{
	if (offset < 0 || offset >= FLASH_SIZE || (FLASH_SIZE - offset) < len) {
		LOG_DBG("offset(%ld) or len(%zu) out of bounds", offset, len);
		return -EINVAL;
	}

	return 0;
}

static int flash_rtl87x3g_nor_read(const struct device *dev, off_t offset,
			       void *data, size_t len)
{
    if (len == 0U) {
		return 0;
	}

	if (data == NULL) {
		LOG_ERR("Read data buffer is NULL");
		return -EINVAL;
	}

    int rc = flash_rtl87x3g_nor_check_bounds(offset, len);

	if (rc != 0) {
		return rc;
	}

	bool ret = fmc_flash_nor_read(FLASH_ADDR + offset, (uint8_t *)data, len);

	if (ret) {
		return 0;
	} else {
        LOG_ERR("flash_rtl87x3g_nor_read failed");
        return -EIO;
    }
}

static int flash_rtl87x3g_nor_write(const struct device *dev, off_t offset,
				const void *data, size_t len)
{
	if (len == 0U) {
		return 0;
	}

	if (data == NULL) {
		LOG_ERR("Write data buffer is NULL");
		return -EINVAL;
	}

    int rc = flash_rtl87x3g_nor_check_bounds(offset, len);

	if (rc != 0) {
		return rc;
	}

#if CONFIG_KERNEL_MEM_POOL && CONFIG_HEAP_MEM_POOL_SIZE
	if ((uint32_t)data >= FLASH_ADDR) {
		uint8_t *tmp = k_malloc(len);

		if (tmp != NULL) {
			fmc_flash_nor_read((uint32_t)data, (uint8_t *)tmp, len);
			fmc_flash_nor_write(FLASH_ADDR + offset, (uint8_t *)tmp, len);
			k_free(tmp);
		} else {
			LOG_ERR("k_malloc 0x%x for flash data transfer station failed", len);
		}
		return 0;
	}
#else
	__ASSERT((uint32_t)data < FLASH_ADDR,
		"not supported: Data in flash (0x%x) cannot be used as source",
		(uint32_t)data);
#endif

    bool ret = fmc_flash_nor_write(FLASH_ADDR + offset, (uint8_t *)data, len);

    if (ret) {
		return 0;
	} else {
        LOG_ERR("flash_rtl87x3g_nor_write failed");
        return -EIO;
    }

    return 0;
}

static int flash_rtl87x3g_nor_erase(const struct device *dev, off_t offset, size_t len)
{
	bool ret;

    if (len == 0U) {
		return 0;
	}

	if ((offset % FLASH_ERASE_BLK_SZ) != 0) {
		LOG_ERR("offset %ld: not aligned to erase block size %d", offset,
			FLASH_ERASE_BLK_SZ);
		return -EINVAL;
	}

	if ((len % FLASH_ERASE_BLK_SZ) != 0) {
		LOG_ERR("len %zu: not aligned to erase block size %d", len, FLASH_ERASE_BLK_SZ);
		return -EINVAL;
	}

	int rc = flash_rtl87x3g_nor_check_bounds(offset, len);

	if (rc != 0) {
		return rc;
	}

	uint32_t addr = FLASH_ADDR + offset;

	for (size_t remaining = len; remaining > 0; remaining -= FLASH_ERASE_BLK_SZ) {
		ret = fmc_flash_nor_erase(addr, FMC_FLASH_NOR_ERASE_SECTOR);
		if (ret != true) {
			LOG_ERR("fmc_flash_nor_erase failed: %d", ret);
			return -EIO;
		}
		addr += FLASH_ERASE_BLK_SZ;
	}

	return 0;
}

static const struct flash_parameters *
flash_rtl87x3g_nor_get_parameters(const struct device *dev)
{
    ARG_UNUSED(dev);

    return &flash_rtl87x3g_nor_parameters;
}

static const struct flash_driver_api flash_rtl87x3g_nor_driver_api =
{
    .read = flash_rtl87x3g_nor_read,
    .write = flash_rtl87x3g_nor_write,
    .erase = flash_rtl87x3g_nor_erase,
    .get_parameters = flash_rtl87x3g_nor_get_parameters,
#ifdef CONFIG_FLASH_PAGE_LAYOUT
    .page_layout = flash_rtl87x3g_nor_page_layout,
#endif
};


DEVICE_DT_INST_DEFINE(0, NULL, NULL,
			NULL, NULL, POST_KERNEL,
			CONFIG_FLASH_INIT_PRIORITY, &flash_rtl87x3g_nor_driver_api);
