/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_DEFINES_H
#define OT_DEFINES_H

#include "autoconf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef OT_CHIP
#define OT_CHIP                          CONFIG_OT_SUBCHIP_TYPE
#endif

#define OT_CHIP_HI3516DV500                    0x3516D500
#define OT_CHIP_HI3519DV500                    0x3519D500
#define OT_CHIP_SD3403V100                     0xd3403100
#define OT_CHIP_SD3491V100                     0xd3491100
#define OT_CHIP_HI3516CV610                    0x3516C610

#if OT_CHIP == OT_CHIP_HI3516DV500
#define OT_CHIP_NAME                       "HI3516DV500"
#elif OT_CHIP == OT_CHIP_HI3519DV500
#define OT_CHIP_NAME                       "HI3519DV500"
#elif OT_CHIP == OT_CHIP_HI3516CV610
#define OT_CHIP_NAME                       "HI3516CV610"
#else
#error OT_CHIP define may be error
#endif

#define OT_MAX_BIND_DST_NUM                4
#define OT_MPP_VER_PRIX                    "_MPP_V"

#define OT_MAX_COLOR_COMPONENT             2
#define OT_ALIGN_NUM                       4
#define OT_MAX_PRIVATE_DATA_NUM            2
#define OT_MAX_USER_DATA_NUM               2
#define OT_LUMA_PHYS_ALIGN                  32

#define OT_DEFAULT_ALIGN                   8
#define OT_MAX_ALIGN                       1024
#define OT_SEG_COMPRESS_LEN                256

#define OT_MAX_MMZ_NAME_LEN                32

#define OT_QUAD_POINT_NUM                  4

/* For VB */
#define OT_VB_MAX_POOLS                  96

/* For VENC */
#define OT_VENC_VEDU_IP_NUM              1
#define OT_VENC_JPGE_IP_NUM              1
#define OT_VENC_MAX_TILE_NUM             1
#define OT_VENC_MAX_REG_NUM              2
#define OT_VENC_MAX_CHN_NUM              16
#define OT_VENC_H264_MAX_WIDTH           4096
#define OT_VENC_H264_MAX_HEIGHT          4096
#define OT_VENC_H264_MIN_WIDTH           114
#define OT_VENC_H264_MIN_HEIGHT          114
#define OT_VENC_H265_MAX_WIDTH           4096
#define OT_VENC_H265_MAX_HEIGHT          4096
#define OT_VENC_H265_MIN_WIDTH           114
#define OT_VENC_H265_MIN_HEIGHT          114
#define OT_VENC_JPEG_MAX_WIDTH           16384
#define OT_VENC_JPEG_MAX_HEIGHT          16384
#define OT_VENC_JPEG_MIN_WIDTH           32
#define OT_VENC_JPEG_MIN_HEIGHT          32
#define OT_VENC_MAX_INPUT_FRAME_RATE     240
#define OT_VENC_MIN_INPUT_FRAME_RATE     1
#define OT_VENC_MAX_RECV_SRC             4
#define OT_VENC_H265_ADAPTIVE_FRAME_TYPE 4
#define OT_VENC_H265_ADAPTIVE_QP_TYPE    5
#define OT_VENC_H265E_LCU_SIZE           32
#define OT_VENC_SVAC3_MAX_WIDTH           4096
#define OT_VENC_SVAC3_MAX_HEIGHT          4096
#define OT_VENC_SVAC3_MIN_WIDTH           114
#define OT_VENC_SVAC3_MIN_HEIGHT          114
#define OT_VENC_SVAC3_ADAPTIVE_FRAME_TYPE 4
#define OT_VENC_SVAC3_ADAPTIVE_QP_TYPE    5
#define OT_VENC_SVAC3E_LCU_SIZE           32
#define OT_VENC_MAX_IN_DEPTH              3

/* For RC */
#define OT_VENC_MAX_BITRATE              (160 * 1024)
#define OT_VENC_MIN_BITRATE              2

/* For VDEC */
#define OT_VDEC_MAX_CHN_NUM                8
#define OT_VFMW_MAX_CHN_NUM                8

#define OT_H264D_ALIGN_W                   64
#define OT_H264D_ALIGN_H                   16
#define OT_H265D_ALIGN_W                   64
#define OT_H265D_ALIGN_H                   64
#define OT_JPEGD_ALIGN_W                   64
#define OT_JPEGD_ALIGN_H                   16

#define OT_H264D_MAX_SPS                   32
#define OT_H264D_MIN_SPS                   1
#define OT_H264D_MAX_PPS                   256
#define OT_H264D_MIN_PPS                   1
#define OT_H264D_MAX_SLICE                 600
#define OT_H264D_MIN_SLICE                 1

#define OT_H265D_MAX_VPS                   16
#define OT_H265D_MIN_VPS                   1
#define OT_H265D_MAX_SPS                   16
#define OT_H265D_MIN_SPS                   1
#define OT_H265D_MAX_PPS                   64
#define OT_H265D_MIN_PPS                   1
#define OT_H265D_MAX_SLICE                 600
#define OT_H265D_MIN_SLICE                 1

#define OT_H264D_MAX_WIDTH                 6144
#define OT_H264D_MAX_HEIGHT                6144
#define OT_H264D_MIN_WIDTH                 114
#define OT_H264D_MIN_HEIGHT                114
#define OT_H265D_MAX_WIDTH                 6144
#define OT_H265D_MAX_HEIGHT                6144
#define OT_H265D_MIN_WIDTH                 114
#define OT_H265D_MIN_HEIGHT                114
#define OT_VDH_MIN_MSG_NUM                 1
#define OT_VDH_MAX_MSG_NUM                 8
#define OT_VEDU_MSG_NUM                    2

#define OT_JPEGD_IP_NUM                    1
#define OT_JPEGD_MAX_WIDTH                 16384
#define OT_JPEGD_MAX_HEIGHT                16384
#define OT_JPEGD_MIN_WIDTH                 8
#define OT_JPEGD_MIN_HEIGHT                8

/* For Region */
#define OT_RGN_MIN_WIDTH                   2
#define OT_RGN_MIN_HEIGHT                  2
#define OT_RGN_ALIGN                       2

#define OT_RGN_COVER_MIN_X                 (-4096)
#define OT_RGN_COVER_MIN_Y                 (-4096)
#define OT_RGN_COVER_MAX_X                 4094
#define OT_RGN_COVER_MAX_Y                 4094
#define OT_RGN_COVER_MAX_WIDTH             4096
#define OT_RGN_COVER_MAX_HEIGHT            4096
#define OT_RGN_COVER_MIN_THICK             2
#define OT_RGN_COVER_MAX_THICK             16
#define OT_RGN_COVER_THICK_ALIGN           2

#define OT_RGN_COVEREX_MIN_X               (-4096)
#define OT_RGN_COVEREX_MIN_Y               (-4096)
#define OT_RGN_COVEREX_MAX_X               4094
#define OT_RGN_COVEREX_MAX_Y               4094
#define OT_RGN_COVEREX_MAX_WIDTH           4096
#define OT_RGN_COVEREX_MAX_HEIGHT          4096
#define OT_RGN_COVEREX_MIN_THICK           2
#define OT_RGN_COVEREX_MAX_THICK           16
#define OT_RGN_COVEREX_THICK_ALIGN         2

#define OT_RGN_OVERLAY_MIN_X               0
#define OT_RGN_OVERLAY_MIN_Y               0
#define OT_RGN_OVERLAY_MAX_X               4094
#define OT_RGN_OVERLAY_MAX_Y               4094
#define OT_RGN_OVERLAY_MAX_WIDTH           4096
#define OT_RGN_OVERLAY_MAX_HEIGHT          4096
#define OT_RGN_OVERLAY_MIN_ALPHA           0
#define OT_RGN_OVERLAY_MAX_ALPHA           255
#define OT_RGN_OVERLAY_VENC_MAX_ALPHA      255

#define OT_RGN_OVERLAYEX_MIN_X             0
#define OT_RGN_OVERLAYEX_MIN_Y             0
#define OT_RGN_OVERLAYEX_MAX_X             4094
#define OT_RGN_OVERLAYEX_MAX_Y             4094
#define OT_RGN_OVERLAYEX_MAX_WIDTH         4096
#define OT_RGN_OVERLAYEX_MAX_HEIGHT        4096
#define OT_RGN_OVERLAYEX_MIN_ALPHA         0
#define OT_RGN_OVERLAYEX_MAX_ALPHA         255

#define OT_RGN_LINEEX_MIN_X                (-4096)
#define OT_RGN_LINEEX_MIN_Y                (-4096)
#define OT_RGN_LINEEX_MAX_X                4094
#define OT_RGN_LINEEX_MAX_Y                4094
#define OT_RGN_LINEEX_MIN_THICK            2
#define OT_RGN_LINEEX_MAX_THICK            16

#define OT_RGN_CORNER_RECT_MIN_X           (-4096)
#define OT_RGN_CORNER_RECT_MIN_Y           (-4096)
#define OT_RGN_CORNER_RECT_MAX_X           4094
#define OT_RGN_CORNER_RECT_MAX_Y           4094
#define OT_RGN_CORNER_RECT_MIN_THICK       2
#define OT_RGN_CORNER_RECT_MAX_THICK       16
#define OT_RGN_CORNER_RECT_THICK_ALIGN     2

#define OT_RGN_CORNER_RECT_MAX_WIDTH       4096
#define OT_RGN_CORNER_RECT_MAX_HEIGHT      4096
#define OT_RGN_CORNER_RECT_MIN_HOR_LEN     2
#define OT_RGN_CORNER_RECT_MAX_HOR_LEN     4096
#define OT_RGN_CORNER_RECT_MIN_VER_LEN     2
#define OT_RGN_CORNER_RECT_MAX_VER_LEN     4096

#define OT_RGN_CORNER_RECTEX_MIN_X         (-4096)
#define OT_RGN_CORNER_RECTEX_MIN_Y         (-4096)
#define OT_RGN_CORNER_RECTEX_MAX_X         4094
#define OT_RGN_CORNER_RECTEX_MAX_Y         4094
#define OT_RGN_CORNER_RECTEX_MIN_THICK     2
#define OT_RGN_CORNER_RECTEX_MAX_THICK     16
#define OT_RGN_CORNER_RECTEX_THICK_ALIGN   2

#define OT_RGN_CORNER_RECTEX_MAX_WIDTH     4096
#define OT_RGN_CORNER_RECTEX_MAX_HEIGHT    4096
#define OT_RGN_CORNER_RECTEX_MIN_HOR_LEN   2
#define OT_RGN_CORNER_RECTEX_MAX_HOR_LEN   4096
#define OT_RGN_CORNER_RECTEX_MIN_VER_LEN   2
#define OT_RGN_CORNER_RECTEX_MAX_VER_LEN   4096

#define OT_RGN_HANDLE_MAX                  128
#define OT_RGN_MAX_BUF_NUM                 2

#define OT_RGN_VI_MAX_COVEREX_NUM          16

#define OT_RGN_VENC_MAX_OVERLAY_NUM        8

#define OT_RGN_VPSS_MAX_COVER_NUM          8
#define OT_RGN_VPSS_MAX_RECT_COVER_NUM     8
#define OT_RGN_VPSS_MAX_QUAD_COVER_NUM     0
#define OT_RGN_VPSS_MAX_COVEREX_NUM        8
#define OT_RGN_VPSS_MAX_OVERLAYEX_NUM      8
#define OT_RGN_VPSS_MAX_LINEEX_NUM         40
#define OT_RGN_VPSS_MAX_CORNER_RECT_NUM    4
#define OT_RGN_VPSS_MAX_CORNER_RECTEX_NUM  4

#define OT_RGN_VGS_MAX_TASK_WIDTH          8192

/* For vi */
#define OT_VI_MAX_DEV_NUM                 2
#define OT_VI_MAX_PHYS_PIPE_NUM           2
#define OT_VI_MAX_VIRT_PIPE_NUM           2
#define OT_VI_MAX_PIPE_NUM                (OT_VI_MAX_PHYS_PIPE_NUM + OT_VI_MAX_VIRT_PIPE_NUM)
#define OT_VI_MAX_STITCH_GRP_NUM          2
#define OT_VI_MAX_WDR_FUSION_GRP_NUM      OT_VI_MAX_PIPE_NUM
#define OT_VI_MAX_DISTRIBUTE_GRP_NUM      2
#define OT_VI_MAX_DISTRIBUTE_PIPE_NUM     2
#define OT_VI_MAX_WDR_NUM                 2
#define OT_VI_MAX_WDR_FRAME_NUM           2
#define OT_VI_MAX_PHYS_CHN_NUM            1
#define OT_VI_MAX_EXT_CHN_NUM             1
#define OT_VI_EXT_CHN_START               OT_VI_MAX_PHYS_CHN_NUM
#define OT_VI_MAX_CHN_NUM                 (OT_VI_MAX_PHYS_CHN_NUM + OT_VI_MAX_EXT_CHN_NUM)
#define OT_VI_MAX_EXTCHN_BIND_PER_CHN     8

#define OT_VI_MAX_SPLIT_NODE_NUM          2

#define OT_VI_MIN_BNR_BUF_NUM             2
#define OT_VI_MAX_BNR_BUF_NUM             128

#define OT_VI_DEV_MIN_WIDTH               120
#define OT_VI_DEV_MIN_HEIGHT              80
#define OT_VI_DEV_MAX_WIDTH               4096
#define OT_VI_DEV_MAX_HEIGHT              4096

#define OT_VI_PIPE_OFFLINE_MIN_WIDTH      120
#define OT_VI_PIPE_OFFLINE_MIN_HEIGHT     80
#define OT_VI_PIPE_OFFLINE_RAW_MIN_HEIGHT 120
#define OT_VI_PIPE_OFFLINE_MAX_WIDTH      4096
#define OT_VI_PIPE_OFFLINE_MAX_HEIGHT     4096

#define OT_VI_PIPE_ONLINE_MIN_WIDTH       120
#define OT_VI_PIPE_ONLINE_MIN_HEIGHT      80
#define OT_VI_PIPE_ONLINE_RAW_MIN_HEIGHT  120
#define OT_VI_PIPE_ONLINE_MAX_WIDTH       3200
#define OT_VI_PIPE_ONLINE_MAX_HEIGHT      4096

#define OT_VI_PHYS_CHN_OFFLINE_MIN_WIDTH  120
#define OT_VI_PHYS_CHN_OFFLINE_MIN_HEIGHT 80
#define OT_VI_PHYS_CHN_OFFLINE_MAX_WIDTH  4096
#define OT_VI_PHYS_CHN_OFFLINE_MAX_HEIGHT 4096

#define OT_VI_PHYS_CHN_ONLINE_MIN_WIDTH   120
#define OT_VI_PHYS_CHN_ONLINE_MIN_HEIGHT  80
#define OT_VI_PHYS_CHN_ONLINE_MAX_WIDTH   3200
#define OT_VI_PHYS_CHN_ONLINE_MAX_HEIGHT  4096

#define OT_VI_EXT_CHN_MIN_WIDTH           32
#define OT_VI_EXT_CHN_MIN_HEIGHT          32
#define OT_VI_EXT_CHN_MAX_WIDTH           4096
#define OT_VI_EXT_CHN_MAX_HEIGHT          4096
#define OT_VI_EXT_CHN_MAX_ZOOMIN          16
#define OT_VI_EXT_CHN_MAX_ZOOMOUT         30

#define OT_VI_COMPRESS_PARAM_SIZE         152

/* For DIS */
#define OT_DIS_PYRAMID_LAYER_NUM          5
#define OT_DIS_MAX_CHN_NUM                4
#define OT_DIS_MAX_IMAGE_WIDTH            8192
#define OT_DIS_MIN_IMAGE_WIDTH            120
#define OT_DIS_MAX_IMAGE_HEIGHT           8192
#define OT_DIS_MIN_IMAGE_HEIGHT           88

/* For VO */
#define OT_VO_MAX_PHYS_DEV_NUM            1  /* max physical dev num */
#define OT_VO_MAX_VIRT_DEV_NUM            0  /* max virtual dev num */
#define OT_VO_MAX_CAS_DEV_NUM             0  /* max cascade dev num */
/* max dev num */
#define OT_VO_MAX_DEV_NUM                 (OT_VO_MAX_PHYS_DEV_NUM + OT_VO_MAX_VIRT_DEV_NUM + OT_VO_MAX_CAS_DEV_NUM)

#define OT_VO_MAX_PHYS_VIDEO_LAYER_NUM    1 /* max physical video layer num */
#define OT_VO_MAX_GFX_LAYER_NUM           1 /* max graphic layer num */
/* max physical layer num */
#define OT_VO_MAX_PHYS_LAYER_NUM          (OT_VO_MAX_PHYS_VIDEO_LAYER_NUM + OT_VO_MAX_GFX_LAYER_NUM)
/* max layer num */
#define OT_VO_MAX_LAYER_NUM               (OT_VO_MAX_PHYS_LAYER_NUM + OT_VO_MAX_VIRT_DEV_NUM + OT_VO_MAX_CAS_DEV_NUM)
#define OT_VO_MAX_LAYER_IN_DEV            2 /* max video layer num of each dev */

#define OT_VO_LAYER_V0                    0 /* video layer 0 */
#define OT_VO_LAYER_G0                    1 /* graphics layer 0 */
#define OT_VO_MAX_PRIORITY                1      /* max layer priority */
#define OT_VO_MIN_TOLERATE                1      /* min play toleration 1ms */
#define OT_VO_MAX_TOLERATE                100000 /* max play toleration 100s */

#define OT_VO_MAX_CHN_NUM                 4    /* max chn num */
#define OT_VO_MIN_CHN_WIDTH               32   /* channel minimal width */
#define OT_VO_MIN_CHN_HEIGHT              32   /* channel minimal height */
#define OT_VO_MAX_ZOOM_RATIO              1000 /* max zoom ratio, 1000 means 100% scale */
#define OT_VO_MAX_NODE_NUM                16   /* max node num */

#define OT_VO_MAX_WBC_NUM                 0     /* max wbc num */
#define OT_VO_MAX_IMG_WIDTH               16384 /* vo max img width */
#define OT_VO_MAX_IMG_HEIGHT              8192  /* vo max img height */

/* For AVS */
#define OT_AVS_MAX_GRP_NUM                32
#define OT_AVS_PIPE_NUM                   4
#define OT_AVS_SPLIT_PIPE_NUM             4
#define OT_AVS_MAX_CHN_NUM                2
#define OT_AVS_SPLIT_NUM                  2
#define OT_AVS_CUBE_MAP_SURFACE_NUM       6

#define OT_AVS_BLEND_MIN_IN_WIDTH         1280
#define OT_AVS_BLEND_MIN_IN_HEIGHT        720
#define OT_AVS_NO_BLEND_MIN_IN_WIDTH      64
#define OT_AVS_NO_BLEND_MIN_IN_HEIGHT     64
#define OT_AVS_MAX_IN_WIDTH               8192
#define OT_AVS_MAX_IN_HEIGHT              8192

#define OT_AVS_MIN_OUT_WIDTH              256
#define OT_AVS_MIN_OUT_HEIGHT             256
#define OT_AVS_MAX_OUT_WIDTH              16384
#define OT_AVS_MAX_OUT_HEIGHT             8192

/* For AUDIO */
#define OT_AI_DEV_MAX_NUM                  1
#define OT_AO_DEV_MIN_NUM                  0
#define OT_AO_DEV_MAX_NUM                  1
#define OT_AIO_MAX_NUM           ((OT_AO_DEV_MAX_NUM > OT_AI_DEV_MAX_NUM) ? OT_AO_DEV_MAX_NUM : OT_AI_DEV_MAX_NUM)
#define OT_AENC_MAX_CHN_NUM                8
#define OT_ADEC_MAX_CHN_NUM                8

#define OT_AI_MAX_CHN_NUM                  4
#define OT_AO_MAX_CHN_NUM                  3
#define OT_AO_SYS_CHN_ID                  (OT_AO_MAX_CHN_NUM - 1)

#define OT_AIO_MAX_CHN_NUM       ((OT_AO_MAX_CHN_NUM > OT_AI_MAX_CHN_NUM) ? OT_AO_MAX_CHN_NUM : OT_AI_MAX_CHN_NUM)

/* For VPSS */
#define OT_VPSS_MAX_GRP_NUM             6
#define OT_VPSS_MAX_GRP_PIPE_NUM        1
#define OT_VPSS_VGS_GRP_NO              6
#define OT_VPSS_IP_NUM                  1
#define OT_VPSS0                        0
#define OT_VPSS_MAX_PHYS_CHN_NUM        3
#define OT_VPSS_MAX_EXT_CHN_NUM         2
#define OT_VPSS_MAX_CHN_NUM             (OT_VPSS_MAX_PHYS_CHN_NUM + OT_VPSS_MAX_EXT_CHN_NUM)
#define OT_VPSS_MAX_SPLIT_NODE_NUM      2
#define OT_VPSS_MAX_STITCH_BUF_NUM      5
#define OT_VPSS_MAX_FPS                 240
#define OT_VPSS_USER_LIST_MAX_DEPTH     8
#define OT_VPSS_MAX_OUT_RGN_NUM         1
#define OT_VPSS_LOW_DELAY_MIN_LINE      16
#define OT_VPSS_MIN_IMG_WIDTH           64
#define OT_VPSS_MIN_IMG_HEIGHT          64
#define OT_VPSS_MAX_IN_IMG_WIDTH        4096
#define OT_VPSS_MAX_IN_IMG_HEIGHT       4096
#define OT_VPSS_MAX_OUT_IMG_WIDTH       4096
#define OT_VPSS_MAX_OUT_IMG_HEIGHT      4096
#define OT_VPSS_MAX_SEG_COMPACT_WIDTH   4096
#define OT_VPSS_MAX_ZOOMIN              16
#define OT_VPSS_MAX_ZOOMOUT             15
#define OT_VPSS_EXT_CHN_MAX_ZOOMIN      16
#define OT_VPSS_EXT_CHN_MAX_ZOOMOUT     30
#define OT_VPSS_EXT_CHN_MAX_IMG_WIDTH   4096
#define OT_VPSS_EXT_CHN_MAX_IMG_HEIGHT  4096

/* For PCIV */
#define OT_PCIV_MAX_CHN_NUM                16 /* max pciv channel number in each pciv device */

/* For IVS_MD */
#define OT_MD_MAX_CHN                      4
#define OT_MD_MAX_WIDTH                    1920
#define OT_MD_MAX_HEIGHT                   1080
#define OT_MD_MIN_WIDTH                    64
#define OT_MD_MIN_HEIGHT                   64

/* For IVE */
#define OT_IVE_MAX_ARR_LEN                   512
#define OT_IVE_MIN_ARR_LEN                   20
#define OT_IVE_DEFAULT_NODE_NUM              64

/* For RECT */
#define OT_DPU_RECT_MAX_GRP_NUM           8
#define OT_DPU_RECT_MAX_PIPE_NUM          2
#define OT_DPU_RECT_MAX_CHN_NUM           2

#define OT_DPU_RECT_IN_IMAGE_MAX_WIDTH    2048
#define OT_DPU_RECT_IN_IMAGE_MAX_HEIGHT   2048
#define OT_DPU_RECT_IN_IMAGE_MIN_WIDTH    64
#define OT_DPU_RECT_IN_IMAGE_MIN_HEIGHT   64
#define OT_DPU_RECT_OUT_IMAGE_MAX_WIDTH   2048
#define OT_DPU_RECT_OUT_IMAGE_MAX_HEIGHT  2048
#define OT_DPU_RECT_OUT_IMAGE_MIN_WIDTH   64
#define OT_DPU_RECT_OUT_IMAGE_MIN_HEIGHT  64

/* For MATCH */
#define OT_DPU_MATCH_MAX_GRP_NUM          8
#define OT_DPU_MATCH_MAX_PIPE_NUM         2
#define OT_DPU_MATCH_MAX_CHN_NUM          1

#define OT_DPU_MATCH_IN_IMAGE_MAX_WIDTH   2048
#define OT_DPU_MATCH_IN_IMAGE_MAX_HEIGHT  2048
#define OT_DPU_MATCH_IN_IMAGE_MIN_WIDTH   64
#define OT_DPU_MATCH_IN_IMAGE_MIN_HEIGHT  64
#define OT_DPU_MATCH_OUT_IMAGE_MAX_WIDTH  2048
#define OT_DPU_MATCH_OUT_IMAGE_MAX_HEIGHT 2048
#define OT_DPU_MATCH_OUT_IMAGE_MIN_WIDTH  64
#define OT_DPU_MATCH_OUT_IMAGE_MIN_HEIGHT 64

/* For NPU */
#define OT_SVP_NPU_MAX_STREAM_NUM       4
#define OT_SVP_NPU_MIN_STREAM_NUM       1
#define OT_SVP_NPU_DEF_STREAM_NUM       4
#define OT_SVP_NPU_MAX_TASK_NODE_NUM    512
#define OT_SVP_NPU_MIN_TASK_NODE_NUM    2
#define OT_SVP_NPU_DEF_TASK_NODE_NUM    32
#define OT_SVP_NPU_MAX_EVENT_NUM        0
#define OT_SVP_NPU_MIN_EVENT_NUM        0
#define OT_SVP_NPU_DEF_EVENT_NUM        0
#define OT_SVP_NPU_MAX_PROF_MEM_KB      1048576
#define OT_SVP_NPU_MIN_PROF_MEM_KB      512
#define OT_SVP_NPU_DEF_PROF_MEM_KB      2048
#define OT_SVP_NPU_DEF_OPS_SEL          3

/* For VGS */
#define OT_VGS_IP_NUM                      1
#define OT_VGS0                            0
#define OT_VGS_MAX_CHN_NUM                 1
#define OT_VGS_MAX_CORNER_RECT_NUM         1
#define OT_VGS_MAX_SOLID_RECT_COVER_NUM    1
#define OT_VGS_MAX_HOLLOW_RECT_COVER_NUM   OT_VGS_MAX_CORNER_RECT_NUM
#define OT_VGS_MAX_RECT_COVER_NUM          (OT_VGS_MAX_SOLID_RECT_COVER_NUM + OT_VGS_MAX_HOLLOW_RECT_COVER_NUM)
#define OT_VGS_MAX_QUAD_COVER_NUM          1
#define OT_VGS_MAX_COVER_NUM               OT_VGS_MAX_SOLID_RECT_COVER_NUM
#define OT_VGS_MAX_OSD_NUM                 1
#define OT_VGS_MAX_MOSAIC_NUM              1

#define OT_VGS_DEFAULT_JOB_NUM             64
#define OT_VGS_DEFAULT_TASK_NUM            100
#define OT_VGS_DEFAULT_NODE_NUM            100

#define OT_VGS_MAX_JOB_NUM                 200
#define OT_VGS_MIN_JOB_NUM                 20
#define OT_VGS_MAX_TASK_NUM                400
#define OT_VGS_MIN_TASK_NUM                20
#define OT_VGS_MAX_NODE_NUM                400
#define OT_VGS_MIN_NODE_NUM                20

/* For TDE */
#define OT_TDE_MAX_NODE_NUM               500
#define OT_TDE_MIN_NODE_NUM                2
#define OT_TDE_DEFAULT_NODE_NUM           100

/* For GDC */
#define OT_FISHEYE_MIN_IN_IMAGE_WIDTH     352
#define OT_FISHEYE_MIN_IN_IMAGE_HEIGHT    288
#define OT_FISHEYE_MIN_OUT_IMAGE_WIDTH    352
#define OT_FISHEYE_MIN_OUT_IMAGE_HEIGHT   288

#define OT_LDC_MIN_IMAGE_WIDTH            120
#define OT_LDC_MIN_IMAGE_HEIGHT           88

#define OT_SPREAD_MIN_IMAGE_WIDTH         120
#define OT_SPREAD_MIN_IMAGE_HEIGHT        88

#define OT_ROTATION_EX_MIN_IMAGE_WIDTH    120
#define OT_ROTATION_EX_MIN_IMAGE_HEIGHT   88

#define OT_PMF_MIN_IMAGE_WIDTH            120
#define OT_PMF_MIN_IMAGE_HEIGHT           88

#define OT_GDC_MAX_IMAGE_WIDTH            8192
#define OT_GDC_MAX_IMAGE_HEIGHT           8192
#define OT_GDC_IP_NUM                     1

#define OT_GDC_MAX_JOB_NUM              100  /* gdc max job number */
#define OT_GDC_MIN_JOB_NUM              4   /* gdc min job number */

#define OT_GDC_MAX_TASK_NUM             200  /* gdc max task number */
#define OT_GDC_MIN_TASK_NUM             4   /* gdc min task number */

#define OT_GDC_MAX_NODE_NUM             200  /* gdc max node number */
#define OT_GDC_MIN_NODE_NUM             4   /* gdc min node number */

#define OT_GDC_DEFAULT_JOB_NUM          16
#define OT_GDC_DEFAULT_TASK_NUM         50
#define OT_GDC_DEFAULT_NODE_NUM         50

/* For MCF */
#define OT_MCF_PIPE_NUM                   2
#define OT_MCF_MAX_GRP_NUM                2
#define OT_MCF_MAX_PHYS_CHN_NUM           3
#define OT_MCF_MAX_EXT_CHN_NUM            4
#define OT_MCF_MAX_CHN_NUM                (OT_MCF_MAX_PHYS_CHN_NUM + OT_MCF_MAX_EXT_CHN_NUM)

#define OT_MCF_MAX_PIPE_WIDTH        8192
#define OT_MCF_MAX_PIPE_HEIGHT       4096
#define OT_MCF_MIN_PIPE_WIDTH        256
#define OT_MCF_MIN_PIPE_HEIGHT       256

#define OT_MCF_MAX_CHN_WIDTH        8192
#define OT_MCF_MAX_CHN_HEIGHT       8192
#define OT_MCF_MIN_CHN_WIDTH        128
#define OT_MCF_MIN_CHN_HEIGHT       64

#define OT_MCF_MAX_EXT_CHN_WIDTH        8192
#define OT_MCF_MAX_EXT_CHN_HEIGHT       8192
#define OT_MCF_MIN_EXT_CHN_WIDTH        64
#define OT_MCF_MIN_EXT_CHN_HEIGHT       64

/* For UVC */
#define OT_UVC_MAX_CHN_NUM                1 /* max chn num */

#define OT_AIISP_PIPE_MIN_WIDTH      120
#define OT_AIISP_PIPE_MIN_HEIGHT     88
#define OT_AIISP_PIPE_MAX_WIDTH      4096
#define OT_AIISP_PIPE_MAX_HEIGHT     4096

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OT_DEFINES_H */
