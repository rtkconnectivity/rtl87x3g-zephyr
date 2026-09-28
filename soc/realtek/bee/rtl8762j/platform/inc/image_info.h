/**
  ******************************************************************************
  * @file    patch_header_check.h
  * @author
  * @version V0.0.1
  * @date    2017-09-01
  * @brief   This file contains all the functions regarding patch header check.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _IMAGE_INFO_H_
#define _IMAGE_INFO_H_
#include <stdbool.h>
#include <stdint.h>

/** @addtogroup  FLASH_DEVICE    Flash Device
    * @{
    */
/*============================================================================*
  *                                   Constants
  *============================================================================*/
// *INDENT-ON*
#define DEFAULT_HEADER_SIZE                 (0x400)
#define CMAC_BYTE_SIZE                      (16)
#define RSA_PUBLIC_KEY_BYTE_SIZE            (384)
#define ECDSA_PUBLIC_KEY_BYTE_SIZE          (32)
// ecdsa uncompressed key len = (public key byte len * 2) + 1
#define ECDSA_UNCOMPRESSED_PUBLIC_KEY_LEN   (65)
#define ED25519_PUBLIC_KEY_BYTE_SIZE        (32)
#define SHA256_BYTE_SIZE                    (32)

#define RSA_SIGNATURE_BYTE_SIZE             (RSA_PUBLIC_KEY_BYTE_SIZE)
#define ECDSA_SIGNATURE_BYTE_SIZE           (ECDSA_PUBLIC_KEY_BYTE_SIZE * 2)
#define ED25519_SIGNATURE_BYTE_SIZE         (ED25519_PUBLIC_KEY_BYTE_SIZE * 2)

/*
        - mac range         = signature + hash + header content + payload
        - signature range   = hash + header content + payload
        - hash range        = header content + payload
*/
#define MAC_RANGE(header)           (((T_IMG_HEADER_FORMAT *)(header))->ctrl_header.payload_len + get_image_header_format_size() - CMAC_BYTE_SIZE)

#define SIGNATURE_RANGE(header)     (((T_IMG_HEADER_FORMAT *)(header))->ctrl_header.payload_len + get_image_header_format_size() - CMAC_BYTE_SIZE - RSA_SIGNATURE_BYTE_SIZE)

#define HASH_RANGE(header)          (((T_IMG_HEADER_FORMAT *)(header))->ctrl_header.payload_len + get_image_header_format_size() - CMAC_BYTE_SIZE - RSA_SIGNATURE_BYTE_SIZE - SHA256_BYTE_SIZE)


/*IC Type refer to WIKI: https://wiki.realtek.com/display/Bee1/BT+SOC+IC+Type*/
#define IMG_IC_TYPE                  18

#define UUID_SIZE                    16
#define DFU_HEADER_SIZE              12  /*currently, first 12 byte only will be treated as image header*/
#define IMG_HEADER_SIZE              1024
#define OTA_HEADER_SIZE              0x1000

#define UP_ALIGN(size, align)        (((size) + (align) - 1) & ~(align - 1))

#define SYS_CFG_SIGNATURE            0x12345bb3
#define FLASH_TABLE_MAGIC_PATTERN    (0x5A5A12A5)

#define GOLDEN_PATTERN_WORD_LEN  (20)
#define GOLDEN_PATTERN_SIZE      (sizeof(uint32_t) * GOLDEN_PATTERN_WORD_LEN)

/*============================================================================*
  *                                   Types
  *============================================================================*/
/** @defgroup FLASH_DEVICE_Exported_Types Flash Device Exported Types
  * @brief
  * @{
  */
typedef enum
{
    IMAGE_FIRST       = 0x278D,
    IMG_SCCD          = 0x278D,
    IMG_OCCD          = 0x278E,
    IMG_BOOTPATCH     = 0x278F,
    IMG_OTA           = 0x2790, /**< OTA header */
    IMG_RSVD          = 0x2791,
    IMG_BANK_FIRST    = IMG_RSVD,
    IMG_MCUPATCH      = 0x2792,
    IMG_MCUAPP        = 0x2793,
    IMG_MCUAPPDATA1   = 0x2794,
    IMG_MCUAPPDATA2   = 0x2795,
    IMG_MCUAPPDATA3   = 0x2796,
    IMG_MCUAPPDATA4   = 0x2797,
    IMG_MCUAPPDATA5   = 0x2798,
    IMG_MCUCFGDATA    = 0x2799,
    IMG_BT_HOST_PATCH       = 0x279a,
    IMG_BT_CONTROLLER_PATCH = 0x279b,
    IMG_MAX,

    PRE_IMAGE_FIRST       = 0x27a1,
    PRE_IMG_PLATFORM      = PRE_IMAGE_FIRST,
    PRE_IMG_BT_CONTROLLER = 0x27a2,
    PRE_IMG_BT_HOST       = 0x27a3,
    PRE_IMG_MAX,
} IMG_ID;

typedef enum
{
    FLASH_OCCD,
    FLASH_BOOT_PATCH,
    FLASH_OTA_BANK_0,
    FLASH_OTA_BANK_1,
    FLASH_BKP_DATA1,
    FLASH_BKP_DATA2,
    FLASH_OTA_TMP,
    FLASH_FTL,
    FLASH_TOTAL,
} FLASH_LAYOUT_NAME;

typedef enum _ENC_KEY_SELECT
{
    ENC_KEY_SCEK = 0,
    ENC_KEY_SCEK_WITH_RTKCONST,
    ENC_KEY_OCEK,
    ENC_KEY_OCEK_WITH_OEMCONST,
    ENC_KEY_ON_FLASH,
    ENC_KEY_MAX,
} ENC_KEY_SELECT;

typedef union
{
    uint8_t d8[ECDSA_UNCOMPRESSED_PUBLIC_KEY_LEN];
    struct
    {
        uint8_t flag; /*04 for uncompressed public key*/
        uint8_t x[ECDSA_PUBLIC_KEY_BYTE_SIZE];
        uint8_t y[ECDSA_PUBLIC_KEY_BYTE_SIZE];
    } ecdsa_pb_key;
} PUBLIC_KEY;

typedef enum _APP_IMAGE_TYPE
{
    IMAGE_NORMAL              = 0,
    IMAGE_COMPRESSED          = 1,
    //2-7 are reserved
} T_IMAGE_TYPE;

typedef struct _IMG_CTRL_HEADER_FORMAT
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t header_version: 8;
uint32_t device_type:
            4; //  eflash: 4; nor :3; nand :2: , sd : 1, emmc :0 , whatever: 15   nand\sd\emmc: 14
            uint32_t integrity_check_en_in_boot: 1; // enable image integrity check in boot flow
            uint32_t load_when_boot: 1; // load image when boot
            uint32_t not_ready: 1; //for copy image in ota
            uint32_t not_obsolete: 1; //for copy image in ota
            uint32_t compressed_not_ready: 1;
            uint32_t compressed_not_obsolete: 1;
            uint32_t compressed_image_type: 3;
            uint32_t ctrl_flag_reserved: 11;
        };
    } ctrl_flag;
    uint8_t ic_type;
    uint8_t secure_version;
    uint16_t image_id;
    uint32_t header_len;
    uint32_t payload_len;
} T_IMG_CTRL_HEADER_FORMAT;

typedef struct
{
    union
    {
        uint64_t version;
        struct
        {
            uint64_t _version_reserve: 32;   //!< reserved
            uint64_t _version_revision: 16; //!< revision version
            uint64_t _version_minor: 8;     //!< minor version
            uint64_t _version_major: 8;     //!< major version
        } sub_version;
    };
    uint32_t _version_commitid;     //!< git commit id
    uint8_t _customer_name[8];      //!< branch name for customer patch
    uint8_t version_reserved[4];
} __attribute__((packed)) T_VERSION_FORMAT;

typedef struct _AUTH_HEADER_FORMAT
{
    uint8_t img_string[6];// "IMGHDR"
    uint16_t auth_length;
    uint8_t auth_type;
    uint8_t auth_reserved[7]; //for cmac 16bytes align
    uint8_t cmac[CMAC_BYTE_SIZE];
    uint8_t image_signature[ECDSA_SIGNATURE_BYTE_SIZE];
    uint8_t image_hash[SHA256_BYTE_SIZE];
    PUBLIC_KEY PubKey;
    uint8_t auth_reserved1[3];
} T_AUTH_HEADER_FORMAT; // 196 bytes

typedef union
{
    uint8_t bytes[12];
    struct
    {
        uint32_t app_ram_data_addr;
        uint32_t app_ram_data_size;
        uint32_t app_heap_data_on_size;
    } app_ram_info;
} T_EXTRA_INFO_FORMAT;

typedef struct
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t enc: 1;
            uint32_t enc_type: 3; // 1: normal AES 2: on the fly
            uint32_t mode: 4; // gcm mode:0b00, ctr mode:0b01, mix mode:0b10
            uint32_t key_select: 4;
            uint32_t is_encrypted: 1;
            uint32_t rsvd: 19;
        };
    } ctrl_flag;
    uint32_t encryption_load_addr; // load base  nor: flash address nand/sd/emmc: psram
    uint32_t encryption_size;  // load size
    uint32_t encryption_exe_addr;  // exe base  nor: ram address nand/sd/emmc: psram
    uint8_t dec_key[16];
    union
    {
        uint8_t iv[16];
        struct
        {
            uint8_t  iv_high[4];
            uint8_t  iv_low[4];
            uint8_t  iv_reserved[8];
        };
    };

} T_ENCRYPT_FORMAT; //48bytes

typedef struct
{
    uint32_t  image_load_base;
    uint32_t  image_load_size;
    uint32_t  image_exe_base;
    uint32_t  image_exe_size;
} T_IMAGE_INFO;

typedef union _IMG_HEADER_FORMAT
{
    uint8_t bytes[DEFAULT_HEADER_SIZE];
    struct
    {
        T_AUTH_HEADER_FORMAT auth;   //196 bytes
        T_IMG_CTRL_HEADER_FORMAT ctrl_header; //16 bytes
        T_ENCRYPT_FORMAT encrypt_header; //48bytes
        uint8_t uuid[16]; //16bytes
        uint32_t magic_pattern; //4 bytes
        T_VERSION_FORMAT git_ver;  //24bytes
        uint8_t  common_reserved[64]; //64 bytes
        union //656bytes
        {
            struct  // data 8bytes
            {
                uint16_t reserved[1];
                uint16_t tool_version;
                uint32_t timestamp;
                uint8_t  data_reserved[648];
            };

            struct // image_header
            {
                uint32_t exe_entry;
                uint32_t image_base; //image begin: nor:flash address   nand/sd/emmc: psram address
                uint32_t ram_load_src; // ram code load base
                uint32_t ram_load_len; // ram code load size
                uint32_t ram_load_dst; // ram code exe base
                union
                {
                    uint8_t  exe_image_reserved[636]; //T_EXTRA_INFO_FORMAT ex_info; //uint8_t comiple_stamp[16];
                    struct // boot patch image_header 148
                    {
                        uint32_t resv_golden_pattern[GOLDEN_PATTERN_WORD_LEN];
                        uint32_t pre_image_num;
                        T_IMAGE_INFO ext_image_excute_info[(PRE_IMG_MAX - PRE_IMAGE_FIRST)]; //3x4
                        uint8_t boot_patch_header_reserved[504];
                    };
                    struct
                    {
                        T_EXTRA_INFO_FORMAT ex_info;
                        uint8_t app_header_reserved[624];
                    };
                };
            };
            struct   // ota header 264
            {
                uint32_t layout_size; // K
                uint32_t ota_image_num;
                T_IMAGE_INFO image_excute_info[(IMG_MAX - IMG_BANK_FIRST)];  //11x4
                uint8_t  ota_header_reserved[452];
            };
        };
    };
}  __attribute__((packed)) T_IMG_HEADER_FORMAT;


/*************************************************************************************************
*                          functions
*************************************************************************************************/
//static __inline uint32_t *get_image_addr_in_bank(uint32_t ota_addr, IMG_ID image_id)
//{
//    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
//    return &ota->image_excute_info[(image_id - IMG_BANK_FIRST)].image_load_base;
//}

//static __inline uint32_t *get_image_size_in_bank(uint32_t ota_addr, IMG_ID image_id)
//{
//    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
//    return &ota->image_excute_info[(image_id - IMG_BANK_FIRST)].image_load_size;
//}

static __inline uint32_t *get_pre_image_addr_out_of_bank(uint32_t bootpatch_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *bootpatch = (T_IMG_HEADER_FORMAT *)bootpatch_addr;
    return &bootpatch->ext_image_excute_info[(image_id - PRE_IMAGE_FIRST)].image_load_base;
}

static __inline uint32_t *get_pre_image_size_out_of_bank(uint32_t bootpatch_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *bootpatch = (T_IMG_HEADER_FORMAT *)bootpatch_addr;
    return &bootpatch->ext_image_excute_info[(image_id - PRE_IMAGE_FIRST)].image_load_size;
}

static __inline uint32_t *get_image_load_addr_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_excute_info[image_id - IMG_BANK_FIRST].image_load_base;
}

static __inline uint32_t *get_image_load_size_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_excute_info[image_id - IMG_BANK_FIRST].image_load_size;
}

static __inline uint32_t *get_image_exe_addr_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_excute_info[(image_id - IMG_BANK_FIRST)].image_exe_base;
}

static __inline uint32_t *get_image_exe_size_in_bank(uint32_t ota_addr, IMG_ID image_id)
{
    T_IMG_HEADER_FORMAT *ota = (T_IMG_HEADER_FORMAT *)ota_addr;
    return &ota->image_excute_info[(image_id - IMG_BANK_FIRST)].image_exe_size;
}

bool is_ota_support_bank_switch(void);

uint32_t get_image_header_format_size(void);

uint32_t get_active_ota_bank_addr(void);

void set_active_ota_bank_addr(uint32_t ota_addr);

uint32_t get_active_bank_image_addr_by_img_id(IMG_ID image_id);

uint32_t get_active_bank_image_size_by_img_id(IMG_ID image_id);

uint32_t get_temp_bank_image_addr_by_img_id(IMG_ID image_id);

uint32_t get_temp_bank_image_size_by_img_id(IMG_ID image_id);

uint32_t get_ota_bank_image_version(IMG_ID image_id, bool is_active_bank);

T_EXTRA_INFO_FORMAT *get_extra_info(void);

#endif // _IMAGE_INFO_H_
