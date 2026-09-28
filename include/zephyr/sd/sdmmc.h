/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public API for SD memory card subsystem
 */

#ifndef ZEPHYR_INCLUDE_SD_SDMMC_H_
#define ZEPHYR_INCLUDE_SD_SDMMC_H_

#include <zephyr/device.h>
#include <zephyr/drivers/sdhc.h>
#include <zephyr/sd/sd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Write blocks to SD card from buffer
 *
 * Writes blocks from SD buffer to SD card. For best performance, this buffer
 * should be aligned to CONFIG_SDHC_BUFFER_ALIGNMENT
 * @param card SD card to write from
 * @param wbuf write buffer
 * @param start_block first block to write to
 * @param num_blocks number of blocks to write
 * @retval 0 write succeeded
 * @retval -EBUSY: card is busy with another request
 * @retval -ETIMEDOUT: card write timed out
 * @retval -EIO: I/O error
 */
int sdmmc_write_blocks(struct sd_card *card, const uint8_t *wbuf,
	uint32_t start_block, uint32_t num_blocks);

/**
 * @brief Read block from SD card to buffer
 *
 * Reads blocks into SD buffer from SD card. For best performance, this buffer
 * should be aligned to CONFIG_SDHC_BUFFER_ALIGNMENT
 * @param card SD card to read from
 * @param rbuf read buffer
 * @param start_block first block to read from
 * @param num_blocks number of blocks to read
 * @retval 0 read succeeded
 * @retval -EBUSY: card is busy with another request
 * @retval -ETIMEDOUT: card read timed out
 * @retval -EIO: I/O error
 */
int sdmmc_read_blocks(struct sd_card *card, uint8_t *rbuf,
	uint32_t start_block, uint32_t num_blocks);

/**
 * @brief Get I/O control data from SD card
 *
 * Sends I/O control commands to SD card.
 * @param card SD card
 * @param cmd I/O control command
 * @param buf I/O control buf
 * @retval 0 IOCTL command succeeded
 * @retval -ENOTSUP: IOCTL command not supported
 * @retval -EIO: I/O failure
 */
int sdmmc_ioctl(struct sd_card *card, uint8_t cmd, void *buf);

#if defined(CONFIG_SOC_SERIES_RTL87X3G)
/**
 * @brief Physically erase a sector range on an SD card
 *
 * Issues CMD32/CMD33/CMD38 to erase sectors [start_block, end_block], resetting
 * the FTL so sustained write speed returns toward the factory value. This
 * destroys all data in the range. Mirrors mmc_erase() for SD cards.
 * @param card SD card
 * @param start_block first sector to erase
 * @param end_block last sector to erase (inclusive)
 * @retval 0 erase succeeded
 * @retval -ETIMEDOUT: card stayed busy past the erase timeout
 * @retval -EIO: I/O failure
 */
int sdmmc_erase(struct sd_card *card, uint32_t start_block, uint32_t end_block);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_SD_SDMMC_H_ */
