/*
 * Copyright (c) 2016 Intel Corporation.
 * Copyright (c) 2022-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <zephyr/types.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/disk.h>
#include <errno.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include "trace.h"
#include "os_sync.h"
#include "storage.h"
#include "nftl.h"
#include "patch_header_check.h"
#include "rtk_errno.h"

LOG_MODULE_REGISTER(flashdisk, 3);

struct flashdisk_data {
	struct disk_info info;
	struct k_mutex lock;
	const T_STORAGE_PARTITION_INFO rtk_partition_info;
	const size_t disk_size;
	const size_t block_size;
	const size_t page_size;
};

static int disk_flash_access_status(struct disk_info *disk)
{
	return DISK_STATUS_OK;
}

static int disk_flash_access_init(struct disk_info *disk)
{
	const T_STORAGE_PARTITION_INFO info = storage_partition_get(FILESYSTEM_NAME);

	bool ret = nftl_module_init(FILESYSTEM_NAME, info.size);
	LOG_DBG("nftl_module_init info size0x%x ret %d\n", info.size, ret);
	return !ret;
}

static int rtk_flash_read(const struct device * dev, uint32_t offset, void *data, size_t len)
{
	uint32_t ret = nftl_module_read(FILESYSTEM_NAME, offset, data, len);
	LOG_DBG("rtk_flash_read offset 0x%x, data 0x%p, len 0x%x, ret %d\n", offset, data, len, ret);
	if(ret != ESUCCESS && ret != ENOF)
	{
		return -1;
	}
	return 0;
}

static int rtk_flash_write(const struct device * dev, uint32_t offset, void * data, size_t len)
{
	uint32_t ret = nftl_module_write(FILESYSTEM_NAME, offset, data, len);
	LOG_DBG("rtk_flash_write offset 0x%x, data 0x%p, len 0x%x, ret %d\n", offset, data, len, ret);
	if(ret != ESUCCESS)
	{
		return -1;
	}
	return 0;
}

static int rtk_flash_erase_sectors(const struct device * dev, uint32_t offset, uint32_t size)
{
	uint32_t ret = nftl_module_release(FILESYSTEM_NAME, offset, size);
	LOG_DBG("rtk_flash_erase_sectors offset 0x%x, size 0x%x, ret %d\n", offset, size, ret);
	if(ret != ESUCCESS)
	{
		return -1;
	}
	return 0;
}

static bool sectors_in_range(struct flashdisk_data *ctx,
			     uint32_t start_sector, uint32_t sector_count)
{
	uint32_t start, end;

	start = (start_sector * ctx->page_size);
	end = start + (sector_count * ctx->page_size);

	if ((end >= start) && (end <= ctx->disk_size)) {
		return true;
	}

	LOG_ERR("sector start %" PRIu32 " count %" PRIu32
		" outside partition boundary", start_sector, sector_count);
	return false;
}

static int disk_flash_access_read(struct disk_info *disk, uint8_t *buff,
				uint32_t start_sector, uint32_t sector_count)
{
	struct flashdisk_data *ctx;
	int rc = 0;

	ctx = CONTAINER_OF(disk, struct flashdisk_data, info);

	if (!sectors_in_range(ctx, start_sector, sector_count)) {
		return -EINVAL;
	}

	k_mutex_lock(&ctx->lock, K_FOREVER);
    if(rtk_flash_read(disk->dev, start_sector * ctx->page_size, buff, sector_count * ctx->page_size) < 0)
	{
		rc = -EIO;
		goto end;
	}
end:
	k_mutex_unlock(&ctx->lock);

	return rc;
}

static int disk_flash_access_write(struct disk_info *disk, const uint8_t *buff,
				 uint32_t start_sector, uint32_t sector_count)
{
	struct flashdisk_data *ctx;
	int rc = 0;

	ctx = CONTAINER_OF(disk, struct flashdisk_data, info);

	if (!sectors_in_range(ctx, start_sector, sector_count)) {
		return -EINVAL;
	}

	k_mutex_lock(&ctx->lock, K_FOREVER);


	if (rtk_flash_write(disk->dev, start_sector * ctx->page_size, (void *)buff, sector_count * ctx->page_size) < 0) {
		rc = -EIO;
		goto end;
	}

end:
	k_mutex_unlock(&ctx->lock);

	return 0;
}

static int disk_flash_access_ioctl(struct disk_info *disk, uint8_t cmd, void *buff)
{
	int rc;
	struct flashdisk_data *ctx;

	ctx = CONTAINER_OF(disk, struct flashdisk_data, info);

	switch (cmd) {
	case DISK_IOCTL_CTRL_DEINIT:
	case DISK_IOCTL_CTRL_SYNC:
		return 0;
	case DISK_IOCTL_GET_SECTOR_COUNT:
		const T_STORAGE_PARTITION_INFO info = storage_partition_get(FILESYSTEM_NAME);
		*(uint32_t *)buff = info.size / ctx->page_size;
		LOG_DBG("DISK_IOCTL_GET_SECTOR_COUNT 0x%x\n", *(uint32_t *)buff);
		return 0;
	case DISK_IOCTL_GET_SECTOR_SIZE:
		LOG_DBG("DISK_IOCTL_GET_SECTOR_SIZE 0x%x\n", ctx->page_size);
		*(uint32_t *)buff = ctx->page_size;
		return 0;
	case DISK_IOCTL_GET_ERASE_BLOCK_SZ: /* in sectors */
		*(uint32_t *)buff = 1;
		LOG_DBG("DISK_IOCTL_GET_ERASE_BLOCK_SZ 0x%x\n", *(uint32_t *)buff);
		return 0;
	case DISK_IOCTL_CTRL_INIT:
		return disk_flash_access_init(disk);
	case DISK_IOCTL_ERASE_SECTORS:
		uint32_t addr[2] = {0};
		memcpy(addr, buff, sizeof(addr));
		LOG_DBG("DISK_IOCTL_ERASE_SECTORS 0x%x 0x%x\n", addr[0], addr[1]);
		return rtk_flash_erase_sectors(disk->dev, addr[0] * ctx->page_size, (addr[1] - addr[0]) * ctx->page_size);
	default:
		break;
	}

	return -EINVAL;
}

static const struct disk_operations flash_disk_ops = {
	.init = disk_flash_access_init,
	.status = disk_flash_access_status,
	.read = disk_flash_access_read,
	.write = disk_flash_access_write,
	.ioctl = disk_flash_access_ioctl,
};

static int disk_flash_init(const struct device *dev)
{
	int err = 0;
	int rc;

	struct flashdisk_data *flash_disks = dev->data;

	k_mutex_init(&flash_disks->lock);

	rc = disk_access_register(&flash_disks->info);
	if (rc < 0) {
		LOG_ERR("Failed to register disk %s error %d",
			flash_disks->info.name, rc);
		err = rc;
	}
	storage_partition_init(&flash_disks->rtk_partition_info, 1);

	return err;
}


#define DT_DRV_COMPAT realtek_nand_flash_disk

#define DEFINE_FLASHDISKS_DEVICE(n)						\
	static struct flashdisk_data flash_disks##n = {						\
		.info = {												\
			.ops = &flash_disk_ops,								\
			.name = DT_INST_PROP(n, disk_name),					\
		},														\
		.rtk_partition_info = {									\
			.name = FILESYSTEM_NAME,							\
			.size = DT_INST_PROP(n, disk_size),					\
		},														\
		.disk_size = DT_INST_PROP(n, disk_size),				\
		.page_size = DT_INST_PROP(n, page_size),				\
		.block_size = DT_INST_PROP(n, block_size),				\
	};															\
	DEVICE_DT_INST_DEFINE(n,						\
		&disk_flash_init,					\
		NULL,							\
		&flash_disks##n,					\
		NULL,							\
		APPLICATION,						\
		CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,				\
		NULL);																								

DT_INST_FOREACH_STATUS_OKAY(DEFINE_FLASHDISKS_DEVICE)

// SYS_INIT(disk_flash_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
