/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_3A_H
#define OT_COMMON_3A_H

#include "ot_common.h"
#include "ot_common_isp.h"
#include "ot_common_sns.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define is_linear_mode(mode)      ((mode) == OT_WDR_MODE_NONE)
#define is_wdr_mode(mode)         ((!is_linear_mode(mode)) && ((mode) < OT_WDR_MODE_BUTT))
#define is_hdr_mode(mode)         (((mode) > OT_DYNAMIC_RANGE_SDR10) && ((mode) < OT_DYNAMIC_RANGE_BUTT))
#define is_built_in_wdr_mode(mode) ((mode) == OT_WDR_MODE_BUILT_IN)
#define is_fs_wdr_mode(mode)      ((!is_linear_mode(mode)) && (!is_built_in_wdr_mode(mode)) && \
                                   ((mode) < OT_WDR_MODE_BUTT))
#define is_2to1_wdr_mode(mode)    (((mode) == OT_WDR_MODE_2To1_FRAME) || \
                                   ((mode) == OT_WDR_MODE_2To1_LINE) || ((mode) == OT_WDR_MODE_QUADRA))
#define is_3to1_wdr_mode(mode)    (((mode) == OT_WDR_MODE_3To1_FRAME) || ((mode) == OT_WDR_MODE_3To1_LINE))
#define is_4to1_wdr_mode(mode)    (((mode) == OT_WDR_MODE_4To1_FRAME) || ((mode) == OT_WDR_MODE_4To1_LINE))
#define is_full_wdr_mode(mode)     0

#define is_half_wdr_mode(mode)    (((mode) == OT_WDR_MODE_2To1_FRAME) || ((mode) == OT_WDR_MODE_3To1_FRAME) || \
                                   ((mode) == OT_WDR_MODE_4To1_FRAME))
#define is_line_wdr_mode(mode)    (((mode) == OT_WDR_MODE_2To1_LINE) || ((mode) == OT_WDR_MODE_3To1_LINE) || \
                                   ((mode) == OT_WDR_MODE_4To1_LINE) || ((mode) == OT_WDR_MODE_QUADRA))

#define is_stitch_main_pipe(vi_pipe, main_pipe) ((vi_pipe) == (main_pipe))
#define is_offline_mode(mode)    ((mode) == ISP_MODE_RUNNING_OFFLINE)
#define is_online_mode(mode)     ((mode) == ISP_MODE_RUNNING_ONLINE)
#define is_sidebyside_mode(mode) ((mode) == ISP_MODE_RUNNING_SIDEBYSIDE)
#define is_striping_mode(mode)   ((mode) == ISP_MODE_RUNNING_STRIPING)
#define is_pre_online_post_offline(mode)  ((mode) == ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE)
#define is_virt_pipe(vi_pipe)    (((vi_pipe) >= OT_ISP_MAX_PHY_PIPE_NUM) && ((vi_pipe) < OT_ISP_MAX_PIPE_NUM))
#define is_no_fe_pipe(vi_pipe)   (((vi_pipe) >= OT_ISP_MAX_FE_PIPE_NUM) && ((vi_pipe) < OT_ISP_MAX_PIPE_NUM))
#define is_no_fe_phy_pipe(vi_pipe)    (((vi_pipe) >= OT_ISP_MAX_FE_PIPE_NUM) && ((vi_pipe) < OT_ISP_MAX_PHY_PIPE_NUM))

#define OT_ISP_MAX_REGISTER_ALG_LIB_NUM 2
#define OT_ISP_THERMO_FUSION_CURVE_NUM 1025

typedef enum {
    OT_ISP_ALG_AE = 0,
    OT_ISP_ALG_AF,             /* 1 */
    OT_ISP_ALG_AWB,            /* 2 */
    OT_ISP_ALG_BLC,            /* 3 */
    OT_ISP_ALG_DP,             /* 4 */
    OT_ISP_ALG_PREGAMMA,       /* 5 */
    OT_ISP_ALG_DRC,            /* 6 */
    OT_ISP_ALG_DEMOSAIC,       /* 7 */
    OT_ISP_ALG_ANTIFALSECOLOR, /* 8 */
    OT_ISP_ALG_GAMMA,          /* 9 */
    OT_ISP_ALG_GE,             /* 10 */
    OT_ISP_ALG_SHARPEN,        /* 11 */
    OT_ISP_ALG_FRAMEWDR,       /* 12 */
    OT_ISP_ALG_FPN,            /* 13 */
    OT_ISP_ALG_DEHAZE,         /* 14 */
    OT_ISP_ALG_LCAC,           /* 15 */
    OT_ISP_ALG_CAC,            /* 16 */
    OT_ISP_ALG_BSHP,           /* 17 */
    OT_ISP_ALG_CSC,            /* 18 */
    OT_ISP_ALG_EXPANDER,       /* 19 */
    OT_ISP_ALG_MCDS,           /* 20 */
    OT_ISP_ALG_ACS,            /* 21 */
    OT_ISP_ALG_LSC,            /* 22 */
    OT_ISP_ALG_RC,             /* 23 */
    OT_ISP_ALG_RGBIR,          /* 24 */
    OT_ISP_ALG_HRS,            /* 25 */
    OT_ISP_ALG_DG,             /* 26 */
    OT_ISP_ALG_BAYERNR,        /* 27 */
    OT_ISP_ALG_FLICKER,        /* 28 */
    OT_ISP_ALG_LDCI,           /* 29 */
    OT_ISP_ALG_CA,             /* 30 */
    OT_ISP_ALG_CLUT,           /* 31 */
    OT_ISP_ALG_CCM,            /* 32 */
    OT_ISP_ALG_AI,             /* 33 */
    OT_ISP_ALG_CRB,            /* 34 */
    OT_ISP_ALG_HNR,            /* 35 */
    OT_ISP_ALG_LBLC,           /* 36 */
    OT_ISP_ALG_MOD_BUTT,
} ot_isp_alg_mod;

typedef enum {
    OT_ISP_WDR_MODE_SET = 8000,
    OT_ISP_PROC_WRITE,

    OT_ISP_AE_FPS_BASE_SET,
    OT_ISP_AE_BLC_SET,
    OT_ISP_AE_RC_SET,
    OT_ISP_AE_BAYER_FORMAT_SET,
    OT_ISP_AE_INIT_INFO_GET,

    OT_ISP_AWB_ISO_SET,  /* set iso, change saturation when iso change */
    OT_ISP_CHANGE_IMAGE_MODE_SET,
    OT_ISP_UPDATE_INFO_GET,
    OT_ISP_FRAMEINFO_GET,
    OT_ISP_ATTACHINFO_GET,
    OT_ISP_COLORGAMUTINFO_GET,
    OT_ISP_AWB_INTTIME_SET,
    OT_ISP_BAS_MODE_SET,
    OT_ISP_PROTRIGGER_SET,
    OT_ISP_AWB_PIRIS_SET,
    OT_ISP_AWB_SNAP_MODE_SET,
    OT_ISP_AWB_ZONE_ROW_SET,
    OT_ISP_AWB_ZONE_COL_SET,
    OT_ISP_AWB_ZONE_BIN_SET,
    OT_ISP_AWB_ERR_GET,

    OT_ISP_DEHAZE_DEBUG_ATTR_SET,
    OT_ISP_AE_DEBUG_ATTR_SET,
    OT_ISP_CTRL_CMD_BUTT,
} ot_isp_ctrl_cmd;

typedef struct {
    td_char *proc_buff;
    td_u32   buff_len;
    size_t   write_len;   /* the len count should contain '\0'. */
} ot_isp_ctrl_proc_write;

typedef struct {
    td_bool stitch_enable;
    td_bool main_pipe;
    td_u8   stitch_pipe_num;
    td_s8   stitch_bind_id[OT_ISP_MAX_STITCH_NUM];
} ot_isp_stitch_attr; /* Not support for Hi3516CV610 */

/* AE */
/* the init param of ae alg */
typedef struct {
    ot_sensor_id sns_id;
    td_u8  wdr_mode;
    td_u8  hdr_mode;
    td_u16 black_level;
    td_float fps;
    ot_isp_bayer_format bayer;
    ot_isp_stitch_attr stitch_attr;

    td_s32 reserved;
} ot_isp_ae_param;

/* the statistics of ae alg */
typedef struct {
    td_u32  pixel_count[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  pixel_weight[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  histogram_mem_array[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_HIST_NUM];
    td_u32  histogram_mem_array_ir[OT_ISP_HIST_NUM]; /* only support fe0  not support for Hi3519DV500 and Hi3516CV610 */
} ot_isp_fe_ae_stat_1;

typedef struct {
    td_u16  global_avg_r[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  global_avg_gr[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  global_avg_gb[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  global_avg_b[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  global_avg_ir; /* only support fe0  not support for Hi3519DV500 */
} ot_isp_fe_ae_stat_2; /* Not support for Hi3516CV610 */

typedef struct {
    td_u16  zone_avg[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM];
    td_u16  zone_avg_ir[OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN]; /* only support fe0 not support for Hi3519DV500 */
} ot_isp_fe_ae_stat_3; /* Not support for Hi3516CV610 */

typedef struct {
    td_u16  zone_avg[OT_ISP_MAX_STITCH_NUM][OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN]\
        [OT_ISP_BAYER_CHN_NUM];
} ot_isp_fe_ae_stitch_stat_3; /* Not support for Hi3516CV610 */

typedef struct {
    td_u32  pixel_count;
    td_u32  pixel_weight;
    td_u32  histogram_mem_array[OT_ISP_HIST_NUM];
    td_u32  estimate_histogram_mem_array[OT_ISP_HIST_NUM];
} ot_isp_be_ae_stat_1;

typedef struct {
    td_u16  global_avg_r;
    td_u16  global_avg_gr;
    td_u16  global_avg_gb;
    td_u16  global_avg_b;
} ot_isp_be_ae_stat_2;

typedef struct {
    td_u16  zone_avg[OT_ISP_BE_AE_ZONE_ROW][OT_ISP_BE_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM];
} ot_isp_be_ae_stat_3;

typedef struct {
    td_u16  zone_avg[OT_ISP_MAX_STITCH_NUM][OT_ISP_BE_AE_ZONE_ROW][OT_ISP_BE_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM];
} ot_isp_be_ae_stitch_stat_3; /* Not support for Hi3516CV610 */

typedef struct {
    td_u32  frame_cnt;    /* the counting of frame */
    td_u64  frame_pts;
    td_u16  frame_width;
    td_u16  frame_height;
    ot_isp_smart_info smart_info;

    ot_isp_fe_ae_stat_1 *fe_ae_stat1;
    ot_isp_fe_ae_stat_2 *fe_ae_stat2;           /* Not support for Hi3516CV610 */
    ot_isp_fe_ae_stat_3 *fe_ae_stat3;           /* Not support for Hi3516CV610 */
    ot_isp_fe_ae_stitch_stat_3 *fe_ae_sti_stat; /* Not support for Hi3516CV610 */
    ot_isp_be_ae_stat_1 *be_ae_stat1;
    ot_isp_be_ae_stat_2 *be_ae_stat2;
    ot_isp_be_ae_stat_3 *be_ae_stat3;
    ot_isp_be_ae_stitch_stat_3 *be_ae_sti_stat; /* Not support for Hi3516CV610 */
} ot_isp_ae_info;

typedef struct {
    td_bool change;

    td_bool hist_adjust;
    td_u8 ae_be_sel;
    td_u8 four_plane_mode;
    td_u8 hist_offset_x;
    td_u8 hist_offset_y;
    td_u8 hist_skip_x;
    td_u8 hist_skip_y;

    td_bool mode_update;
    td_u8 hist_mode;
    td_u8 aver_mode;
    td_u8 max_gain_mode;

    td_bool wight_table_update;
    td_u8 weight_table[OT_VI_MAX_PIPE_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN];
    td_u8 be_weight_table[OT_VI_MAX_PIPE_NUM][OT_ISP_BE_AE_ZONE_ROW][OT_ISP_BE_AE_ZONE_COLUMN];
} ot_isp_ae_stat_attr;

typedef struct {
    td_u8   dark_throw_ratio;    /* R; Range:[1, 100]; Format:8.0; */
    td_u8   bright_throw_ratio;  /* R; Range:[1, 100]; Format:8.0; */
    td_u32  bright_compensation; /* R; Range:[0, 16383]; Format:32.0; */
    td_u32  isp_dgain;          /* R; Range:[0x400, 0x40000]; Format:22.10; */
    td_u32  blc;                 /* R; Range:[0, 16383]; Format:32.0; */
    td_u32  iso;                 /* R; Range:[0x64, 0xFFFFFFFF]; Format:32.0; */
    td_u16  tone_mapping_value[OT_ISP_DRC_TM_NODE_NUM]; /* R; Range:[0, 65535]; Format:16.0; */
} ot_isp_thermo_info;

typedef struct {
    td_u16      clip;                                       /* RW; Range:[1, 4096] */
    td_u8       fusion_ratio;                               /* RW; Range:[0, 100] */
    td_u16      fusion_curve_value[OT_ISP_GAMMA_NODE_NUM];  /* RW; Range:[0, 4095] */
} ot_isp_thermo_curve_attr;

typedef struct {
    td_u32 int_time;
    td_u8 run_interval;
    td_u32 iso;
    td_u32 isp_dgain;
    td_u32 dc;
    td_u16 tone_mapping_value[OT_ISP_DRC_TM_NODE_NUM];
} ot_isp_thermo_result;

typedef struct {
    td_u32 blc;               /* RW; Range:[0, 16383]; Format:32.0; */
    td_u32 isp_dgain;         /* RW; Range:[0x400, 0x40000]; Format:22.10; */
} ot_isp_thermo_manual_attr;

typedef struct {
    td_bool    bypass;              /* RW; Range:[0, 1]; Format:1.0; */
    ot_op_mode op_type;
    ot_isp_thermo_manual_attr manual_attr;
    td_u8      thermo_run_interval; /* RW; Range:[1, 255]; Format:8.0; */
    td_u8      dark_throw_ratio;    /* RW; Range:[1, 100]; Format:8.0; */
    td_u8      bright_throw_ratio;  /* RW; Range:[1, 100]; Format:8.0; */
    td_u32     bright_compensation; /* RW; Range:[0, 16383]; Format:32.0; */
} ot_isp_thermo_attr;

/* the final calculate of ae alg */
#define AE_INT_TIME_NUM          4
typedef struct {
    td_u32  int_time[AE_INT_TIME_NUM];
    td_u32  isp_dgain;
    td_u32  again;
    td_u32  dgain;
    td_u32  iso;
    td_u32  isp_dgain_sf;
    td_u32  again_sf;
    td_u32  dgain_sf;
    td_u32  iso_sf;
    td_u8   ae_run_interval;

    td_bool piris_valid;
    td_s32  piris_pos;
    td_u32  piris_gain;
    td_u32  sns_lhcg_exp_ratio;

    ot_isp_fswdr_mode fswdr_mode;
    td_u32  wdr_gain[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  hmax_times; /* unit is ns */
    td_u32  vmax; /* unit is line */

    ot_isp_ae_stat_attr stat_attr;
    ot_isp_dcf_update_info update_info;
} ot_isp_ae_result;

typedef struct {
    td_u32 isp_dgain;
    td_u32 iso;
} ot_isp_ae_init_info;

typedef struct {
    td_s32 (*pfn_ae_init)(td_s32 handle, const ot_isp_ae_param *ae_param);
    td_s32 (*pfn_ae_run)(td_s32 handle,
                         const ot_isp_ae_info *ae_info,
                         ot_isp_ae_result *ae_result,
                         td_s32 reserved);
    td_s32 (*pfn_thermo_run)(td_s32 handle,
                         const ot_isp_ae_info *ae_info,
                         ot_isp_thermo_result *thermo_result,
                         td_s32 reserved);
    td_s32 (*pfn_ae_ctrl)(td_s32 handle, td_u32 cmd, td_void *value);
    td_s32 (*pfn_ae_exit)(td_s32 handle);
} ot_isp_ae_exp_func;

typedef struct {
    ot_isp_ae_exp_func ae_exp_func;
} ot_isp_ae_register;

/* AWB */
typedef enum {
    OT_ISP_AWB_CCM_CONFIG_SET,
    OT_ISP_AWB_CCM_CONFIG_GET,

    OT_ISP_AWB_DEBUG_ATTR_SET,

    OT_ISP_AWB_CTRL_BUTT,
} ot_isp_awb_ctrl_cmd;

typedef struct {
    td_bool awb_bypass_en;
    td_bool manual_sat_en;
    td_bool manual_temp_en;

    td_u32  manual_sat_value;
    td_u32  manual_temp_value;
    td_u16  ccm_speed;

    td_u16  high_ccm[OT_ISP_CCM_MATRIX_SIZE];
    td_u16  low_ccm[OT_ISP_CCM_MATRIX_SIZE];
    td_u16  high_color_temp;
    td_u16  low_color_temp;
} awb_ccm_config;

/* the init param of awb alg */
typedef struct {
    ot_sensor_id sns_id;
    td_u8 wdr_mode;
    td_u8 awb_zone_row;
    td_u8 awb_zone_col;
    td_u8 awb_zone_bin;
    ot_isp_stitch_attr stitch_attr; /* Not support for Hi3516CV610 */
    td_u16 awb_width;
    td_u16 awb_height;
    td_u32 init_iso;
    td_s8 reserved;
} ot_isp_awb_param;

/* the statistics of awb alg */
typedef struct {
    td_u16  metering_awb_avg_r;
    td_u16  metering_awb_avg_g;
    td_u16  metering_awb_avg_b;
    td_u16  metering_awb_count_all;
} ot_isp_awb_stat_1;

typedef struct {
    td_u16  metering_mem_array_avg_r[OT_ISP_AWB_ZONE_NUM];
    td_u16  metering_mem_array_avg_g[OT_ISP_AWB_ZONE_NUM];
    td_u16  metering_mem_array_avg_b[OT_ISP_AWB_ZONE_NUM];
    td_u16  metering_mem_array_count_all[OT_ISP_AWB_ZONE_NUM];
} ot_isp_awb_stat_2;

typedef struct {
    td_u16  zone_col;
    td_u16  zone_row;
    td_u16  zone_bin;
    td_u16  metering_mem_array_avg_r[OT_ISP_AWB_ZONE_STITCH_MAX];
    td_u16  metering_mem_array_avg_g[OT_ISP_AWB_ZONE_STITCH_MAX];
    td_u16  metering_mem_array_avg_b[OT_ISP_AWB_ZONE_STITCH_MAX];
    td_u16  metering_mem_array_count_all[OT_ISP_AWB_ZONE_STITCH_MAX];
} ot_isp_awb_stat_stitch; /* Not support for Hi3516CV610 */

typedef struct {
    td_u16 *zone_avg_r;
    td_u16 *zone_avg_g;
    td_u16 *zone_avg_b;
    td_u16 *zone_count;
} ot_isp_awb_stat_result;

typedef struct {
    td_u8 col_start;
    td_u8 col_stop;
    td_u8 row_start;
    td_u8 row_stop;
} ot_isp_awb_overlap_zone;

typedef struct {
    td_u8  stitch_pipe_num;
    ot_isp_awb_overlap_zone left_zone_range[OT_ISP_MAX_STITCH_NUM];
    ot_isp_awb_overlap_zone right_zone_range[OT_ISP_MAX_STITCH_NUM];
} ot_isp_awb_overlap_all_zone;

typedef struct {
    td_u32  frame_cnt;

    ot_isp_awb_stat_1 *awb_stat1;
    ot_isp_awb_stat_result awb_stat2;
    td_u8  awb_gain_switch;
    td_u8  awb_stat_switch;
    td_bool wb_gain_in_sns;
    td_u32 wdr_wb_gain[OT_ISP_BAYER_CHN_NUM];
    ot_isp_awb_overlap_all_zone overlap_all_zone;
} ot_isp_awb_info;

/* the statistics's attr of awb alg */
typedef struct {
    td_bool stat_cfg_update;

    td_u16  metering_white_level_awb;
    td_u16  metering_black_level_awb;
    td_u16  metering_cr_ref_max_awb;
    td_u16  metering_cb_ref_max_awb;
    td_u16  metering_cr_ref_min_awb;
    td_u16  metering_cb_ref_min_awb;
} ot_isp_awb_raw_stat_attr;

typedef struct {
    td_u32 left_overlap_zone_awb[OT_ISP_MAX_STITCH_NUM][OT_ISP_BAYER_CHN_NUM];
    td_u32 right_overlap_zone_awb[OT_ISP_MAX_STITCH_NUM][OT_ISP_BAYER_CHN_NUM];
} ot_isp_awb_overlap_result;

/* the final calculate of awb alg */
typedef struct {
    td_u32  white_balance_gain[OT_ISP_BAYER_CHN_NUM];
    td_u16  color_matrix[OT_ISP_CCM_MATRIX_SIZE];
    td_u32  color_temp;
    td_u8   saturation;
    td_u16  sector_matrix[OT_ISP_COLOR_SECTOR_NUM][OT_ISP_CCM_MATRIX_SIZE]; /* Not supported for Hi3519DV500 */
    ot_isp_awb_raw_stat_attr raw_stat_attr;
    ot_isp_awb_overlap_result overlap_result;
} ot_isp_awb_result;

typedef struct {
    td_s32 (*pfn_awb_init)(td_s32 handle, const ot_isp_awb_param *awb_param, ot_isp_awb_result *awb_result);
    td_s32 (*pfn_awb_run)(td_s32 handle,
                          const ot_isp_awb_info *awb_info,
                          ot_isp_awb_result *awb_result,
                          td_s32 reserved);
    td_s32 (*pfn_awb_ctrl)(td_s32 handle, td_u32 cmd, td_void *value);
    td_s32 (*pfn_awb_exit)(td_s32 handle);
} ot_isp_awb_exp_func;

typedef struct {
    ot_isp_awb_exp_func awb_exp_func;
} ot_isp_awb_register;

/* AF */
/* the statistics of af alg */
typedef struct {
    td_u16  v1;
    td_u16  h1;
    td_u16  v2;
    td_u16  h2;
    td_u16  y;
    td_u16  hl_cnt;
} ot_isp_af_zone;

typedef struct {
    ot_isp_af_zone zone_metrics[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AF_ZONE_ROW][OT_ISP_AF_ZONE_COLUMN]; /* R;
                                                                                       the zoned measure of contrast */
} ot_isp_fe_af_stat;

typedef struct {
    ot_isp_af_zone zone_metrics[OT_ISP_AF_ZONE_ROW][OT_ISP_AF_ZONE_COLUMN]; /* R; the zoned measure of contrast */
} ot_isp_be_af_stat;

#define ALG_LIB_NAME_SIZE_MAX    20
typedef struct {
    td_s32  id;
    td_char lib_name[ALG_LIB_NAME_SIZE_MAX];
} ot_isp_3a_alg_lib;

typedef struct {
    ot_sensor_id       sns_id;
    ot_isp_3a_alg_lib  ae_lib;
    ot_isp_3a_alg_lib  af_lib;
    ot_isp_3a_alg_lib  awb_lib;
} ot_isp_bind_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_COMM_3A_H */
