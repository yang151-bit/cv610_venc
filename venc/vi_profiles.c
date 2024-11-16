#include "main.h"

// VI device configuration profiles
ot_vi_dev_attr DEV_ATTR_SC500AI_BASE = {
    .intf_mode = OT_VI_INTF_MODE_MIPI,

    /* Invalid argument */
    .work_mode = OT_VI_WORK_MODE_MULTIPLEX_1,

    /* mask component */
    .component_mask = {0xfff00000, 0x00000000},

    .scan_mode = OT_VI_SCAN_PROGRESSIVE,

    /* Invalid argument */
    .ad_chn_id = {-1, -1, -1, -1},

    /* data seq */
    .data_seq = OT_VI_DATA_SEQ_YVYU,

    /* sync param */
    .sync_cfg = {
        .vsync           = OT_VI_VSYNC_FIELD,
        .vsync_neg       = OT_VI_VSYNC_NEG_HIGH,
        .hsync           = OT_VI_HSYNC_VALID_SIG,
        .hsync_neg       = OT_VI_HSYNC_NEG_HIGH,
        .vsync_valid     = OT_VI_VSYNC_VALID_SIG,
        .vsync_valid_neg = OT_VI_VSYNC_VALID_NEG_HIGH,
        .timing_blank    = {
            /* hsync_hfb      hsync_act     hsync_hhb */
            0,                0,            0,
            /* vsync0_vhb     vsync0_act    vsync0_hhb */
            0,                0,            0,
            /* vsync1_vhb     vsync1_act    vsync1_hhb */
            0,                0,            0
        }
    },

    /* data type */
    .data_type = OT_VI_DATA_TYPE_RAW,

    /* data reverse */
    .data_reverse = TD_FALSE,

    /* input size */
    .in_size = {1920, 1080},

    /* data rate */
    .data_rate = OT_DATA_RATE_X1,
};

// VI Pipe configuration profiles
ot_vi_pipe_attr PIPE_ATTR_RAW12_420 = {
    OT_VI_PIPE_BYPASS_NONE, TD_FALSE,
    {0, 0},
    OT_PIXEL_FORMAT_RGB_BAYER_12BPP,
    OT_COMPRESS_MODE_NONE,
    { -1, -1}
};

ot_vi_pipe_attr PIPE_ATTR_RAW10_420 = {
    OT_VI_PIPE_BYPASS_NONE, TD_FALSE,
    {0, 0},
    OT_PIXEL_FORMAT_RGB_BAYER_10BPP,
    OT_COMPRESS_MODE_NONE,
    { -1, -1}
};

// 3DNR configuration profiles
ot_3dnr_attr PIPE_3DNR_ATTR_NORM = {
    TD_TRUE, 
    OT_NR_TYPE_VIDEO_NORM,
    OT_COMPRESS_MODE_FRAME,
    OT_NR_MOTION_MODE_NORM
};

// VI Channel configuration profiles
ot_vi_chn_attr CHN_ATTR_420_SDR8_LINEAR = {
    {0, 0},
    OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    OT_DYNAMIC_RANGE_SDR8,
    OT_VIDEO_FORMAT_LINEAR,
    OT_COMPRESS_MODE_NONE,
    0, 0,
    0,
    { -1, -1}
};
