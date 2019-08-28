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
 * \brief LPC84X I2C1 �û������ļ�
 * \sa am_hwconf_lpc84x_i2c1.c
 *
 * \internal
 * \par Modification history
 * - 1.00 15-07-13  win, first implementation
 * \endinternal
 */

#include <am_lpc84x.h>
#include "ametal.h"
#include "am_lpc_i2c.h"
#include "hw/amhw_lpc84x_clk.h"

/**
 * \addtogroup am_if_src_hwconf_lpc84x_i2c1
 * \copydoc am_hwconf_lpc84x_i2c1.c
 * @{
 */

/**
 * \brief I2C1 ƽ̨��ʼ������
 */
am_local void __lpc84x_i2c1_plfm_init (void)
{
    am_gpio_pin_cfg(__I2C1_SCL_PIN, PIO_FUNC_I2C1_SCL | AM_GPIO_OPEN_DRAIN);
    am_gpio_pin_cfg(__I2C1_SDA_PIN, PIO_FUNC_I2C1_SDA | AM_GPIO_OPEN_DRAIN);

    amhw_lpc84x_clk_i2c1_clk_sel_set (AMHW_LPC84X_CLK_DEVICE_FRG0CLK);
    amhw_lpc84x_clk_periph_enable(AMHW_LPC84X_CLK_I2C1);
    amhw_lpc84x_syscon_periph_reset(AMHW_LPC84X_RESET_I2C1);
}

/**
 * \brief I2C1 ƽ̨���ʼ������
 */
am_local void __lpc84x_i2c1_plfm_deinit (void)
{
    amhw_lpc84x_syscon_periph_reset(AMHW_LPC84X_RESET_I2C1);
    amhw_lpc84x_clk_periph_disable(AMHW_LPC84X_CLK_I2C1);
}

/** \brief I2C1 �������ʲ������� */
#define __BUS_SPEED_I2C1     (400000)

/** \brief I2C1 �豸��Ϣ */
am_local am_const am_lpc_i2c_devinfo_t __g_lpc84x_i2c1_devinfo = {
    __BUS_SPEED_I2C1,           /* I2C1 �������� */
    LPC84X_I2C1_BASE,           /* I2C1 �Ĵ������ַ */
    INUM_I2C1,                  /* I2C1 �жϺ� */
    CLK_I2C1,                   /* I2C1 ʱ�Ӻ� */
    10,                         /* ��ʱʱ�� */
    __lpc84x_i2c1_plfm_init,    /* ƽ̨��ʼ������ */
    __lpc84x_i2c1_plfm_deinit   /* ƽ̨���ʼ������ */
};

/** \brief I2C1 �豸ʵ�� */
am_local am_lpc_i2c_dev_t __g_lpc84x_i2c1_dev;

/**
 * \brief I2C1 ʵ����ʼ��
 */
am_i2c_handle_t am_lpc84x_i2c1_inst_init (void)
{
    return am_lpc_i2c_init(&__g_lpc84x_i2c1_dev, &__g_lpc84x_i2c1_devinfo);
}

/**
 * \brief I2C1 ʵ�����ʼ��
 */
void am_lpc84x_i2c1_inst_deinit (am_i2c_handle_t handle)
{
    am_lpc_i2c_deinit(handle);
}

/**
 * @}
 */

/* end of file */