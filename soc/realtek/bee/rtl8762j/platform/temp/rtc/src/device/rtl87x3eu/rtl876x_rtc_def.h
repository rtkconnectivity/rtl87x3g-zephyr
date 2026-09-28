/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_rtc_def.h
* \brief    RTC Registers Structures Section
* \details
* \author   colin
* \date     2024-06-18
* \version  v1.0
* *********************************************************************************************************
*/

#ifndef RTL_RTC_DEF_H
#define RTL_RTC_DEF_H

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"
#include "indirect_access.h"
#include "rtl876x_rtc_def.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */


/*============================================================================*
 *                         RTC Features
 *============================================================================*/

#define RTC_SUPPORT_CLOCK_IN_FROM_OUTSIDE_PAD          (0)
#define RTC_SUPPORT_CLOCK_OUT_TO_OUTSIDE_PAD           (0)
#define RTC_SUPPORT_WAKEUP_MORE_SOURCE                 (1) /*Support TICK OVERFLOW PRECOMP PRECOMP_COMP3*/
#define RTC_SUPPORT_WAKEUP_COMPARE_GUARDTIME           (0)
#define RTC_SUPPORT_COMPARE_AUTO_RELOAD                (1)
#define RTC_SUPPORT_RAP_FUNCTION                       (0)
#define RTC_SUPPORT_AUTO_CLOCK                         (0)
#define RTC_SUPPORT_RESET_REGISTER_TO_DEFAULT          (0)

/*============================================================================*
 *                         RTC Registers Memory Map
 *============================================================================*/
typedef struct
{
    __IO uint32_t RTC_CR0;                  /*!< 0x00 */
    __IO uint32_t RTC_INT_CLEAR;            /*!< 0x04 */
    __IO uint32_t RTC_INT_SR;               /*!< 0x08 */
    __IO uint32_t RTC_PRESCALER0;           /*!< 0x0C */
    __IO uint32_t RTC_COMP_0;               /*!< 0x10 */
    __IO uint32_t RTC_COMP_1;               /*!< 0x14 */
    __IO uint32_t RTC_COMP_2;               /*!< 0x18 */
    __IO uint32_t RTC_COMP_3;               /*!< 0x1C */
    __IO uint32_t RTC_COMP_0_RELOAD;        /*!< 0x20 */
    __IO uint32_t RTC_COMP_1_RELOAD;        /*!< 0x24 */
    __IO uint32_t RTC_COMP_2_RELOAD;        /*!< 0x28 */
    __IO uint32_t RTC_COMP_3_RELOAD;        /*!< 0x2C */
    __I  uint32_t RTC_CNT0;                 /*!< 0x30 */
    __IO uint32_t RTC_PRESCALE_CNT0;        /*!< 0x34 */
    __IO uint32_t RTC_PRESCALE_CMP0;        /*!< 0x38 */
    __IO uint32_t RTC_BACKUP_REG;           /*!< 0x3C */
} RTC_TypeDef;

/*============================================================================*
 *                         RTC Declaration
 *============================================================================*/
#define RTC_REG_BASE                0x40000100UL
#define RTC                         ((RTC_TypeDef *) RTC_REG_BASE)

/*============================================================================*
 *                         RTC Registers and Field Descriptions
 *============================================================================*/
/* 0x00
   0       R/W    rtc_cnt_start       1'h0
   1       R/W    rtc_cnt_rst         1'h0
   2       R/W    rtc_pre_cnt_rst     1'h0
   7:3     R      reserver000_7_3     5'h0
   8       R/W    rtc_tick_ie         1'h0
   9       R/W    rtc_cnt_ov_ie       1'h0
   10      R/W    rtc_precmp_ie       1'h0
   11      R/W    rtc_precmp_cmp3_ie  1'h0
   15:12   R      reserver000_15_12   4'h0
   16      R/W    rtc_cmp0_nv_ie      1'h0
   17      R/W    rtc_cmp1_nv_ie      1'h0
   18      R/W    rtc_cmp2_nv_ie      1'h0
   19      R/W    rtc_cmp3_nv_ie      1'h0
   20      R/W    rtc_cmp0_wk_ie      1'h0
   21      R/W    rtc_cmp1_wk_ie      1'h0
   22      R/W    rtc_cmp2_wk_ie      1'h0
   23      R/W    rtc_cmp3_wk_ie      1'h0
   24      R/W    rtc_cmp0_reload_en  1'h0
   25      R/W    rtc_cmp1_reload_en  1'h0
   26      R/W    rtc_cmp2_reload_en  1'h0
   27      R/W    rtc_cmp3_reload_en  1'h0
   28      R      reserver000_28      1'h0
   29      R/W    rtc_wk_ie           1'h0
   30      R/W    rtc_nv_ie           1'h0
   31      R/W    rtc_rst             1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_cnt_start: 1;
        uint32_t rtc_cnt_rst: 1;
        uint32_t rtc_pre_cnt_rst: 1;
        const uint32_t reserver000_7_3: 5;
        uint32_t rtc_tick_ie: 1;
        uint32_t rtc_cnt_ov_ie: 1;
        uint32_t rtc_precmp_ie: 1;
        uint32_t rtc_precmp_cmp3_ie: 1;
        const uint32_t reserver000_15_12: 4;
        uint32_t rtc_cmp0_nv_ie: 1;
        uint32_t rtc_cmp1_nv_ie: 1;
        uint32_t rtc_cmp2_nv_ie: 1;
        uint32_t rtc_cmp3_nv_ie: 1;
        uint32_t rtc_cmp0_wk_ie: 1;
        uint32_t rtc_cmp1_wk_ie: 1;
        uint32_t rtc_cmp2_wk_ie: 1;
        uint32_t rtc_cmp3_wk_ie: 1;
        uint32_t rtc_cmp0_reload_en: 1;
        uint32_t rtc_cmp1_reload_en: 1;
        uint32_t rtc_cmp2_reload_en: 1;
        uint32_t rtc_cmp3_reload_en: 1;
        const uint32_t reserver000_28: 1;
        uint32_t rtc_wk_ie: 1;
        uint32_t rtc_nv_ie: 1;
        uint32_t rtc_rst: 1;
    } b;
} RTC_CR0_TypeDef;


/* 0x04
   0       WO     rtc_tick_clr        1'h0
   1       WO     rtc_cnt_ov_clr      1'h0
   2       WO     rtc_precmp_clr      1'h0
   3       WO     rtc_precmp_cmp3_clr 1'h0
   7:4     R      reserver004_7_4     4'h0
   8       WO     rtc_cmp0_nv_clr     1'h0
   9       WO     rtc_cmp1_nv_clr     1'h0
   10      WO     rtc_cmp2_nv_clr     1'h0
   11      WO     rtc_cmp3_nv_clr     1'h0
   12      WO     rtc_cmp0_wk_clr     1'h0
   13      WO     rtc_cmp1_wk_clr     1'h0
   14      WO     rtc_cmp2_wk_clr     1'h0
   15      WO     rtc_cmp3_wk_clr     1'h0
   31:16   R      reserver004_31_16   16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_tick_clr: 1;
        uint32_t rtc_cnt_ov_clr: 1;
        uint32_t rtc_precmp_clr: 1;
        uint32_t rtc_precmp_cmp3_clr: 1;
        const uint32_t reserver004_7_4: 4;
        uint32_t rtc_cmp0_nv_clr: 1;
        uint32_t rtc_cmp1_nv_clr: 1;
        uint32_t rtc_cmp2_nv_clr: 1;
        uint32_t rtc_cmp3_nv_clr: 1;
        uint32_t rtc_cmp0_wk_clr: 1;
        uint32_t rtc_cmp1_wk_clr: 1;
        uint32_t rtc_cmp2_wk_clr: 1;
        uint32_t rtc_cmp3_wk_clr: 1;
        const uint32_t reserver004_31_16: 16;
    } b;
} RTC_INT_CLEAR_TypeDef;


/* 0x08
   0       R      rtc_tick_sr         1'h0
   1       R      rtc_cnt_ov_sr       1'h0
   2       R      rtc_precmp_sr       1'h0
   3       R      rtc_precmp_cmp3_sr  1'h0
   7:4     R      reserver008_7_4     4'h0
   8       R      rtc_cmp0_nv_sr      1'h0
   9       R      rtc_cmp1_nv_sr      1'h0
   10      R      rtc_cmp2_nv_sr      1'h0
   11      R      rtc_cmp3_nv_sr      1'h0
   12      R      rtc_cmp0_wk_sr      1'h0
   13      R      rtc_cmp1_wk_sr      1'h0
   14      R      rtc_cmp2_wk_sr      1'h0
   15      R      rtc_cmp3_wk_sr      1'h0
   31:16   R      reserver008_31_16   16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t rtc_tick_sr: 1;
        const uint32_t rtc_cnt_ov_sr: 1;
        const uint32_t rtc_precmp_sr: 1;
        const uint32_t rtc_precmp_cmp3_sr: 1;
        const uint32_t reserver008_7_4: 4;
        const uint32_t rtc_cmp0_nv_sr: 1;
        const uint32_t rtc_cmp1_nv_sr: 1;
        const uint32_t rtc_cmp2_nv_sr: 1;
        const uint32_t rtc_cmp3_nv_sr: 1;
        const uint32_t rtc_cmp0_wk_sr: 1;
        const uint32_t rtc_cmp1_wk_sr: 1;
        const uint32_t rtc_cmp2_wk_sr: 1;
        const uint32_t rtc_cmp3_wk_sr: 1;
        const uint32_t reserver008_31_16: 16;
    } b;
} RTC_INT_SR_TypeDef;


/* 0x0C
   11:0    R/W    rtc_prescaler       12'h1
   31:12   R      reserver00c_31_12   20'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_prescaler: 12;
        const uint32_t reserver00c_31_12: 20;
    } b;
} RTC_PRESCALER0_TypeDef;

/* 0x10
   31:0    R/W    rtc_cmp0            32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_cmp0: 32;
    } b;
} RTC_COMP_0_TypeDef;

/* 0x14
   31:0    R/W    rtc_cmp1            32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_cmp1: 32;
    } b;
} RTC_COMP_1_TypeDef;

/* 0x18
   31:0    R/W    rtc_cmp2            32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_cmp2: 32;
    } b;
} RTC_COMP_2_TypeDef;

/* 0x1C
   31:0    R/W    rtc_cmp3            32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_cmp3: 32;
    } b;
} RTC_COMP_3_TypeDef;

/* 0x20
   31:0    R/W    rtc_cmp0_reload     32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_cmp0_reload: 32;
    } b;
} RTC_COMP_0_RELOAD_TypeDef;

/* 0x24
   31:0    R/W    rtc_cmp1_reload     32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_cmp1_reload: 32;
    } b;
} RTC_COMP_1_RELOAD_TypeDef;


/* 0x28
   31:0    R/W    rtc_cmp2_reload     32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_cmp2_reload: 32;
    } b;
} RTC_COMP_2_RELOAD_TypeDef;


/* 0x2C
   31:0    R/W    rtc_cmp3_reload     32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_cmp3_reload: 32;
    } b;
} RTC_COMP_3_RELOAD_TypeDef;

/* 0x30
   31:0    R      rtc_cnt             32'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t rtc_cnt: 32;
    } b;
} RTC_CNT0_TypeDef;

/* 0x34
   11:0    R      rtc_prescale_cnt    12'h0
   31:12   R      reserver034_31_12   20'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t rtc_prescale_cnt: 12;
        const uint32_t reserver034_31_12: 20;
    } b;
} RTC_PRESCALE_CNT0_TypeDef;


/* 0x38
   11:0    R/W    rtc_prescale_cmp    12'h0
   31:12   R      reserver038_31_12   20'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_prescale_cmp: 12;
        const uint32_t reserver038_31_12: 20;
    } b;
} RTC_PRESCALE_CMP0_TypeDef;


/* 0x3C
   31:0    R/W    rtc_backup          32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t rtc_backup: 32;
    } b;
} RTC_BACKUP_REG_TypeDef;

/*============================================================================*
 *                         RTC Wrappers
 *============================================================================*/

/**
 * \brief    RTC Macro Wrapper
 *
 * \ingroup  RTC_Exported_Constants
 */
#define COMP0_INDEX             RTC_COMP0
#define COMP1_INDEX             RTC_COMP1
#define COMP2_INDEX             RTC_COMP2
#define COMP3_INDEX             RTC_COMP3
#define RTC_PRECMP_INT          RTC_INT_PRE_COMP
#define RTC_PRECMP_CMP3_INT     RTC_INT_PRE_COMP3
#define RTC_CMP0_NV_INT         RTC_INT_COMP0
#define RTC_CMP1_NV_INT         RTC_INT_COMP1
#define RTC_CMP2_NV_INT         RTC_INT_COMP2
#define RTC_CMP3_NV_INT         RTC_INT_COMP3
#define RTC_CMP0_WK_INT         RTC_WK_COMP0
#define RTC_CMP1_WK_INT         RTC_WK_COMP1
#define RTC_CMP2_WK_INT         RTC_WK_COMP2
#define RTC_CMP3_WK_INT         RTC_WK_COMP3
#define RTC_CMP0_NV_CLR         RTC_COMP0_CLR
#define RTC_CMP1_NV_CLR         RTC_COMP1_CLR
#define RTC_CMP2_NV_CLR         RTC_COMP2_CLR
#define RTC_CMP3_NV_CLR         RTC_COMP3_CLR
#define RTC_PRECMP_CLR          RTC_PRE_COMP_CLR
#define RTC_PRECMP_CMP3_CLR     RTC_PRE_COMP3_CLR
#define RTC_CNT_OV_CLR          RTC_OVERFLOW_CLR
#define RTC_CMP0_WK_CLR         RTC_COMP0_WK_CLR
#define RTC_CMP1_WK_CLR         RTC_COMP1_WK_CLR
#define RTC_CMP2_WK_CLR         RTC_COMP2_WK_CLR
#define RTC_CMP3_WK_CLR         RTC_COMP3_WK_CLR

#define REG_OFFSET(type,v)      (&(((type *)0)->v))
#define RTC_REG_OFFSET(MEMBER)  REG_OFFSET(RTC_TypeDef, MEMBER)
#define RTC_WRITE(reg, value)   aon_indirect_write_reg_safe((uint32_t)RTC_REG_OFFSET(reg), value)
#define RTC_WRITE_DELAY(reg, value)   do {\
        aon_indirect_write_reg_safe((uint32_t)RTC_REG_OFFSET(reg), value);\
        __NOP();\
        __NOP();\
        aon_indirect_write_reg_safe((uint32_t)RTC_REG_OFFSET(reg), 0);\
    } while (0)

/**
 * \brief    RTC API Wrapper
 *
 * \ingroup  RTC_Exported_Constants
 */
#define RTC_RunCmd              RTC_Cmd
#define RTC_INTEnableConfig     RTC_INTConfig
#define RTC_CpuNVICEnable       RTC_NvCmd
#define RTC_ClearINTStatus      RTC_ClearINTPendingBit
#define RTC_ClearCompWkINT      RTC_ClearWakeupStatusBit
#define RTC_SetComp             RTC_SetCompValue
#define RTC_GetComp             RTC_GetCompValue

#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* RTL_RTC_DEF_H */
