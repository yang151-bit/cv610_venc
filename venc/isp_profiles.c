#include "main.h"

// ISP Configuration profiles
ot_isp_pub_attr ISP_PROFILE_SC500AI_MIPI = {
    {0, 0, 1920, 1080},
    { 1920, 1080 },
    60,
    OT_ISP_BAYER_BGGR,
    OT_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 1920, 1080},
    },
};