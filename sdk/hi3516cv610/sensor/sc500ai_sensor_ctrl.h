/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SC500AI_SENSOR_CTRL_H
#define SC500AI_SENSOR_CTRL_H

#include "sensor_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 sc500ai_linear_5m30_10bit_init(cis_info *cis);
td_s32 sc500ai_linear_1080p100_10bit_init(cis_info *cis);
td_s32 sc500ai_linear_800p90_10bit_init(cis_info *cis);
td_s32 sc500ai_vc_wdr_2t1_5m30_10bit_init(cis_info *cis);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
