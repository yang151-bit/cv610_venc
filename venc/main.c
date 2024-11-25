#include "main.h"
#include <stdbool.h>
#include <signal.h>
#include <time.h>

// Configuration profiles
extern combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_SC500AI_10BIT_2M_NOWDR_ATTR;
extern combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_SC500AI_10BIT_1M_NOWDR_ATTR;
extern combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_SC500AI_10BIT_5M_NOWDR_ATTR;
extern ot_vi_dev_attr DEV_ATTR_SC500AI_BASE;
extern ot_vi_pipe_attr PIPE_ATTR_RAW12_420;
extern ot_vi_pipe_attr PIPE_ATTR_RAW10_420;
extern ot_vi_chn_attr CHN_ATTR_420_SDR8_LINEAR;
extern ot_3dnr_attr PIPE_3DNR_ATTR_NORM;
extern ot_isp_pub_attr ISP_PROFILE_SC500AI_MIPI;

uint8_t* tx_buffer;
uint32_t tx_buffer_used = 0;

void printHelp() {
  printf(
    "\n\t\tOpenIPC FPV Streamer for HiSilicon (%s)\n"
    "\n"
    "  Usage:\n"
    "    venc [Arguments]\n"
    "\n"
    "  Arguments:\n"
    "    -v [Version]   - Camera version                  (Default: "
    "610_sc500)\n"
    "\n"
    "      HI3516cv610 / SC500AI\n"
    "        610_sc500F  - v200, IMX307, 4-lane MIPI | 1080p  | any fps\n"
    "        610_sc500B  - v200, IMX307, 2-lane MIPI | 800p   | any fps\n"
    "        610_sc500F5 - v200, IMX307, 4-lane MIPI | 2880x1620   | 30  fps only\n"
    "\n"
    "    -h [IP]        - Sink IP address                 (Default: "
    "127.0.0.1)\n"
    "    -p [Port]      - Sink port                       (Default: 5000)\n"
    "    -r [Rate]      - Max video rate in Kbit/sec.     (Default: 8192)\n"
    "    -n [Size]      - Max payload frame size in bytes (Default: 1400)\n"
    "    -m [Mode]      - Streaming mode                  (Default: "
    "compact)\n"
    "       compact       - Compact UDP stream \n"
    "       rtp           - RTP stream\n"
    "\n"
    "    -s [Size]      - Encoded image size              (Default: "
    "version specific)\n"
    "\n"
    "      Standard resolutions\n"
    "        D1           - 720  x 480\n"
    "        960h         - 960  x 576\n"
    "        720p         - 1280 x 720\n"
    "        1.3MP        - 1280 x 1024\n"
    "        1080p        - 1920 x 1080\n"
    "        4MP          - 2592 x 1520\n"
    "\n"
    "      Custom resolution format\n"
    "        WxH          - Custom resolution W x H pixels\n"
    "\n"
    "    -f [FPS]       - Encoder FPS (25,30,50,60)       (Default: 60)\n"
    "    -g [Value]     - GOP denominator                 (Default: 10)\n"
    "    -c [Codec]     - Encoder mode                    (Default: "
    "264avbr)\n"
    "\n"
    "           --- H264 ---\n"
    "       264avbr       - h264 AVBR\n"
    "       264qvbr       - h264 QVBR\n"
    "       264vbr        - h264 VBR\n"
    "       264cbr        - h264 CBR\n"
    "\n"
    "           --- H265 ---\n"
    "       265avbr       - h265 AVBR\n"
    "       265qvbr       - h265 QVBR\n"
    "       265vbr        - h265 VBR\n"
    "       265cbr        - h265 CBR\n"
    "\n"
    "    -d [Format]    - Data format                       (Default: "
    "stream)\n"
    "      stream         - Produce NALUs in stream mode\n"
    "      frame          - Produce NALUs in packet mode\n"
    "\n"
    "    --no-slices          - Disable slices\n"
    "    --slice-size [size]  - Slices size in lines      (Default: 4)\n"
    "\n"
    "    --low-delay    - Enable low delay mode\n"
    "    --mirror       - Mirror image\n"
    "    --flip         - Flip image\n"
    "    --exp          - Limit exposure\n"
    "\n"
    "    --roi          - Enable ROI\n"
    "    --roi-qp [QP]  - ROI quality points              (Default: 20)\n"
    "\n", __DATE__
  );
}

uint8_t stream_mode = 0;
uint16_t hisi_version = 610;
SensorType sensor_type = SC500AI;
uint32_t sensor_width = 1920;
uint32_t sensor_height = 1080;
uint32_t sensor_framerate = 60;
bool loop_running = true;

static void handler(int value) {
  loop_running = false;
}

int main(int argc, const char* argv[]) {
  if (argc == 2 && !strcmp(argv[1], "help")) {
    printHelp();
    return 1;
  }

  uint32_t mipi_device_id = 0;
  uint32_t mipi_sensor_id = 0;
  uint32_t isp_framerate = 60;
  
  uint32_t image_width = 1920; // Encoded image width
  uint32_t image_height = 1080; // Encoded image height

  ot_vi_dev vi_dev_id = 0;
  ot_vi_pipe vi_pipe_id = 0;
  ot_vi_chn vi_channel_id = 0;

  uint32_t vi_vpss_mode = OT_VI_ONLINE_VPSS_ONLINE;
  ot_vpss_grp vpss_group_id = 0;
  ot_vpss_chn vpss_first_ch_id = 0;
  ot_vpss_chn vpss_second_ch_id = 1;

  uint32_t venc_gop_denom = 10;
  uint32_t venc_gop_size = sensor_framerate / venc_gop_denom;
  uint32_t venc_max_rate = 1024 * 8;

  ot_venc_chn venc_first_ch_id = 0;
  td_bool venc_by_frame = TD_FALSE;
  uint32_t venc_slice_size = 4;

  uint32_t udp_sink_ip = inet_addr("127.0.0.1");
  uint16_t udp_sink_port = 5000;
  uint16_t max_frame_size = 1400;

  int enable_slices = 1;
  int enable_lowdelay = 0;
  int enable_roi = 0;
  bool limit_exposure = false;
  int ret = 0;

  int image_mirror = TD_FALSE;
  int image_flip = TD_FALSE;
  uint16_t roi_qp = 20;

  ot_payload_type rc_codec = OT_PT_H264;
  int rc_mode = OT_VENC_RC_MODE_H264_AVBR;

  // Load console arguments
  __BeginParseConsoleArguments__(printHelp) __OnArgument("-h") {
    udp_sink_ip = inet_addr(__ArgValue);
    continue;
  }

  __OnArgument("-p") {
    udp_sink_port = atoi(__ArgValue);
    continue;
  }

  __OnArgument("-r") {
    venc_max_rate = atoi(__ArgValue);
    continue;
  }

  __OnArgument("-n") {
    max_frame_size = atoi(__ArgValue);
    continue;
  }

  __OnArgument("-m") {
    const char* value = __ArgValue;
    if (!strcmp(value, "compact")) {
      stream_mode = 0;
    } else if (!strcmp(value, "rtp")) {
      stream_mode = 1;
    } else {
      printf("> ERROR: Unknown streaming mode\n");
      exit(1);
    }
    continue;
  }

  __OnArgument("-v") {
    const char* value = __ArgValue;
    // cv610 SC500AI 1080P
    if (!strcmp(value, "610_sc500F")) {
      hisi_version = 610;
      sensor_type = SC500AI;
      sensor_width = image_width = 1920;
      sensor_height = image_height = 1080;
      sensor_framerate = 60;
      isp_framerate = 60;
      vi_vpss_mode      = OT_VI_OFFLINE_VPSS_ONLINE;
    // cv610 SC500AI 1080P
    } else if (!strcmp(value, "610_sc500B")) {
      hisi_version = 610;
      sensor_type = SC500AI;
      sensor_width = image_width = 1440;
      sensor_height = image_height = 800;
      sensor_framerate = 90;
      isp_framerate = 90;
      vi_vpss_mode      = OT_VI_OFFLINE_VPSS_ONLINE;
    // - cv610 SC500AI 5MP
    } else if (!strcmp(value, "610_sc500F5")) {
      hisi_version = 610;
      sensor_type = SC500AI;
      sensor_width = image_width = 2880;
      sensor_height = image_height = 1620;
      sensor_framerate = 30;
      isp_framerate = 30;
      vi_vpss_mode      = OT_VI_OFFLINE_VPSS_ONLINE;

    } else {
      printf("> ERROR: Unknown version [%s]\n", value);
      exit(1);
    }
    continue;
  }

  __OnArgument("--no-slices") {
    enable_slices = 0;
    continue;
  }

  __OnArgument("--slice-size") {
    venc_slice_size = atoi(__ArgValue);
    continue;
  }

  __OnArgument("--low-delay") {
    enable_lowdelay = 1;
    continue;
  }

  __OnArgument("--mirror") {
    image_mirror = TD_TRUE;
    continue;
  }

  __OnArgument("--flip") {
    image_flip = TD_TRUE;
    continue;
  }

  __OnArgument("--exp") {
    limit_exposure = true;
    continue;
  }

  __OnArgument("-c") {
    const char* value = __ArgValue;
    if (!strcmp(value, "264avbr")) {
      rc_codec = OT_PT_H264;
      rc_mode = OT_VENC_RC_MODE_H264_AVBR;
    } else if (!strcmp(value, "264qvbr")) {
      rc_codec = OT_PT_H264;
      rc_mode = OT_VENC_RC_MODE_H264_QVBR;
    } else if (!strcmp(value, "264vbr")) {
      rc_codec = OT_PT_H264;
      rc_mode = OT_VENC_RC_MODE_H264_VBR;
    } else if (!strcmp(value, "264cbr")) {
      rc_codec = OT_PT_H264;
      rc_mode = OT_VENC_RC_MODE_H264_CBR;
    } else if (!strcmp(value, "265avbr")) {
      rc_codec = OT_PT_H265;
      rc_mode = OT_VENC_RC_MODE_H265_AVBR;
    } else if (!strcmp(value, "265qvbr")) {
      rc_codec = OT_PT_H265;
      rc_mode = OT_VENC_RC_MODE_H265_QVBR;
    } else if (!strcmp(value, "265vbr")) {
      rc_codec = OT_PT_H265;
      rc_mode = OT_VENC_RC_MODE_H265_VBR;
    } else if (!strcmp(value, "265cbr")) {
      rc_codec = OT_PT_H265;
      rc_mode = OT_VENC_RC_MODE_H265_CBR;
    } else {
      printf("> ERROR: Unsuported codec [%s]\n", value);
      exit(1);
    }
    continue;
  }

  __OnArgument("-d") {
    const char* format = __ArgValue;
    if (!strcmp(format, "stream")) {
      venc_by_frame = 0;
    } else if (!strcmp(format, "frame")) {
      venc_by_frame = 1;
    } else {
      printf("> ERROR: Unsupported data format [%s]\n", format);
    }
    continue;
  }

  __OnArgument("-f") {
    sensor_framerate = atoi(__ArgValue);
    // Normalize sensor framerate
    if (sensor_framerate > 60) {
      sensor_framerate = 60;
    }
    continue;
  }

  __OnArgument("-g") {
    venc_gop_denom = atoi(__ArgValue);
    continue;
  }

  __OnArgument("--roi") {
    enable_roi = 1;
    continue;
  }

  __OnArgument("--roi-qp") {
    roi_qp = atoi(__ArgValue);
    continue;
  }

  __OnArgument("-s") {
    const char* value = __ArgValue;
    if (!strcmp(value, "D1")) {
      image_width = 720;
      image_height = 480;
    } else if (!strcmp(value, "960p")) {
      image_width = 960;
      image_height = 576;
    } else if (!strcmp(value, "1.3MP")) {
      image_width = 1280;
      image_height = 1024;
    } else if (!strcmp(value, "720p")) {
      image_width = 1280;
      image_height = 720;
    } else if (!strcmp(value, "800p")) {
      image_width = 1440;
      image_height = 800;
    } else if (!strcmp(value, "1080p")) {
      image_width = 1920;
      image_height = 1080;
    } else if (!strcmp(value, "4MP")) {
      image_width = 2592;
      image_height = 1520;
    } else if (!strcmp(value, "5MP")) {
      image_width = 2880;
      image_height = 1620;
    } else {
      if (sscanf(value, "%dx%d", &image_width, &image_height) != 2) {
        printf("> ERROR: Unsuported image size [%s]\n", value);
        exit(1);
      }
    }
    continue;
  }

  __OnArgument("--mirror") {
    image_mirror = TD_TRUE;
    continue;
  }

  __OnArgument("--flip") {
    image_flip = TD_TRUE;
    continue;
  }

  __EndParseConsoleArguments__

  // Normalize sensor framerate
  if (sensor_framerate > 60) {
    sensor_framerate = 60;
  }

  // Normalize GOP
  venc_gop_size = sensor_framerate / venc_gop_denom;

  /* --- v300 IMX307 --- */
  combo_dev_attr_t* mipi_profile = 0;
  ot_isp_pub_attr* isp_profile = 0;
  ot_isp_sns_obj* sns_object = 0;
  ot_vi_dev_attr* sns_profile = 0;

  ot_vi_pipe_attr* vi_pipe_profile = 0;
  ot_vi_chn_attr* vi_channel_profile = 0;
  ot_3dnr_attr* pipe_3dnr_profile = 0;

  switch (sensor_type) {
    case SC500AI:
      isp_profile = &ISP_PROFILE_SC500AI_MIPI;
      sns_object = &g_sns_sc500ai_obj;
      sns_profile = &DEV_ATTR_SC500AI_BASE;

      vi_pipe_profile = &PIPE_ATTR_RAW12_420;
      vi_channel_profile = &CHN_ATTR_420_SDR8_LINEAR;
      pipe_3dnr_profile = &PIPE_3DNR_ATTR_NORM;
      vi_vpss_mode = OT_VI_ONLINE_VPSS_ONLINE;

      mipi_profile = &MIPI_4lane_CHN0_SENSOR_SC500AI_10BIT_2M_NOWDR_ATTR;
      mipi_profile->mipi_attr.lane_id[0] = 0;
      mipi_profile->mipi_attr.lane_id[1] = 1;
      mipi_profile->mipi_attr.lane_id[2] = 2;
      mipi_profile->mipi_attr.lane_id[3] = 3;

      mipi_profile->mipi_attr.input_data_type = DATA_TYPE_RAW_10BIT;
      break;
  }

  // Update VI pipe / channel resolution
  vi_pipe_profile->size.width = sensor_width;
  vi_pipe_profile->size.height = sensor_height;
  vi_channel_profile->size.width = sensor_width;
  vi_channel_profile->size.height = sensor_height;
  vi_channel_profile->frame_rate_ctrl.src_frame_rate = -1;
  vi_channel_profile->frame_rate_ctrl.dst_frame_rate = -1;

  // Update ISP profile
  isp_profile->frame_rate = isp_framerate;
  isp_profile->sns_size.width = sensor_width;
  isp_profile->sns_size.height = sensor_height;
  isp_profile->wnd_rect.width = sensor_width;
  isp_profile->wnd_rect.height = sensor_height;
  isp_profile->mipi_crop_attr.mipi_crop_offset.height = sensor_height;
  isp_profile->mipi_crop_attr.mipi_crop_offset.width = sensor_width;

  printf(
    "> Starting\n"
    "  - CPU     : v%d\n"
    "  - Sensor  : %d x %d @ %d\n"
    "  - Image   : %d x %d\n"
    ,
    hisi_version,
    sensor_width, sensor_height, sensor_framerate,
    image_width,  image_height
    
  );

  // Reset previous configuration
  ret = ss_mpi_sys_exit();

  // Setup memory pools and initialize system
  ot_vb_cfg vb_conf;
  ot_pic_buf_attr pic_buf_attr;
  memset(&vb_conf, 0x00, sizeof(vb_conf));

  // Use two memory pools
  vb_conf.max_pool_cnt = 2;

  // Memory pool for VI
  pic_buf_attr.width = sensor_width;
  pic_buf_attr.height = sensor_height;
  pic_buf_attr.compress_mode = OT_COMPRESS_MODE_NONE;
  pic_buf_attr.align = OT_DEFAULT_ALIGN;
  pic_buf_attr.bit_width = OT_DATA_BIT_WIDTH_12;
  pic_buf_attr.pixel_format = OT_PIXEL_FORMAT_RGB_BAYER_12BPP;
  pic_buf_attr.video_format = OT_VIDEO_FORMAT_LINEAR;

  vb_conf.common_pool[0].blk_cnt  = 3;
  vb_conf.common_pool[0].blk_size = ot_common_get_pic_buf_size(&pic_buf_attr);

  // Memory pool for VENC
  pic_buf_attr.width = image_width;
  pic_buf_attr.height = image_height;
  pic_buf_attr.compress_mode = OT_COMPRESS_MODE_NONE;
  pic_buf_attr.align = OT_DEFAULT_ALIGN;
  pic_buf_attr.bit_width = OT_DATA_BIT_WIDTH_8;
  pic_buf_attr.pixel_format = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
  pic_buf_attr.video_format = OT_VIDEO_FORMAT_LINEAR;

  vb_conf.common_pool[1].blk_cnt = 2;
  vb_conf.common_pool[1].blk_size = ot_common_get_pic_buf_size(&pic_buf_attr);

  // Configure video buffer
  ret = ss_mpi_vb_set_cfg(&vb_conf);
  if (ret) {
    printf("ERROR: Configure VB failed : 0x%x\n", ret);
  }

  // Initilize video buffer
  ret = ss_mpi_vb_init();
  if (ret) {
    printf("ERROR: Init VB failed : 0x%x\n", ret);
  }

  // Initialize system
  ret = ss_mpi_sys_init();
  if (ret) {
    printf("ERROR: Init SYS failed : 0x%x\n", ret);
    ss_mpi_vb_exit();
  }

  // Set VI-VPSS mode to VI offline and VPSS online
  ot_vi_vpss_mode vi_vpss_mode_config;
  ss_mpi_sys_get_vi_vpss_mode(&vi_vpss_mode_config);
  
  if(vi_vpss_mode == OT_VI_ONLINE_VPSS_ONLINE){
    vi_vpss_mode_config.mode[0] = OT_VI_OFFLINE_VPSS_OFFLINE;
    vi_vpss_mode_config.mode[1] = OT_VI_OFFLINE_VPSS_OFFLINE;
    vi_vpss_mode_config.mode[2] = OT_VI_OFFLINE_VPSS_OFFLINE;
    vi_vpss_mode_config.mode[3] = OT_VI_OFFLINE_VPSS_OFFLINE;
  }else{
    vi_vpss_mode_config.mode[0] = OT_VI_OFFLINE_VPSS_ONLINE;
    vi_vpss_mode_config.mode[1] = OT_VI_OFFLINE_VPSS_ONLINE;
    vi_vpss_mode_config.mode[2] = OT_VI_OFFLINE_VPSS_ONLINE;
    vi_vpss_mode_config.mode[3] = OT_VI_OFFLINE_VPSS_ONLINE;
  }
  
  vi_vpss_mode_config.mode[vi_dev_id] = vi_vpss_mode;

  ret = ss_mpi_sys_set_vi_vpss_mode(&vi_vpss_mode_config);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to set VI-VPSS mode\n");
    return ret;
  }

  // Open MIPI device
  int mipi_device = open("/dev/ot_mipi_rx", O_RDWR);

  // Activate MIPI
  mipi_set_hs_mode(mipi_device, LANE_DIVIDE_MODE_0);
  mipi_enable_clock(mipi_device, mipi_device_id, 1);
  mipi_set_reset(mipi_device, mipi_device_id, 1);

  // Activate sensor
  mipi_enable_sensor_clock(mipi_device, mipi_sensor_id, 1);
  mipi_set_sensor_reset(mipi_device, mipi_sensor_id, 1);

  // Configure
  combo_dev_attr_t mipi_config;
  memcpy(&mipi_config, mipi_profile, sizeof(combo_dev_attr_t));
  mipi_config.devno = mipi_device_id;
  mipi_config.img_rect.width = sensor_width;
  mipi_config.img_rect.height = sensor_height;
  mipi_configure(mipi_device, &mipi_config);

  // Remove reset states
  mipi_set_reset(mipi_device, mipi_device_id, 0);
  mipi_set_sensor_reset(mipi_device, mipi_sensor_id, 0);

  // Close MIPI (not needed anymore)
  close(mipi_device);

  // Set VI device configuration
  sns_profile->in_size.width = sensor_width;
  sns_profile->in_size.height = sensor_height;

  ret = ss_mpi_vi_set_dev_attr(vi_dev_id, sns_profile);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to set VI device attribute : 0x%x\n", ret);
    return ret;
  }

  ot_vi_dev_attr attr;
  ss_mpi_vi_get_dev_attr(vi_dev_id, &attr);

  // Enable VI device
  ret = ss_mpi_vi_enable_dev(vi_dev_id);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to enable VI device\n");
    return ret;
  }

  // Create pipe on VI device
  ret = ss_mpi_vi_bind(vi_dev_id, vi_pipe_id);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to bind VI pipe : 0x%x\n", ret);
    return ret;
  }

  ot_vi_wdr_fusion_grp_attr fusion_grp_attr;
  fusion_grp_attr.wdr_mode = OT_WDR_MODE_NONE;
  fusion_grp_attr.pipe_id[0] = vi_pipe_id;
  fusion_grp_attr.cache_line = image_height/2;
  fusion_grp_attr.pipe_reverse = TD_FALSE;
  ss_mpi_vi_set_wdr_fusion_grp_attr(vi_pipe_id, &fusion_grp_attr);

  // Configure pipe
  ret = ss_mpi_vi_create_pipe(vi_pipe_id, vi_pipe_profile);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to create VI pipe = 0x%x\n", ret);
    return ret;
  }
  
  ot_frame_interrupt_attr vi_int_attr;
  vi_int_attr.interrupt_type = OT_FRAME_INTERRUPT_EARLY;
  vi_int_attr.early_line = image_height - 200;
  ret = ss_mpi_vi_set_pipe_frame_interrupt_attr(vi_pipe_id, &vi_int_attr);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to set VI pipe interrupt : 0x%x\n", ret);
    return ret;
  }

  if(vi_vpss_mode == OT_VI_ONLINE_VPSS_ONLINE){
    ot_frame_interrupt_attr vpss_int_attr;
    vpss_int_attr.interrupt_type = OT_FRAME_INTERRUPT_EARLY_END;
    vpss_int_attr.early_line = image_height - 200;
    ret = ss_mpi_vpss_set_grp_frame_interrupt_attr(vi_pipe_id, &vpss_int_attr);
    if (ret != TD_SUCCESS) {
      printf("ERROR: Unable to set VPSS grp interrupt : 0x%x\n", ret);
      return ret;
    }
  }

  // Start pipe
  ret = ss_mpi_vi_start_pipe(vi_pipe_id);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to start VI pipe\n");
    return ret;
  }

  ot_low_delay_info info;

  if(vi_vpss_mode != OT_VI_ONLINE_VPSS_ONLINE){
    ret = ss_mpi_vi_get_pipe_low_delay(vi_pipe_id, &info);
    if (enable_lowdelay) {
      info.enable = TD_TRUE;
      info.line_cnt = image_height / 4;
      ret = ss_mpi_vi_set_pipe_low_delay(vi_pipe_id, &info);
      if (ret != TD_SUCCESS) {
        printf("ERROR: Unable to set VI low delay mode\n");
        return ret;
      }
    }
  }
  
  // Configure channel
  ret = ss_mpi_vi_set_chn_attr(vi_pipe_id, vi_channel_id, vi_channel_profile);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to set VI channel attribute : 0x%x\n", ret);
    return ret;
  }
  
  // Configure 3dnr

  ot_vi_pipe_param vi_pipe_param;
  ss_mpi_vi_get_pipe_param(vi_pipe_id, &vi_pipe_param);
  vi_pipe_param.nr_effect_mode = OT_VI_NR_EFFECT_MODE_NORM;
  ss_mpi_vi_set_pipe_param(vi_pipe_id, &vi_pipe_param);
  ss_mpi_vi_set_pipe_3dnr_attr(vi_pipe_id, pipe_3dnr_profile);

  // Start channel
  ret = ss_mpi_vi_enable_chn(vi_pipe_id, vi_channel_id);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to enable VI channel : 0x%x\n", ret);
    return ret;
  }

  // Initialize ISP for VI pipe
  ot_isp_sns_commbus bus;
  bus.i2c_dev = 0; // I2C device #0

  // Register ISP libraries in sensor driver
  ot_isp_3a_alg_lib ae_lib;
  ot_isp_3a_alg_lib awb_lib;

  ae_lib.id = 0;
  awb_lib.id = 0;

  strncpy_s(ae_lib.lib_name, sizeof(ae_lib.lib_name), OT_AE_LIB_NAME, sizeof(OT_AE_LIB_NAME));
  strncpy_s(awb_lib.lib_name, sizeof(awb_lib.lib_name), OT_AWB_LIB_NAME, sizeof(OT_AWB_LIB_NAME));

  // Register library callbacks
  ret = sns_object->pfn_register_callback(vi_pipe_id, &ae_lib, &awb_lib);
  if (ret != TD_SUCCESS) {
      printf("sensor_register_callback failed with %#x!\n", ret);
      return ret;
  }
  
  ret = sns_object->pfn_set_bus_info(vi_pipe_id, bus);
  if (ret != TD_SUCCESS) {
      printf("set sensor bus info failed with %#x!\n", ret);
      return ret;
  }

  // Load (register) ISP libraries in MPI
  ret = ss_mpi_ae_register(vi_pipe_id, &ae_lib);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to load ISP ae : 0x%x\n", ret);
    return ret;
  }
  ret = ss_mpi_awb_register(vi_pipe_id, &awb_lib);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to load ISP awb : 0x%x\n", ret);
    return ret;
  }

  // Initialize ISP memory for VI pipe
  ret = ss_mpi_isp_mem_init(vi_pipe_id);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to init ISP memory : 0x%x\n", ret);
    return ret;
  }

  // Configure ISP
  ret = ss_mpi_isp_set_pub_attr(vi_pipe_id, isp_profile);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to set ISP attribute : 0x%x\n", ret);
    return ret;
  }

  // Initialize ISP
  ret = ss_mpi_isp_init(vi_pipe_id);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to init ISP : 0x%x\n", ret);
    ss_mpi_isp_exit(vi_pipe_id);
    return ret;
  }

  if (limit_exposure) {
    ot_isp_exposure_attr attr;
    ret = ss_mpi_isp_get_exposure_attr(0, &attr);
    if (ret != TD_SUCCESS) {
      printf("ERROR: Unable to get exposure\n");
      return ret;
    }

    attr.auto_attr.exp_time_range.max = 10000 * 1000 / sensor_framerate;
    ret = ss_mpi_isp_set_exposure_attr(0, &attr);
    if (ret != TD_SUCCESS) {
      printf("ERROR: Unable to set exposure\n");
      return ret;
    }
  }

  // Create VPSS group
  ot_vpss_grp_attr grp_attr;
  memset(&grp_attr, 0x00, sizeof(grp_attr));
  grp_attr.dynamic_range = OT_DYNAMIC_RANGE_SDR8;
  grp_attr.pixel_format = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
  grp_attr.max_width = MAX2(sensor_width, image_width);
  grp_attr.max_height = MAX2(sensor_height, image_height);
  grp_attr.dei_mode = OT_VPSS_DEI_MODE_OFF;
  grp_attr.frame_rate.src_frame_rate = -1;
  grp_attr.frame_rate.dst_frame_rate = -1;
  ss_mpi_vpss_create_grp(vpss_group_id, &grp_attr);
  
  // Create second VPSS channel #1 for small stream (secondary stream)
  ot_vpss_chn_attr chn_attr;
  memset(&chn_attr, 0x00, sizeof(chn_attr));
  chn_attr.width = image_width;
  chn_attr.height = image_height;
  chn_attr.chn_mode = OT_VPSS_CHN_MODE_USER;
  chn_attr.compress_mode = OT_COMPRESS_MODE_NONE;
  chn_attr.dynamic_range = OT_DYNAMIC_RANGE_SDR8;
  chn_attr.pixel_format = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
  chn_attr.frame_rate.src_frame_rate = -1;
  chn_attr.frame_rate.dst_frame_rate = -1;

  chn_attr.depth = 0;
  chn_attr.mirror_en = image_mirror;
  chn_attr.flip_en = image_flip;
  chn_attr.video_format = OT_VIDEO_FORMAT_LINEAR;
  chn_attr.aspect_ratio.mode = OT_ASPECT_RATIO_NONE;

  ret = ss_mpi_vpss_set_chn_attr(vpss_group_id, vpss_first_ch_id, &chn_attr);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to set VPSS channel configuration = 0x0%x\n", ret);
    return ret;
  }

  ret = ss_mpi_vpss_get_chn_low_delay(vpss_group_id, vpss_first_ch_id, &info);
  if (enable_lowdelay) {
    info.enable = TD_TRUE;
    info.line_cnt = image_height / 4;
    ret = ss_mpi_vpss_set_chn_low_delay(vpss_group_id, vpss_first_ch_id, &info);
    if (ret != TD_SUCCESS) {
      printf("ERROR: Unable to set VPSS low delay mode\n");
      return ret;
    }
  }

  ret = ss_mpi_vpss_get_chn_low_delay(vpss_group_id, vpss_first_ch_id, &info);
  printf("> Low delay is %s, line count = %d\n",
    info.enable ? "[Enabled]" : "[Disabled]", info.line_cnt);

  // Enable channels
  ret = ss_mpi_vpss_enable_chn(vpss_group_id, vpss_first_ch_id);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to enable VPSS channel\n");
    return ret;
  }

  // Start group
  ret = ss_mpi_vpss_start_grp(vpss_group_id);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to start VPSS group\n");
    return ret;
  }

  // Connect VI to VPSS
  ot_mpp_chn vi_src;
  ot_mpp_chn vpss_dst;

  vi_src.mod_id = OT_ID_VI;
  vi_src.dev_id = vi_pipe_id;
  vi_src.chn_id = vi_channel_id;

  vpss_dst.mod_id = OT_ID_VPSS;
  vpss_dst.dev_id = vpss_group_id;
  vpss_dst.chn_id = vpss_first_ch_id;

  ss_mpi_sys_bind(&vi_src, &vpss_dst);

  // Configure h264 encoder
  ot_venc_chn_attr config;
  memset(&config, 0x00, sizeof(config));
  config.venc_attr.type = rc_codec;
  config.venc_attr.max_pic_width = image_width;
  config.venc_attr.max_pic_height = image_height;
  config.venc_attr.pic_width = image_width;
  config.venc_attr.pic_height = image_height;
  config.venc_attr.buf_size = OT_ALIGN_UP(image_width * image_height * 3 / 4, 64);
  config.venc_attr.profile = 0; // Baseline (0), Main(1), High(1)
  config.venc_attr.is_by_frame = venc_by_frame;
  config.gop_attr.gop_mode = OT_VENC_GOP_MODE_NORMAL_P;
  config.gop_attr.normal_p.ip_qp_delta = 2;
  config.rc_attr.rc_mode = rc_mode;

  switch (rc_codec) {
    case OT_PT_H264:
      config.venc_attr.h264_attr.rcn_ref_share_buf_en = TD_FALSE;
      config.venc_attr.h264_attr.frame_buf_ratio = 100;
      break;

    case OT_PT_H265:
      config.venc_attr.h265_attr.rcn_ref_share_buf_en = TD_FALSE;
      config.venc_attr.h265_attr.frame_buf_ratio = 100;
      break;
  }

  switch (rc_mode) {
    case OT_VENC_RC_MODE_H264_AVBR:
      printf("> Codec: h264 AVBR\n");
      config.rc_attr.h264_avbr.src_frame_rate = sensor_framerate;
      config.rc_attr.h264_avbr.dst_frame_rate = sensor_framerate;
      config.rc_attr.h264_avbr.gop = venc_gop_size;
      config.rc_attr.h264_avbr.max_bit_rate = venc_max_rate;
      config.rc_attr.h264_avbr.stats_time = 4;
      break;

    case OT_VENC_RC_MODE_H264_QVBR:
      printf("> Codec: h264 QVBR\n");
      config.rc_attr.h264_qvbr.src_frame_rate = sensor_framerate;
      config.rc_attr.h264_qvbr.dst_frame_rate = sensor_framerate;
      config.rc_attr.h264_qvbr.gop = venc_gop_size;
      config.rc_attr.h264_qvbr.target_bit_rate = venc_max_rate;
      config.rc_attr.h264_qvbr.stats_time = 4;

    case OT_VENC_RC_MODE_H264_VBR:
      printf("> Codec: h264 VBR\n");
      config.rc_attr.h264_vbr.src_frame_rate = sensor_framerate;
      config.rc_attr.h264_vbr.dst_frame_rate = sensor_framerate;
      config.rc_attr.h264_vbr.gop = venc_gop_size;
      config.rc_attr.h264_vbr.max_bit_rate = venc_max_rate;
      config.rc_attr.h264_vbr.stats_time = 4;
      break;

    case OT_VENC_RC_MODE_H264_CBR:
      printf("> Codec: h264 CBR\n");
      config.rc_attr.h264_cbr.src_frame_rate = sensor_framerate;
      config.rc_attr.h264_cbr.dst_frame_rate = sensor_framerate;
      config.rc_attr.h264_cbr.gop = venc_gop_size;
      config.rc_attr.h264_cbr.bit_rate = venc_max_rate;
      config.rc_attr.h264_cbr.stats_time = 4;
      break;

    case OT_VENC_RC_MODE_H265_AVBR:
      printf("> Codec: h265 AVBR\n");
      config.rc_attr.h265_avbr.src_frame_rate = sensor_framerate;
      config.rc_attr.h265_avbr.dst_frame_rate = sensor_framerate;
      config.rc_attr.h265_avbr.gop = venc_gop_size;
      config.rc_attr.h265_avbr.max_bit_rate = venc_max_rate;
      config.rc_attr.h265_avbr.stats_time = 4;
      break;

    case OT_VENC_RC_MODE_H265_VBR:
      printf("> Codec: h265 VBR\n");
      config.rc_attr.h265_vbr.src_frame_rate = sensor_framerate;
      config.rc_attr.h265_vbr.dst_frame_rate = sensor_framerate;
      config.rc_attr.h265_vbr.gop = venc_gop_size;
      config.rc_attr.h265_vbr.max_bit_rate = venc_max_rate;
      config.rc_attr.h265_vbr.stats_time = 4;
      break;

    case OT_VENC_RC_MODE_H265_CBR:
      printf("> Codec: h265 CBR\n");
      config.rc_attr.h265_cbr.src_frame_rate = sensor_framerate;
      config.rc_attr.h265_cbr.dst_frame_rate = sensor_framerate;
      config.rc_attr.h265_cbr.gop = venc_gop_size;
      config.rc_attr.h265_cbr.bit_rate = venc_max_rate;
      config.rc_attr.h265_cbr.stats_time = 4;
      break;

    case OT_VENC_RC_MODE_H265_QVBR:
      printf("> Codec: h265 QVBR\n");
      config.rc_attr.h265_qvbr.src_frame_rate = sensor_framerate;
      config.rc_attr.h265_qvbr.dst_frame_rate = sensor_framerate;
      config.rc_attr.h265_qvbr.gop = venc_gop_size;
      config.rc_attr.h265_qvbr.target_bit_rate = venc_max_rate;
      config.rc_attr.h265_qvbr.stats_time = 4;
      break;
  }

  // Create channel #0
  ret = ss_mpi_venc_create_chn(venc_first_ch_id, &config);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to create VENC channel = 0x%x\n", ret);
    return ret;
  }

  // Configure rate control for channel #1
  ot_venc_rc_param rc_param;
  ss_mpi_venc_get_rc_param(venc_first_ch_id, &rc_param);
  switch (rc_mode) {
    case OT_VENC_RC_MODE_H264_AVBR:
      rc_param.h264_avbr_param.max_reencode_times = 0;
      break;

    case OT_VENC_RC_MODE_H264_QVBR:
      rc_param.h264_qvbr_param.max_reencode_times = 0;
      break;

    case OT_VENC_RC_MODE_H264_VBR:
      rc_param.h264_vbr_param.max_reencode_times = 0;
      break;

    case OT_VENC_RC_MODE_H264_CBR:
      rc_param.h264_cbr_param.max_reencode_times = 0;
      break;

    case OT_VENC_RC_MODE_H265_AVBR:
      rc_param.h265_avbr_param.max_reencode_times = 0;
      break;

    case OT_VENC_RC_MODE_H265_QVBR:
      rc_param.h265_qvbr_param.max_reencode_times = 0;
      break;

    case OT_VENC_RC_MODE_H265_VBR:
      rc_param.h265_vbr_param.max_reencode_times = 0;
      break;

    case OT_VENC_RC_MODE_H265_CBR:
      rc_param.h265_cbr_param.max_reencode_times = 0;
      break;
  }

  rc_param.first_frame_start_qp = -1;
  rc_param.scene_chg_detect.adapt_insert_idr_frame_en = TD_TRUE;
  rc_param.scene_chg_detect.detect_scene_chg_en = TD_TRUE;

  ret = ss_mpi_venc_set_rc_param(venc_first_ch_id, &rc_param);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to set VENC RC options = 0x%x\n", ret);
    return ret;
  }

  ss_mpi_venc_get_rc_param(venc_first_ch_id, &rc_param);
  printf("> Scene detect = %s, Adaptive IDR = %s, Start Qp = %d, Row dQp = %d\n",
    rc_param.scene_chg_detect.detect_scene_chg_en ? "YES" : "NO",
    rc_param.scene_chg_detect.adapt_insert_idr_frame_en ? "YES" : "NO",
    rc_param.first_frame_start_qp, rc_param.row_qp_delta);

  // Enable slices (not available in frame mode)

  ot_venc_slice_split avc_param;
  ss_mpi_venc_get_slice_split(venc_first_ch_id, &avc_param);
  avc_param.split_mode = 1;
  avc_param.enable = 1;
  avc_param.slice_output_en = 1;
  avc_param.split_size = venc_slice_size;

  if (enable_slices) {
    if (venc_by_frame) {
      printf("WARN: Slices are not available in [frame] data format\n");
    } else {
      int ret = ss_mpi_venc_set_slice_split(venc_first_ch_id, &avc_param);
      if (ret != TD_SUCCESS) {
        printf("ERROR: Unable to set VENC slice size = 0x%x\n", ret);
        return ret;
      }
    }
  }

  ss_mpi_venc_get_slice_split(venc_first_ch_id, &avc_param);
  printf("> VENC slices is [%s] | Slice size = %d lines\n",
    avc_param.enable ? "Enabled" : "Disabled", avc_param.split_size);

  ot_venc_ref_param ref_param;
  ss_mpi_venc_get_ref_param(venc_first_ch_id, &ref_param);
  printf("> Reference = EN: %d, Base: %d, Enhance: %d\n",
    ref_param.pred_en, ref_param.base, ref_param.enhance);

  ref_param.pred_en = 1;
  ref_param.enhance = 0;
  ref_param.base = 1;

  ret = ss_mpi_venc_set_ref_param(venc_first_ch_id, &ref_param);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to set VENC REF options = 0x%x\n", ret);
    return ret;
  }

  // Setup frame lost strategy
  if (rc_codec == OT_PT_H265) {
    ot_venc_frame_lost_strategy lost_param;
    ret = ss_mpi_venc_get_frame_lost_strategy(venc_first_ch_id, &lost_param);
    if (ret != TD_SUCCESS) {
      printf("ERROR: Unable to get frame lost strategy = 0x%x\n", ret);
      return ret;
    }

    lost_param.enable = 1;
    lost_param.mode = OT_VENC_FRAME_LOST_P_SKIP;
    lost_param.bit_rate_threshold = venc_max_rate * 1024 / 2;
    lost_param.frame_gap = 1;

    ret = ss_mpi_venc_set_frame_lost_strategy(venc_first_ch_id, &lost_param);
    if (ret != TD_SUCCESS) {
      printf("ERROR: Unable to set frame lost strategy = 0x%x\n", ret);
      return ret;
    }
  }

  if (enable_roi) {
    ot_venc_roi_attr roi_config;
    roi_config.enable = TD_TRUE;
    roi_config.idx = 0;
    roi_config.rect.x = OT_ALIGN_UP(image_width / 4, 16);
    roi_config.rect.y = OT_ALIGN_UP(image_height / 4, 16);
    roi_config.rect.width = OT_ALIGN_UP(image_width / 2, 16);
    roi_config.rect.height = OT_ALIGN_UP(image_height / 2, 16);
    roi_config.is_abs_qp = TD_TRUE;
    roi_config.qp = roi_qp;

    ret = ss_mpi_venc_set_roi_attr(venc_first_ch_id, &roi_config);
    if (ret != TD_SUCCESS) {
      printf("ERROR: Unable to setup VENC ROI = 0x%x\n", ret);
      return ret;
    }

    printf("> ROI is [Enabled]\n");
  }

  // Connect VPSS channel #1 to VENC channel #1
  ot_mpp_chn vpss_src;
  ot_mpp_chn venc_dst;

  vpss_src.mod_id = OT_ID_VPSS;
  vpss_src.dev_id = vpss_group_id;
  vpss_src.chn_id = vpss_first_ch_id;

  venc_dst.mod_id = OT_ID_VENC;
  venc_dst.dev_id = 0;
  venc_dst.chn_id = venc_first_ch_id;

  ss_mpi_sys_bind(&vpss_src, &venc_dst);

  // Start VENC channel #1 without frames count limit
  ot_venc_start_param recv_param;
  recv_param.recv_pic_num = -1;
  ret = ss_mpi_venc_start_chn(venc_first_ch_id, &recv_param);
  if (ret != TD_SUCCESS) {
    printf("ERROR: Unable to start Rx frames\n");
    return ret;
  }

  // Start ISP service thread
  pthread_t isp_thread;
  pthread_create(&isp_thread, NULL, __ISP_THREAD__, (void*)vi_pipe_id);

  // Open socket handle
  int socket_handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in dst_addr;
  dst_addr.sin_family = AF_INET;
  dst_addr.sin_port = htons(udp_sink_port);
  dst_addr.sin_addr.s_addr = udp_sink_ip;

  // Prepare Tx buffer
  tx_buffer = malloc(65536);
  printf("> Ready for streaming\n");
  signal(SIGINT, handler);

  while (loop_running) {
    // Process stream on encoder channel #1
    if (!processStream(venc_first_ch_id, socket_handle,
        (struct sockaddr*)&dst_addr, max_frame_size)) {
      // --- Take a rest if no frames received
      // Another way: HI_MPI_VENC_GetFd(vecn_channel_id) + epoll
      usleep(1);
    }
  }

  printf("> Stop streaming\n");

  ss_mpi_isp_exit(vi_pipe_id);
  ss_mpi_vpss_stop_grp(vpss_group_id);
  ss_mpi_vpss_destroy_grp(vpss_group_id);

  ss_mpi_vi_disable_chn(vi_pipe_id, vi_channel_id);
  ss_mpi_vi_stop_pipe(vi_pipe_id);
  ss_mpi_vi_destroy_pipe(vi_pipe_id);
  ss_mpi_vi_disable_dev(vi_pipe_id);

  ss_mpi_sys_exit();
  ss_mpi_vb_exit();

  return 0;
}

void* __ISP_THREAD__(void* param) {
  ss_mpi_isp_run((ot_vi_pipe)param);
}

double getTimeInterval(
  struct timespec* timestamp, struct timespec* last_meansure_timestamp) {
  return (timestamp->tv_sec - last_meansure_timestamp->tv_sec) +
       (timestamp->tv_nsec - last_meansure_timestamp->tv_nsec) / 1000000000.;
}

struct timespec last_timestamp = {0, 0};
uint32_t bytes_sent = 0;
uint32_t frames_sent = 0;
uint64_t seq_last = 0;

uint64_t jitter_sum = 0;
uint64_t jitter_cnt = 0;
uint32_t nal_max_size = 0;
uint32_t single_packets = 0;

uint32_t pps_count = 0;
uint32_t sps_count = 0;
uint32_t idr_count = 0;
uint32_t sei_count = 0;
uint32_t s_count = 0;
uint32_t packets_sent = 0;

int processStream(ot_venc_chn channel_id, int socket_handle,
  struct sockaddr* dst_address, uint16_t max_frame_size) {
  // Get channel status
  ot_venc_chn_status channel_status;
  int ret = ss_mpi_venc_query_status(channel_id, &channel_status);
  if (ret != TD_SUCCESS) {
    printf("WARN: Unable to query VENC status = 0x%x\n", ret);
    usleep(100000);
    return 0;
  }

  // Check if has encoded data
  if (!channel_status.cur_packs) {
    // Nothing to get
    return 0;
  }

  // Stream packets descriptors
  // Per-Slice mode always return one slice at a time.
  // Per-Frame mode may return multiple slices, we need to allocate memory for all reported slices.
  ot_venc_pack packet_descriptor[32];

  // Stream buffer
  ot_venc_stream stream;
  memset(&stream, 0x00, sizeof(stream));
  stream.pack = packet_descriptor;
  stream.pack_cnt = channel_status.cur_packs;

  // Acquire stream
  ret = ss_mpi_venc_get_stream(channel_id, &stream, 0);
  if (ret != TD_SUCCESS) {
    printf("WARN: Failed to get VENC stream  = 0x%x. Current packs = %d\n",
      ret, channel_status.cur_packs);
    usleep(100000);
    return 0;
  }

  // Send encoded packets
  for (uint32_t i = 0; i < stream.pack_cnt; i++) {
    sendPacket(stream.pack[i].addr + stream.pack[i].offset,
      stream.pack[i].len - stream.pack[i].offset,
      socket_handle, dst_address, max_frame_size);
  }

  // Release stream
  ss_mpi_venc_release_stream(channel_id, &stream);

  // Print rate stats
  struct timespec current_timestamp;
  if (!clock_gettime(CLOCK_MONOTONIC_COARSE, &current_timestamp)) {
    double interval = getTimeInterval(&current_timestamp, &last_timestamp);
    if (interval > 1) {
      printf("> Rate: %.2f Mbit/sec. (%.1f pps) | Frames: %d, NotFrag: "
           "%d | AVG Size: %d, MAX Size: %d | S: %d, IDR: %d, SEI: %d, "
           "PPS: %d, SPS: %d | Packets: %d\n",
        ((double)bytes_sent * 8) / interval / 1024 / 1024,
        (double)frames_sent / interval, /* jitter_sum / jitter_cnt,*/
        frames_sent, single_packets, bytes_sent / frames_sent,
        nal_max_size, s_count, idr_count, sei_count, pps_count,
        sps_count, packets_sent);

      bytes_sent = 0;
      frames_sent = 0;
      jitter_sum = 0;
      jitter_cnt = 0;
      nal_max_size = 0;
      s_count = 0;
      idr_count = 0;
      pps_count = 0;
      sps_count = 0;
      sei_count = 0;
      single_packets = 0;
      packets_sent = 0;
      last_timestamp = current_timestamp;
    }
  }

  return 1;
}

uint32_t sequence_id = 0;
uint32_t frame_id = 0;
uint16_t rtp_sequence = 0;

void transmit(int socket_handle, uint8_t* tx_buffer, uint32_t tx_size,
  struct sockaddr* dst_address) {

  switch (stream_mode) {
    // Compact mode
    case 0:
      sendto(socket_handle, tx_buffer, tx_size, 0, dst_address, sizeof(struct sockaddr_in));
      break;

    // RTP mode
    case 1:;
      struct RTPHeader rtp_header;
      rtp_header.version = 0x80;
      rtp_header.sequence = htobe16(rtp_sequence++);
      rtp_header.payload_type = 0x60;
      rtp_header.timestamp = 0;
      rtp_header.ssrc_id = 0xDEADBEEF;

      // struct iovec iov[2];
      // iov[0].iov_base = &rtp_header;
      // iov[0].iov_len = sizeof(struct RTPHeader);
      // iov[1].iov_base = tx_buffer;
      // iov[1].iov_len = tx_size;

      // struct msghdr msg;
      // msg.msg_iovlen = 2;
      // msg.msg_iov = iov;
      // msg.msg_name = dst_address;
      // msg.msg_namelen = sizeof(struct sockaddr_in);

      // sendmsg(socket_handle, &msg, 0);
      
      uint8_t* combined_buffer = (uint8_t*)malloc(sizeof(struct RTPHeader) + tx_size);
      memcpy(combined_buffer, &rtp_header, sizeof(struct RTPHeader));
      memcpy(combined_buffer + sizeof(struct RTPHeader), tx_buffer, tx_size);
      sendto(socket_handle, combined_buffer, sizeof(struct RTPHeader) + tx_size,
                 0, dst_address, sizeof(struct sockaddr_in));
      free(combined_buffer);
      break;
  }
}

void sendPacket(uint8_t* pack_data, uint32_t pack_size, int socket_handle,
    struct sockaddr* dst_address, uint32_t max_size) {
  uint8_t prefix = 4;
  pack_data += prefix;
  pack_size -= prefix;

  frame_id++;
  frames_sent++;

  if (pack_size > nal_max_size) {
    nal_max_size = pack_size;
  }

  if (pack_size <= max_size) {
    single_packets++;
  }

  // Get NAL type
  uint8_t nal_type = pack_data[0] & 0x1F;
  switch (nal_type) {
    case 1:
      s_count++;
      break;

    case 5:
      idr_count++;
      break;

    case 6:
      sei_count++;
      break;

    case 7:
      sps_count++;
      break;

    case 8:
      pps_count++;
      break;

    default:
      break;
  }

  if (pack_size > max_size + prefix) {
    uint8_t nal_type_avc = pack_data[0] & 0x1F;
    uint8_t nal_type_hevc = (pack_data[0] >> 1) & 0x3F;
    uint8_t nal_bits_avc = pack_data[0] & 0xE0;
    uint8_t nal_bits_hevc = pack_data[0] & 0x81;

    bool start_bit = true;
    uint8_t tx_size = 2;

    while (pack_size) {
      uint32_t chunk_size = pack_size > max_size ? max_size : pack_size;
      if (nal_type_avc == 1 || nal_type_avc == 5) {
        tx_buffer[0] = nal_bits_avc | 28;
        tx_buffer[1] = nal_type_avc;

        if (start_bit) {
          pack_data++;
          pack_size--;
          tx_buffer[1] = 0x80 | nal_type_avc;
          start_bit = false;
        }

        if (chunk_size == pack_size) {
          tx_buffer[1] |= 0x40;
        }
      }

      if (nal_type_hevc == 1 || nal_type_hevc == 19) {
        tx_buffer[0] = nal_bits_hevc | 49 << 1;
        tx_buffer[1] = 1;
        tx_buffer[2] = nal_type_hevc;
        tx_size = 3;

        if (start_bit) {
          pack_data += 2;
          pack_size -= 2;
          tx_buffer[2] = 0x80 | nal_type_hevc;
          start_bit = false;
        }

        if (chunk_size == pack_size) {
          tx_buffer[2] |= 0x40;
        }
      }

      memcpy(tx_buffer + tx_size, pack_data, chunk_size + tx_size);
      transmit(socket_handle, tx_buffer, chunk_size + tx_size, dst_address);

      packets_sent++;
      bytes_sent += chunk_size + tx_size;

      pack_data += chunk_size;
      pack_size -= chunk_size;
    }
  } else {
    transmit(socket_handle, pack_data, pack_size, dst_address);
    packets_sent++;
  }
}
