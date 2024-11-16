#include "main.h"

/**
 * @brief Set MIPI HS mode
 * @param device - MIPI device handle
 * @param mode - New mode
 */
int mipi_set_hs_mode(int device, lane_divide_mode_t mode) {
  return ioctl(device, OT_MIPI_SET_HS_MODE, &mode);
}

/**
 * @brief Enable MIPI clock
 * @param device - MI
 * @param device_id - Device identifier
 * @param enable -
 */
int mipi_enable_clock(int device, uint32_t device_id, int enable) {
  return ioctl(device, enable ? OT_MIPI_ENABLE_MIPI_CLOCK : OT_MIPI_DISABLE_MIPI_CLOCK, &device_id);
}

/**
 * @brief Set MIPI reset state
 * @param device
 * @param device_id
 * @param enable - Activate reset state
 */
int mipi_set_reset(int device, uint32_t device_id, int enable) {
  return ioctl( device, enable ? OT_MIPI_RESET_MIPI : OT_MIPI_UNRESET_MIPI, &device_id);
}

/**
 * @brief Enable MIPI sensor clock
 * @param device
 * @param sensor_id
 * @param enable
 */
int mipi_enable_sensor_clock(
  int device, sns_clk_source_t sensor_id, int enable) {
  return ioctl(device, enable ? OT_MIPI_ENABLE_SENSOR_CLOCK : OT_MIPI_DISABLE_SENSOR_CLOCK, &sensor_id);
}

/**
 * @brief Set sensor reset state
 * @param device
 * @param sensor_id
 * @param enable
 */
int mipi_set_sensor_reset(int device, sns_clk_source_t sensor_id, int enable) {
  return ioctl(device, enable ? OT_MIPI_RESET_SENSOR : OT_MIPI_UNRESET_SENSOR, &sensor_id);
}

/**
 * @brief Configure MIPI
 * @param device -
 * @param config - Pointer to MIPI configuration
 */
int mipi_configure(int device, combo_dev_attr_t* config) {
  return ioctl(device, OT_MIPI_SET_DEV_ATTR, config);
}
