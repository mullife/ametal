/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief ZLG237 bootloader 双区用户配置文件
 *
 * \internal
 * \par Modification history
 * - 1.00 18-12-13  yrh, first implementation
 * \endinternal
 */

/**
 * \addtogroup am_if_src_bootconf_zlg237_double
 * \copydoc am_bootconf_zlg237_double.c
 * @{
 */

#include "am_zlg237.h"
#include "am_zlg237_inst_init.h"
#include "am_zlg237_boot_flash.h"
#include "am_arm_boot.h"
#include "am_boot_flash.h"
#include "am_boot_mem_flash.h"
#include "am_zlg237_flash.h"
#include "zlg237_regbase.h"
#include "am_boot_firmware_flash.h"
#include "am_int.h"
#include "am_zlg237_clk.h"
#include "amhw_zlg237_rcc.h"
#include "am_arm_nvic.h"
#include "am_bootconf_zlg237.h"
#include "am_boot_msg_flag.h"

static am_boot_flash_handle_t flash_handle;
static am_boot_mem_handle_t   mem_handle;
/*******************************************************************************
 * flash配置
 ******************************************************************************/
/**
 *  \brief bootloader flash 设备信息
 */
static am_zlg237_boot_flash_devinfo_t __g_flash_devinfo = {

    {
        /** \brief flash的起始地址 */
        0x08000000,
        /** \brief flash的总的大小 */
        128 * 1024,
        /** \brief flash扇区大小 */
        1024 * 4,
        /** \brief flash页大小 */
        1024,
    },
    /** \brief flash寄存器的基地址 */
    ZLG237_FLASH_BASE,
    /** \brief 平台初始化函数 */
    NULL,
    /** \brief 平台初解始化函数 */
    NULL,
};

/**
 * < \brief bootloader flash 设备实例
 */
static am_zlg237_boot_flash_dev_t __g_flash_dev;

/**
 * \brief bootloader flash实例初始化，获得其标准服务句柄
 *
 * \param 无
 *
 * \return bootloader flash标准服务句柄，若为 NULL，表明初始化失败
 */
static am_boot_flash_handle_t am_zlg237_boot_double_flash_inst_init(void)
{
    return am_zlg237_boot_flash_init(&__g_flash_dev, &__g_flash_devinfo);
}

/*******************************************************************************
 * 固件以flash的方式存储配置
 ******************************************************************************/
static am_boot_firmware_flash_devinfo_t __g_firmware_flash_devinfo = {
    /** \brief 固件在内存中存放的起始地址 */
    0x08007800,
    /**< \brief 固件存放区的总大小 */
    49 * 1024,
    /** \brief 驱动存放固件缓冲区大小 , 值为内存写入最小大小的整数倍 */
    4,
    /** \brief 平台初始化函数 */
    NULL,
    /** \brief 平台解初始化函数 */
    NULL,
};

static am_boot_firmware_flash_dev_t __g_firmware_flash_dev;

am_boot_firmware_handle_t am_zlg237_boot_double_firmware_flash_inst_init()
{
    flash_handle = am_zlg237_boot_double_flash_inst_init();
    return am_boot_firmware_flash_init (&__g_firmware_flash_dev,
                                        &__g_firmware_flash_devinfo,
                                         flash_handle);
}

/*******************************************************************************
 * 以flag方式的消息配置
 ******************************************************************************/
static am_boot_msg_flag_devinfo_t __g_boot_msg_flag_info = {
    /** \brief 备份区起始地址 */
    0x08013C00,
    /** \brief 应用区大小 */
    49 * 1024,
    /** \brief 标志存放地址 */
    0x08007400,
    /** \brief 标志占用的实际大小，一般为内存最小的写入单元，如果使用flash内存，则为最小的擦除单元*/
    4,
    /** \brief 固件拷贝，主要从固件的备份区，拷贝到应用程序的执行区(如果标志，应用程序区，备份区都在同一个内存设备中，可以不用实现该函数)*/
    NULL,
};

am_boot_msg_handle_t am_zlg237_boot_msg_flag_inst_init()
{
    mem_handle = am_boot_mem_flash_init(flash_handle);

    return am_boot_msg_flag_init(&__g_boot_msg_flag_info,
                                  mem_handle);
}

/*******************************************************************************
 * 标准bootloader用户配置
 ******************************************************************************/

/* 在整个bootloader中用到的资源都要解初始化并禁能，常见的如定时器，串口等 */
static void __zlg237_boot_plfm_deinit()
{
    volatile uint32_t i = 2000000;
    /* 串口解初始化 ，解初始化前需要小段的延时，保证寄存器任务已经完成*/
    while(i--){

    }
    amhw_zlg237_usart_disable((amhw_zlg237_usart_t *)ZLG237_USART1_BASE);
//    am_int_disable(INUM_USART1);

    am_gpio_pin_cfg(PIOA_9, AM_GPIO_INPUT);
    am_gpio_pin_cfg(PIOA_10,AM_GPIO_INPUT);

//    am_clk_disable(CLK_USART1);

    /* 定时器解初始化  */
    amhw_zlg_tim_int_disable(ZLG237_TIM4, AMHW_ZLG_TIM_UIE);
    amhw_zlg_tim_disable(ZLG237_TIM4);
    am_int_disable(INUM_TIM4);
    am_zlg237_clk_reset(CLK_TIM4);
    am_clk_disable(CLK_TIM4);

    ZLG237_RCC->ahbenr = 0;
    ZLG237_RCC->apb1enr = 0;
    ZLG237_RCC->apb2enr = 0;

    amhw_zlg237_rcc_sys_clk_set(AMHW_ZLG237_SYSCLK_HSE);

    am_arm_nvic_deinit();
    __disable_irq();
}

/**
 * < \brief bootloader标准设备信息
 */
static am_arm_boot_devinfo_t __g_zlg237_boot_double_devinfo = {
    /**< \brief 应用代码区起始地址*/
    0x08007800,
    /** \brief ram起始地址 */
    0x20000000,
    /** \brief ram大小 */
    20 * 1024,
    /** \brief 平台初始化函数 */
    NULL,
    /** \brief 平台解初始化函数 */
    __zlg237_boot_plfm_deinit,
};

int am_zlg237_boot_double_inst_init()
{
    return am_arm_boot_init(&__g_zlg237_boot_double_devinfo);
}

/** @} */

/* end of file */
