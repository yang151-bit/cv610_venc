#include "main.h"

// MIPI configuration profiles
combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_SC500AI_10BIT_5M_NOWDR_ATTR = {
  .devno = 0,
  .input_mode = INPUT_MODE_MIPI,
  .data_rate = MIPI_DATA_RATE_X1,
  .img_rect = {0, 0, 2880, 1620},
  {
    .mipi_attr =
    {
      DATA_TYPE_RAW_10BIT,
      OT_MIPI_WDR_MODE_NONE,
        {0, 1, 2, 3}
    }
  }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_SC500AI_10BIT_2M_NOWDR_ATTR = {
  .devno = 0,
  .input_mode = INPUT_MODE_MIPI,
  .data_rate = MIPI_DATA_RATE_X1,
  .img_rect = {0, 0, 1920, 1080},
  {
    .mipi_attr =
    {
      DATA_TYPE_RAW_10BIT,
      OT_MIPI_WDR_MODE_NONE,
        {0, 1, 2, 3}
    }
  }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_SC500AI_10BIT_1M_NOWDR_ATTR = {
  .devno = 0,
  .input_mode = INPUT_MODE_MIPI,
  .data_rate = MIPI_DATA_RATE_X1,
  .img_rect = {0, 0, 1440, 800},
  {
    .mipi_attr =
    {
      DATA_TYPE_RAW_10BIT,
      OT_MIPI_WDR_MODE_NONE,
        {0, 1, -1, -1}
    }
  }
};