/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef OT_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "ot_i2c.h"
#endif

#include "sc500ai_cmos.h"

static int g_fd[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = -1};

int sc500ai_i2c_init(ot_vi_pipe vi_pipe)
{
    int ret;
    char dev_file[I2C_DEV_FILE_NUM] = {0};
    td_u8 dev_num;

    if (g_fd[vi_pipe] >= 0) {
        return TD_SUCCESS;
    }
#ifdef OT_GPIO_I2C
    g_fd[vi_pipe] = open("/dev/gpioi2c_ex", O_RDONLY, S_IRUSR);
    if (g_fd[vi_pipe] < 0) {
        isp_err_trace("Open gpioi2c_ex error!\n");
        return TD_FAILURE;
    }
#else
    ot_isp_sns_commbus *bus_info = TD_NULL;
    bus_info = sc500ai_get_bus_info(vi_pipe);
    dev_num = bus_info->i2c_dev;
    (td_void)snprintf_s(dev_file, sizeof(dev_file), sizeof(dev_file) - 1, "/dev/i2c-%u", dev_num);

    g_fd[vi_pipe] = open(dev_file, O_RDWR, S_IRUSR | S_IWUSR);
    if (g_fd[vi_pipe] < 0) {
        isp_err_trace("Open /dev/ot_i2c_drv-%u error!\n", dev_num);
        return TD_FAILURE;
    }

    ret = ioctl(g_fd[vi_pipe], OT_I2C_SLAVE_FORCE, (SC500AI_I2C_ADDR >> 1));
    if (ret < 0) {
        isp_err_trace("I2C_SLAVE_FORCE error!\n");
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return ret;
    }
#endif

    return TD_SUCCESS;
}

int sc500ai_i2c_exit(ot_vi_pipe vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 sc500ai_read_register(ot_vi_pipe vi_pipe, td_u32 addr)
{
    ot_unused(vi_pipe);
    ot_unused(addr);
    return TD_SUCCESS;
}

td_s32 sc500ai_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data)
{
    if (g_fd[vi_pipe] < 0) {
        return TD_SUCCESS;
    }
    td_s32 ret;

#ifdef OT_GPIO_I2C
    i2c_data.dev_addr = SC500AI_I2C_ADDR;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = SC500AI_ADDR_BYTE;
    i2c_data.data = data;
    i2c_data.data_byte_num = SC500AI_DATA_BYTE;

    ret = ioctl(g_fd[vi_pipe], GPIO_I2C_WRITE, &i2c_data);
    if (ret) {
        isp_err_trace("GPIO-I2C write failed!\n");
        return ret;
    }
#else
    td_u32 idx = 0;
    td_u8 buf[I2C_BUF_NUM];

    if (SC500AI_ADDR_BYTE == 2) {  /* 2 byte */
        buf[idx] = (addr >> 8) & 0xff;  /* shift 8 */
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    } else {
        buf[idx] = addr & 0xff;
        idx++;
    }

    if (SC500AI_DATA_BYTE == 2) {  /* 2 byte */
        buf[idx] = (data >> 8) & 0xff;  /* shift 8 */
        idx++;
        buf[idx] = data & 0xff;
        idx++;
    } else {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[vi_pipe], buf, SC500AI_ADDR_BYTE + SC500AI_DATA_BYTE);
    if (ret < 0) {
        isp_err_trace("I2C_WRITE error!\n");
        return TD_FAILURE;
    }

#endif
    return TD_SUCCESS;
}

static void delay_ms(int ms)
{
    usleep(ms * 1000); /* 1ms: 1000us */
    return;
}

void sc500ai_prog(ot_vi_pipe vi_pipe, const td_u32 *rom)
{
    ot_unused(vi_pipe);
    ot_unused(rom);
    return;
}

void sc500ai_standby(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return;
}

void sc500ai_restart(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return;
}

static td_s32 sc500ai_linear_5m30_10bit_init(ot_vi_pipe vi_pipe);
static td_s32 sc500ai_linear_800p60_10bit_init(ot_vi_pipe vi_pipe);
static td_s32 sc500ai_linear_1080p60_10bit_init(ot_vi_pipe vi_pipe);
static td_s32 sc500ai_vc_wdr_2t1_5m30_10bit_init(ot_vi_pipe vi_pipe);

static void sc500ai_default_reg_init(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_s32 ret = TD_SUCCESS;
    ot_isp_sns_state *past_sensor = TD_NULL;

    past_sensor = sc500ai_get_ctx(vi_pipe);
    for (i = 0; i < past_sensor->regs_info[0].reg_num; i++) {
        ret += sc500ai_write_register(vi_pipe,
            past_sensor->regs_info[0].i2c_data[i].reg_addr,
            past_sensor->regs_info[0].i2c_data[i].data);
    }

    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }
    return;
}

void sc500ai_init(ot_vi_pipe vi_pipe)
{
    ot_wdr_mode      wdr_mode;
    td_u8            img_mode;
    td_bool          init;
    td_s32           ret;
    ot_isp_sns_state *past_sensor = TD_NULL;

    past_sensor = sc500ai_get_ctx(vi_pipe);
    init        = past_sensor->init;
    wdr_mode    = past_sensor->wdr_mode;
    img_mode    = past_sensor->img_mode;

    ret = sc500ai_i2c_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("i2c init failed!\n");
        return;
    }

    /* When sensor first init, config all registers */
    if (init == TD_FALSE) {
        if (OT_WDR_MODE_2To1_LINE == wdr_mode) {
            if (SC500AI_5M_30FPS_10BIT_2TO1_VC_MODE == img_mode) {
                sc500ai_vc_wdr_2t1_5m30_10bit_init(vi_pipe);
            } else {
            }
        } else {
            if (SC500AI_5M_30FPS_10BIT_LINEAR_MODE == img_mode)
                sc500ai_linear_5m30_10bit_init(vi_pipe);
            else if (SC500AI_1080P_100FPS_10BIT_LINEAR_MODE == img_mode)
                sc500ai_linear_1080p60_10bit_init(vi_pipe);
            else
                sc500ai_linear_800p60_10bit_init(vi_pipe);
        }
    } else {
        if (OT_WDR_MODE_2To1_LINE == wdr_mode) {
            if (SC500AI_5M_30FPS_10BIT_2TO1_VC_MODE == img_mode) {
                sc500ai_vc_wdr_2t1_5m30_10bit_init(vi_pipe);
            } else {
            }
        } else {
            if (SC500AI_5M_30FPS_10BIT_LINEAR_MODE == img_mode)
                sc500ai_linear_5m30_10bit_init(vi_pipe);
            else if (SC500AI_1080P_100FPS_10BIT_LINEAR_MODE == img_mode)
                sc500ai_linear_1080p60_10bit_init(vi_pipe);
            else
                sc500ai_linear_800p60_10bit_init(vi_pipe);
        }
    }

    past_sensor->init = TD_TRUE;
    return;
}

void sc500ai_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    ret = sc500ai_i2c_exit(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("SC500AI exit failed!\n");
    }

    return;
}

static td_s32 sc500ai_linear_5m30_10bit_init_part1(ot_vi_pipe vi_pipe)
{
    // cleaned_0x01_FT_SC500AI_27MInput_MIPI_4lane_396Mbps_10bit_2880x1620_30fps.ini
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x0103, 0x01);
    delay_ms(100); /* 100ms */
    ret += sc500ai_write_register(vi_pipe, 0x0100, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x36e9, 0x80);
    ret += sc500ai_write_register(vi_pipe, 0x36f9, 0x80);
    ret += sc500ai_write_register(vi_pipe, 0x301f, 0x01);
    ret += sc500ai_write_register(vi_pipe, 0x3253, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3301, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3302, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3303, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x3304, 0x60);
    ret += sc500ai_write_register(vi_pipe, 0x3306, 0x60);
    ret += sc500ai_write_register(vi_pipe, 0x3308, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x3309, 0x70);
    ret += sc500ai_write_register(vi_pipe, 0x330a, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x330b, 0xf0);
    ret += sc500ai_write_register(vi_pipe, 0x330d, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x330e, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x330f, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x3310, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x331c, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x331e, 0x51);
    ret += sc500ai_write_register(vi_pipe, 0x331f, 0x61);
    return ret;
}

static td_s32 sc500ai_linear_5m30_10bit_init_part2(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x3320, 0x09);
    ret += sc500ai_write_register(vi_pipe, 0x3333, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x334c, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x3356, 0x09);
    ret += sc500ai_write_register(vi_pipe, 0x3364, 0x17);
    ret += sc500ai_write_register(vi_pipe, 0x336d, 0x03);
    ret += sc500ai_write_register(vi_pipe, 0x3390, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x3391, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3392, 0x38);
    ret += sc500ai_write_register(vi_pipe, 0x3393, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3394, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x3395, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x3396, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x3397, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3398, 0x38);
    ret += sc500ai_write_register(vi_pipe, 0x3399, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x339a, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x339b, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x339c, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x33ac, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x33ae, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x33af, 0x19);
    return ret;
}

static td_s32 sc500ai_linear_5m30_10bit_init_part3(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x360f, 0x01);
    ret += sc500ai_write_register(vi_pipe, 0x3622, 0x03);
    ret += sc500ai_write_register(vi_pipe, 0x363a, 0x1f);
    ret += sc500ai_write_register(vi_pipe, 0x363c, 0x40);
    ret += sc500ai_write_register(vi_pipe, 0x3651, 0x7d);
    ret += sc500ai_write_register(vi_pipe, 0x3670, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3671, 0x07);
    ret += sc500ai_write_register(vi_pipe, 0x3672, 0x17);
    ret += sc500ai_write_register(vi_pipe, 0x3673, 0x1e);
    ret += sc500ai_write_register(vi_pipe, 0x3674, 0x82);
    ret += sc500ai_write_register(vi_pipe, 0x3675, 0x64);
    ret += sc500ai_write_register(vi_pipe, 0x3676, 0x66);
    ret += sc500ai_write_register(vi_pipe, 0x367a, 0x48);
    ret += sc500ai_write_register(vi_pipe, 0x367b, 0x78);
    ret += sc500ai_write_register(vi_pipe, 0x367c, 0x58);
    ret += sc500ai_write_register(vi_pipe, 0x367d, 0x78);
    return ret;
}

static td_s32 sc500ai_linear_5m30_10bit_init_part4(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += sc500ai_write_register(vi_pipe, 0x3690, 0x34);
    ret += sc500ai_write_register(vi_pipe, 0x3691, 0x34);
    ret += sc500ai_write_register(vi_pipe, 0x3692, 0x54);
    ret += sc500ai_write_register(vi_pipe, 0x369c, 0x48);
    ret += sc500ai_write_register(vi_pipe, 0x369d, 0x78);
    ret += sc500ai_write_register(vi_pipe, 0x36ec, 0x1a);
    ret += sc500ai_write_register(vi_pipe, 0x3904, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x3908, 0x41);
    ret += sc500ai_write_register(vi_pipe, 0x391d, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x39c2, 0x30);
    ret += sc500ai_write_register(vi_pipe, 0x3e01, 0xcd);
    ret += sc500ai_write_register(vi_pipe, 0x3e02, 0xc0);
    ret += sc500ai_write_register(vi_pipe, 0x3e16, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x3e17, 0x80);
    ret += sc500ai_write_register(vi_pipe, 0x4500, 0x88);
    ret += sc500ai_write_register(vi_pipe, 0x4509, 0x20);
    return ret;
}

static td_s32 sc500ai_linear_5m30_10bit_init_part5(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += sc500ai_write_register(vi_pipe, 0x5799, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x59e0, 0x60);
    ret += sc500ai_write_register(vi_pipe, 0x59e1, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x59e2, 0x3f);
    ret += sc500ai_write_register(vi_pipe, 0x59e3, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x59e4, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x59e5, 0x3f);
    ret += sc500ai_write_register(vi_pipe, 0x59e7, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x59e8, 0x38);
    ret += sc500ai_write_register(vi_pipe, 0x59e9, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x59ea, 0x0c);
    ret += sc500ai_write_register(vi_pipe, 0x59ec, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x59ed, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x59ee, 0xa0);
    ret += sc500ai_write_register(vi_pipe, 0x59ef, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x59f4, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x59f5, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x59f6, 0x0c);
    ret += sc500ai_write_register(vi_pipe, 0x59f9, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x59fa, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x59fb, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x59fc, 0x0c);
    ret += sc500ai_write_register(vi_pipe, 0x59ff, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x36e9, 0x1c);
    ret += sc500ai_write_register(vi_pipe, 0x36f9, 0x24);
    sc500ai_default_reg_init(vi_pipe);
    ret += sc500ai_write_register(vi_pipe, 0x0100, 0x01);
    return ret;
}

static td_s32 sc500ai_linear_5m30_10bit_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_linear_5m30_10bit_init_part1(vi_pipe);
    ret += sc500ai_linear_5m30_10bit_init_part2(vi_pipe);
    ret += sc500ai_linear_5m30_10bit_init_part3(vi_pipe);
    ret += sc500ai_linear_5m30_10bit_init_part4(vi_pipe);
    ret += sc500ai_linear_5m30_10bit_init_part5(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sc500ai write register failed!\n");
        return ret;
    }

    printf("===================================================================================\n");
    printf("vi_pipe:%d,== SC500AI_27MInput_MIPI_4lane_396Mbps_10bit_2880x1620_30fps Init OK! ==\n", vi_pipe);
    printf("===================================================================================\n");
    return ret;
}

static td_s32 sc500ai_vc_wdr_2t1_5m30_10bit_init_part1(ot_vi_pipe vi_pipe)
{
    // cleaned_0x06_SC500AI_MIPI_27Minput_4lane_792Mbps_10bit_2880x1620_SHDR_VC_30fps_continue_mode.ini
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x0103, 0x01);
    delay_ms(100); /* 100ms */
    ret += sc500ai_write_register(vi_pipe, 0x0100, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x36e9, 0x80);
    ret += sc500ai_write_register(vi_pipe, 0x36f9, 0x80);
    ret += sc500ai_write_register(vi_pipe, 0x301f, 0x06);
    ret += sc500ai_write_register(vi_pipe, 0x3106, 0x01);
    ret += sc500ai_write_register(vi_pipe, 0x320e, 0x0c);
    ret += sc500ai_write_register(vi_pipe, 0x320f, 0xe4);
    ret += sc500ai_write_register(vi_pipe, 0x3220, 0x53);
    ret += sc500ai_write_register(vi_pipe, 0x3250, 0xff);
    ret += sc500ai_write_register(vi_pipe, 0x3253, 0x0a);
    return ret;
}

static td_s32 sc500ai_vc_wdr_2t1_5m30_10bit_init_part2(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x3301, 0x0b);
    ret += sc500ai_write_register(vi_pipe, 0x3302, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x3303, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x3304, 0x70);
    ret += sc500ai_write_register(vi_pipe, 0x3306, 0x50);
    ret += sc500ai_write_register(vi_pipe, 0x3308, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3309, 0x80);
    ret += sc500ai_write_register(vi_pipe, 0x330a, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x330b, 0xe8);
    ret += sc500ai_write_register(vi_pipe, 0x330d, 0x30);
    ret += sc500ai_write_register(vi_pipe, 0x330e, 0x30);
    ret += sc500ai_write_register(vi_pipe, 0x330f, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x3310, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x331c, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x331e, 0x61);
    ret += sc500ai_write_register(vi_pipe, 0x331f, 0x71);
    ret += sc500ai_write_register(vi_pipe, 0x3320, 0x11);
    ret += sc500ai_write_register(vi_pipe, 0x3333, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x334c, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x3356, 0x11);
    ret += sc500ai_write_register(vi_pipe, 0x3364, 0x17);
    ret += sc500ai_write_register(vi_pipe, 0x336d, 0x03);
    ret += sc500ai_write_register(vi_pipe, 0x3390, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x3391, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3392, 0x38);
    ret += sc500ai_write_register(vi_pipe, 0x3393, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3394, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3395, 0x12);
    ret += sc500ai_write_register(vi_pipe, 0x3396, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x3397, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3398, 0x38);
    ret += sc500ai_write_register(vi_pipe, 0x3399, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x339a, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x339b, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x339c, 0x12);
    ret += sc500ai_write_register(vi_pipe, 0x33ac, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x33ae, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x33af, 0x21);
    return ret;
}

static td_s32 sc500ai_vc_wdr_2t1_5m30_10bit_init_part3(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x360f, 0x01);
    ret += sc500ai_write_register(vi_pipe, 0x3621, 0xe8);
    ret += sc500ai_write_register(vi_pipe, 0x3622, 0x06);
    ret += sc500ai_write_register(vi_pipe, 0x3630, 0x82);
    ret += sc500ai_write_register(vi_pipe, 0x3633, 0x33);
    ret += sc500ai_write_register(vi_pipe, 0x3634, 0x64);
    ret += sc500ai_write_register(vi_pipe, 0x3637, 0x50);
    ret += sc500ai_write_register(vi_pipe, 0x363a, 0x1f);
    ret += sc500ai_write_register(vi_pipe, 0x363c, 0x40);
    ret += sc500ai_write_register(vi_pipe, 0x3651, 0x7d);
    ret += sc500ai_write_register(vi_pipe, 0x3670, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3671, 0x06);
    ret += sc500ai_write_register(vi_pipe, 0x3672, 0x16);
    ret += sc500ai_write_register(vi_pipe, 0x3673, 0x17);
    ret += sc500ai_write_register(vi_pipe, 0x3674, 0x82);
    ret += sc500ai_write_register(vi_pipe, 0x3675, 0x62);
    ret += sc500ai_write_register(vi_pipe, 0x3676, 0x44);
    ret += sc500ai_write_register(vi_pipe, 0x367a, 0x48);
    ret += sc500ai_write_register(vi_pipe, 0x367b, 0x78);
    ret += sc500ai_write_register(vi_pipe, 0x367c, 0x48);
    ret += sc500ai_write_register(vi_pipe, 0x367d, 0x58);
    return ret;
}

static td_s32 sc500ai_vc_wdr_2t1_5m30_10bit_init_part4(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x3690, 0x34);
    ret += sc500ai_write_register(vi_pipe, 0x3691, 0x34);
    ret += sc500ai_write_register(vi_pipe, 0x3692, 0x54);
    ret += sc500ai_write_register(vi_pipe, 0x369c, 0x48);
    ret += sc500ai_write_register(vi_pipe, 0x369d, 0x78);
    ret += sc500ai_write_register(vi_pipe, 0x36ea, 0x35);
    ret += sc500ai_write_register(vi_pipe, 0x36eb, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x36ec, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x36ed, 0x14);
    ret += sc500ai_write_register(vi_pipe, 0x36fa, 0x35);
    ret += sc500ai_write_register(vi_pipe, 0x36fb, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x36fc, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x36fd, 0x16);
    ret += sc500ai_write_register(vi_pipe, 0x3904, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x3908, 0x41);
    ret += sc500ai_write_register(vi_pipe, 0x391f, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x39c2, 0x30);
    return ret;
}

static td_s32 sc500ai_vc_wdr_2t1_5m30_10bit_init_part5(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x3e00, 0x01);
    ret += sc500ai_write_register(vi_pipe, 0x3e01, 0x82);
    ret += sc500ai_write_register(vi_pipe, 0x3e02, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x3e04, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3e05, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x3e23, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x3e24, 0xc6);
    ret += sc500ai_write_register(vi_pipe, 0x4500, 0x88);
    ret += sc500ai_write_register(vi_pipe, 0x4509, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x4800, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x4837, 0x15);
    ret += sc500ai_write_register(vi_pipe, 0x4853, 0xfd);
    ret += sc500ai_write_register(vi_pipe, 0x36e9, 0x44);
    ret += sc500ai_write_register(vi_pipe, 0x36f9, 0x44);
    sc500ai_default_reg_init(vi_pipe);
    ret += sc500ai_write_register(vi_pipe, 0x0100, 0x01);
    return ret;
}


static td_s32 sc500ai_vc_wdr_2t1_5m30_10bit_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += sc500ai_vc_wdr_2t1_5m30_10bit_init_part1(vi_pipe);
    ret += sc500ai_vc_wdr_2t1_5m30_10bit_init_part2(vi_pipe);
    ret += sc500ai_vc_wdr_2t1_5m30_10bit_init_part3(vi_pipe);
    ret += sc500ai_vc_wdr_2t1_5m30_10bit_init_part4(vi_pipe);
    ret += sc500ai_vc_wdr_2t1_5m30_10bit_init_part5(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sc500ai_cmos write register failed!\n");
        return ret;
    }
    printf("===========================================================================================\n");
    printf("vi_pipe:%d,== SC500AI_MIPI_27Minput_4lane_792Mbps_10bit_2880x1620_SHDR_VC_30fps Init OK! ==\n", vi_pipe);
    printf("============================================================================================\n");
    return ret;
}


static td_s32 sc500ai_linear_800p60_10bit_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x0103, 0x01);
    delay_ms(100); /* 100ms */
    ret += sc500ai_write_register(vi_pipe, 0x0100, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x36e9, 0x80);
    ret += sc500ai_write_register(vi_pipe, 0x36f9, 0x80);
	ret += sc500ai_write_register(vi_pipe, 0x3018, 0x32);
	ret += sc500ai_write_register(vi_pipe, 0x3019, 0x0c);
	ret += sc500ai_write_register(vi_pipe, 0x301f, 0xca);
	ret += sc500ai_write_register(vi_pipe, 0x3207, 0x5b);
	ret += sc500ai_write_register(vi_pipe, 0x3208, 0x05);
	ret += sc500ai_write_register(vi_pipe, 0x3209, 0xa0);
	ret += sc500ai_write_register(vi_pipe, 0x320a, 0x03);
	ret += sc500ai_write_register(vi_pipe, 0x320b, 0x2a);
	ret += sc500ai_write_register(vi_pipe, 0x320c, 0x06);   // HTS_H
	ret += sc500ai_write_register(vi_pipe, 0x320d, 0x2c);   // HTS_L
	ret += sc500ai_write_register(vi_pipe, 0x320e, 0x03);   // VTS_H
	ret += sc500ai_write_register(vi_pipe, 0x320f, 0x43);   // VTS_L
	ret += sc500ai_write_register(vi_pipe, 0x3210, 0x00);   
	ret += sc500ai_write_register(vi_pipe, 0x3211, 0x02);  
	ret += sc500ai_write_register(vi_pipe, 0x3212, 0x00);  
	ret += sc500ai_write_register(vi_pipe, 0x3213, 0x02);  
	ret += sc500ai_write_register(vi_pipe, 0x3215, 0x31);     
	ret += sc500ai_write_register(vi_pipe, 0x3220, 0x17);
	ret += sc500ai_write_register(vi_pipe, 0x3253, 0x0a);
	ret += sc500ai_write_register(vi_pipe, 0x3301, 0x0a);
	ret += sc500ai_write_register(vi_pipe, 0x3302, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3303, 0x10);
	ret += sc500ai_write_register(vi_pipe, 0x3304, 0x60);
	ret += sc500ai_write_register(vi_pipe, 0x3306, 0x60);
    ret += sc500ai_write_register(vi_pipe, 0x3308, 0x10);
	ret += sc500ai_write_register(vi_pipe, 0x3309, 0x70);
	ret += sc500ai_write_register(vi_pipe, 0x330a, 0x00);
	ret += sc500ai_write_register(vi_pipe, 0x330b, 0xf0);
	ret += sc500ai_write_register(vi_pipe, 0x330d, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x330e, 0x20);
	ret += sc500ai_write_register(vi_pipe, 0x330f, 0x02);
	ret += sc500ai_write_register(vi_pipe, 0x3310, 0x02);
	ret += sc500ai_write_register(vi_pipe, 0x331c, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x331e, 0x51);
    ret += sc500ai_write_register(vi_pipe, 0x331f, 0x61);
	ret += sc500ai_write_register(vi_pipe, 0x3320, 0x09);
	ret += sc500ai_write_register(vi_pipe, 0x3333, 0x10);
	ret += sc500ai_write_register(vi_pipe, 0x334c, 0x08);
	ret += sc500ai_write_register(vi_pipe, 0x3356, 0x09);
    ret += sc500ai_write_register(vi_pipe, 0x3364, 0x17);
	ret += sc500ai_write_register(vi_pipe, 0x336d, 0x03);
	ret += sc500ai_write_register(vi_pipe, 0x3390, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x3391, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3392, 0x38);
    ret += sc500ai_write_register(vi_pipe, 0x3393, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3394, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x3395, 0x20);
	ret += sc500ai_write_register(vi_pipe, 0x3396, 0x08);
	ret += sc500ai_write_register(vi_pipe, 0x3397, 0x18);
	ret += sc500ai_write_register(vi_pipe, 0x3398, 0x38);
    ret += sc500ai_write_register(vi_pipe, 0x3399, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x339a, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x339b, 0x20);
	ret += sc500ai_write_register(vi_pipe, 0x339c, 0x20);
	ret += sc500ai_write_register(vi_pipe, 0x33ac, 0x10);
	ret += sc500ai_write_register(vi_pipe, 0x33ae, 0x10);
	ret += sc500ai_write_register(vi_pipe, 0x33af, 0x19);
    ret += sc500ai_write_register(vi_pipe, 0x360f, 0x01);
	ret += sc500ai_write_register(vi_pipe, 0x3622, 0x03);
	ret += sc500ai_write_register(vi_pipe, 0x363a, 0x1f);
    ret += sc500ai_write_register(vi_pipe, 0x363c, 0x40);
	ret += sc500ai_write_register(vi_pipe, 0x3651, 0x7d);
    ret += sc500ai_write_register(vi_pipe, 0x3670, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3671, 0x07);
    ret += sc500ai_write_register(vi_pipe, 0x3672, 0x17);
    ret += sc500ai_write_register(vi_pipe, 0x3673, 0x1e);
	ret += sc500ai_write_register(vi_pipe, 0x3674, 0x82);
	ret += sc500ai_write_register(vi_pipe, 0x3675, 0x64);
	ret += sc500ai_write_register(vi_pipe, 0x3676, 0x66);
    ret += sc500ai_write_register(vi_pipe, 0x367a, 0x48);
	ret += sc500ai_write_register(vi_pipe, 0x367b, 0x78);
	ret += sc500ai_write_register(vi_pipe, 0x367c, 0x58);
    ret += sc500ai_write_register(vi_pipe, 0x367d, 0x78);
    ret += sc500ai_write_register(vi_pipe, 0x3690, 0x34);
	ret += sc500ai_write_register(vi_pipe, 0x3691, 0x34);
	ret += sc500ai_write_register(vi_pipe, 0x3692, 0x54);
    ret += sc500ai_write_register(vi_pipe, 0x369c, 0x48);
    ret += sc500ai_write_register(vi_pipe, 0x369d, 0x78);
	ret += sc500ai_write_register(vi_pipe, 0x36ec, 0x1a);
	ret += sc500ai_write_register(vi_pipe, 0x36ec, 0x1a);
	ret += sc500ai_write_register(vi_pipe, 0x36ed, 0x34);
	ret += sc500ai_write_register(vi_pipe, 0x36fa, 0x32);
	ret += sc500ai_write_register(vi_pipe, 0x36fb, 0x0e);
	ret += sc500ai_write_register(vi_pipe, 0x36fc, 0x10);
	ret += sc500ai_write_register(vi_pipe, 0x36fd, 0x14);
    ret += sc500ai_write_register(vi_pipe, 0x36ec, 0x1a);
	ret += sc500ai_write_register(vi_pipe, 0x36ed, 0x34);
	ret += sc500ai_write_register(vi_pipe, 0x36fa, 0x32);
	ret += sc500ai_write_register(vi_pipe, 0x36fb, 0x0e);
	ret += sc500ai_write_register(vi_pipe, 0x36fc, 0x10);
	ret += sc500ai_write_register(vi_pipe, 0x36fd, 0x14);
    ret += sc500ai_write_register(vi_pipe, 0x3904, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x3908, 0x41);
	ret += sc500ai_write_register(vi_pipe, 0x391d, 0x04);
	ret += sc500ai_write_register(vi_pipe, 0x39c2, 0x30);
	ret += sc500ai_write_register(vi_pipe, 0x3e01, 0x67);
	ret += sc500ai_write_register(vi_pipe, 0x3e02, 0xc0);
	ret += sc500ai_write_register(vi_pipe, 0x3e16, 0x00);
	ret += sc500ai_write_register(vi_pipe, 0x3e17, 0x80);
	ret += sc500ai_write_register(vi_pipe, 0x4500, 0x88);
	ret += sc500ai_write_register(vi_pipe, 0x4509, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x5000, 0x46);
    ret += sc500ai_write_register(vi_pipe, 0x5799, 0x00);
	ret += sc500ai_write_register(vi_pipe, 0x5900, 0xf1);
    ret += sc500ai_write_register(vi_pipe, 0x5901, 0x04);
	ret += sc500ai_write_register(vi_pipe, 0x59e0, 0x60);
    ret += sc500ai_write_register(vi_pipe, 0x59e1, 0x08);
	ret += sc500ai_write_register(vi_pipe, 0x59e2, 0x3f);
	ret += sc500ai_write_register(vi_pipe, 0x59e3, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x59e4, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x59e5, 0x3f);
	ret += sc500ai_write_register(vi_pipe, 0x59e7, 0x02);	// hs-trail
	ret += sc500ai_write_register(vi_pipe, 0x59e8, 0x38);
	ret += sc500ai_write_register(vi_pipe, 0x59e9, 0x20);
	ret += sc500ai_write_register(vi_pipe, 0x59ea, 0x0c);
	ret += sc500ai_write_register(vi_pipe, 0x59ec, 0x08);
	ret += sc500ai_write_register(vi_pipe, 0x59ed, 0x02);
	ret += sc500ai_write_register(vi_pipe, 0x59ee, 0xa0);
	ret += sc500ai_write_register(vi_pipe, 0x59ef, 0x08);
	ret += sc500ai_write_register(vi_pipe, 0x59f4, 0x18);
	ret += sc500ai_write_register(vi_pipe, 0x59f5, 0x10);
	ret += sc500ai_write_register(vi_pipe, 0x59f6, 0x0c);
	ret += sc500ai_write_register(vi_pipe, 0x59f9, 0x02);
	ret += sc500ai_write_register(vi_pipe, 0x59fa, 0x18);
	ret += sc500ai_write_register(vi_pipe, 0x59fb, 0x10);
	ret += sc500ai_write_register(vi_pipe, 0x59fc, 0x0c);
	ret += sc500ai_write_register(vi_pipe, 0x59ff, 0x02);
	ret += sc500ai_write_register(vi_pipe, 0x36e9, 0x1c);
	ret += sc500ai_write_register(vi_pipe, 0x36f9, 0x24);
    sc500ai_default_reg_init(vi_pipe);
    ret += sc500ai_write_register(vi_pipe, 0x0100, 0x01);
    if (ret  != TD_SUCCESS) {
        isp_err_trace("sc500ai write register failed!\n");
        return ret;
    }

    printf("===================================================================================\n");
    printf("vi_pipe:%d,== SC500AI_27MInput_MIPI_2lane_592Mbps_10bit_1440x800_60fps Init OK! ==\n", vi_pipe);
    printf("===================================================================================\n");
    return ret;
}



static td_s32 sc500ai_linear_1080p60_10bit_init(ot_vi_pipe vi_pipe)
{
    
    td_s32 ret = TD_SUCCESS;
    ret += sc500ai_write_register(vi_pipe, 0x0103, 0x01);
    delay_ms(100); /* 100ms */
    ret += sc500ai_write_register(vi_pipe, 0x0100, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x36e9, 0x80);
    ret += sc500ai_write_register(vi_pipe, 0x36f9, 0x80);
    ret += sc500ai_write_register(vi_pipe, 0x301f, 0x06);
    ret += sc500ai_write_register(vi_pipe, 0x3106, 0x01);
	ret += sc500ai_write_register(vi_pipe, 0x3200, 0x01);
	ret += sc500ai_write_register(vi_pipe, 0x3201, 0xe0);
	ret += sc500ai_write_register(vi_pipe, 0x3202, 0x01);
	ret += sc500ai_write_register(vi_pipe, 0x3203, 0x0e);
	ret += sc500ai_write_register(vi_pipe, 0x3204, 0x09);
	ret += sc500ai_write_register(vi_pipe, 0x3205, 0x67);
	ret += sc500ai_write_register(vi_pipe, 0x3206, 0x05);
	ret += sc500ai_write_register(vi_pipe, 0x3207, 0x4d);
	ret += sc500ai_write_register(vi_pipe, 0x3208, 0x07);
	ret += sc500ai_write_register(vi_pipe, 0x3209, 0x80);
	ret += sc500ai_write_register(vi_pipe, 0x320a, 0x04);
	ret += sc500ai_write_register(vi_pipe, 0x320b, 0x38);
	ret += sc500ai_write_register(vi_pipe, 0x320c, 0x05);   // HTS_H
	ret += sc500ai_write_register(vi_pipe, 0x320d, 0xf0);   // HTS_L
	ret += sc500ai_write_register(vi_pipe, 0x320e, 0x08);   // VTS_H
	ret += sc500ai_write_register(vi_pipe, 0x320f, 0x98);   // VTS_L
    ret += sc500ai_write_register(vi_pipe, 0x3220, 0x03);
    ret += sc500ai_write_register(vi_pipe, 0x3250, 0xff);
    ret += sc500ai_write_register(vi_pipe, 0x3253, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3301, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3302, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3303, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x3304, 0x60);
    ret += sc500ai_write_register(vi_pipe, 0x3306, 0x60);
    ret += sc500ai_write_register(vi_pipe, 0x3308, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x3309, 0x70);
    ret += sc500ai_write_register(vi_pipe, 0x330a, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x330b, 0xf0);
    ret += sc500ai_write_register(vi_pipe, 0x330d, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x330e, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x330f, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x3310, 0x02);
    ret += sc500ai_write_register(vi_pipe, 0x331c, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x331e, 0x51);
    ret += sc500ai_write_register(vi_pipe, 0x331f, 0x61);
    ret += sc500ai_write_register(vi_pipe, 0x3320, 0x09);
    ret += sc500ai_write_register(vi_pipe, 0x3333, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x334c, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x3356, 0x09);
    ret += sc500ai_write_register(vi_pipe, 0x3364, 0x17);
    ret += sc500ai_write_register(vi_pipe, 0x336d, 0x03);
    ret += sc500ai_write_register(vi_pipe, 0x3390, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x3391, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3392, 0x38);
    ret += sc500ai_write_register(vi_pipe, 0x3393, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3394, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x3395, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x3396, 0x08);
    ret += sc500ai_write_register(vi_pipe, 0x3397, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3398, 0x38);
    ret += sc500ai_write_register(vi_pipe, 0x3399, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x339a, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x339b, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x339c, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x33ac, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x33ae, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x33af, 0x19);
    ret += sc500ai_write_register(vi_pipe, 0x360f, 0x01);
    ret += sc500ai_write_register(vi_pipe, 0x3621, 0xe8);
    ret += sc500ai_write_register(vi_pipe, 0x3622, 0x06);
    ret += sc500ai_write_register(vi_pipe, 0x3630, 0x82);
    ret += sc500ai_write_register(vi_pipe, 0x3633, 0x33);
    ret += sc500ai_write_register(vi_pipe, 0x3634, 0x64);
    ret += sc500ai_write_register(vi_pipe, 0x3637, 0x50);
    ret += sc500ai_write_register(vi_pipe, 0x363a, 0x1f);
    ret += sc500ai_write_register(vi_pipe, 0x363c, 0x40);
    ret += sc500ai_write_register(vi_pipe, 0x3651, 0x7d);
    ret += sc500ai_write_register(vi_pipe, 0x3670, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x3671, 0x06);
    ret += sc500ai_write_register(vi_pipe, 0x3672, 0x16);
    ret += sc500ai_write_register(vi_pipe, 0x3673, 0x17);
    ret += sc500ai_write_register(vi_pipe, 0x3674, 0x82);
    ret += sc500ai_write_register(vi_pipe, 0x3675, 0x62);
    ret += sc500ai_write_register(vi_pipe, 0x3676, 0x44);
    ret += sc500ai_write_register(vi_pipe, 0x367a, 0x48);
    ret += sc500ai_write_register(vi_pipe, 0x367b, 0x78);
    ret += sc500ai_write_register(vi_pipe, 0x367c, 0x48);
    ret += sc500ai_write_register(vi_pipe, 0x367d, 0x58);
    ret += sc500ai_write_register(vi_pipe, 0x3690, 0x34);
    ret += sc500ai_write_register(vi_pipe, 0x3691, 0x34);
    ret += sc500ai_write_register(vi_pipe, 0x3692, 0x54);
    ret += sc500ai_write_register(vi_pipe, 0x369c, 0x48);
    ret += sc500ai_write_register(vi_pipe, 0x369d, 0x78);
    ret += sc500ai_write_register(vi_pipe, 0x36ea, 0x35);
    ret += sc500ai_write_register(vi_pipe, 0x36eb, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x36ec, 0x0a);
    ret += sc500ai_write_register(vi_pipe, 0x36ed, 0x14);
    ret += sc500ai_write_register(vi_pipe, 0x36fa, 0x35);
    ret += sc500ai_write_register(vi_pipe, 0x36fb, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x36fc, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x36fd, 0x26);
    ret += sc500ai_write_register(vi_pipe, 0x3904, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x3908, 0x41);
    ret += sc500ai_write_register(vi_pipe, 0x391f, 0x10);
    ret += sc500ai_write_register(vi_pipe, 0x39c2, 0x30);
    ret += sc500ai_write_register(vi_pipe, 0x3e00, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x3e01, 0x52);
    ret += sc500ai_write_register(vi_pipe, 0x3e02, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x3e04, 0x18);
    ret += sc500ai_write_register(vi_pipe, 0x3e05, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x3e23, 0x00);
    ret += sc500ai_write_register(vi_pipe, 0x3e24, 0xc6);
    ret += sc500ai_write_register(vi_pipe, 0x4500, 0x88);
    ret += sc500ai_write_register(vi_pipe, 0x4509, 0x20);
    ret += sc500ai_write_register(vi_pipe, 0x4800, 0x04);
    ret += sc500ai_write_register(vi_pipe, 0x4837, 0x15);
    ret += sc500ai_write_register(vi_pipe, 0x4853, 0xfd);
    ret += sc500ai_write_register(vi_pipe, 0x36e9, 0x44);   // pll1
    ret += sc500ai_write_register(vi_pipe, 0x36f9, 0x44);   // pll2
    sc500ai_default_reg_init(vi_pipe);
    ret += sc500ai_write_register(vi_pipe, 0x0100, 0x01);
    if (ret  != TD_SUCCESS) {
        isp_err_trace("sc500ai write register failed!\n");
        return ret;
    }

    printf("===================================================================================\n");
    printf("vi_pipe:%d,== SC500AI_27MInput_MIPI_4lane_792Mbps_10bit_1920x1080_90fps Init OK! ==\n", vi_pipe);
    printf("===================================================================================\n");
    return ret;
}
