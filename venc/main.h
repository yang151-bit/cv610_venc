#pragma once
#define _POSIX_TIMERS
#define _REENTRANT
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include "ot_buffer.h"
#include "ot_common_adec.h"
#include "ot_common_aenc.h"
#include "ot_common_aio.h"
#include "ot_common_isp.h"
#include "ot_common_region.h"
#include "ot_common_sys.h"
#include "ot_common_vb.h"
#include "ot_common_venc.h"
#include "ot_common_vgs.h"
#include "ot_common_vi.h"
#include "ot_common_vpss.h"
#include "ot_common.h"
#include "ot_defines.h"
#include "ot_mipi_rx.h"

#include "ot_math.h"
#include "ot_sns_ctrl.h"
#include "ss_mpi_ae.h"
#include "ss_mpi_audio.h"
#include "ss_mpi_awb.h"
#include "ss_mpi_isp.h"
#include "ss_mpi_region.h"
#include "ss_mpi_sys.h"
#include "ss_mpi_sys_bind.h"
#include "ss_mpi_vb.h"
#include "ss_mpi_venc.h"
#include "ss_mpi_vgs.h"
#include "ss_mpi_vi.h"
#include "ss_mpi_vpss.h"

#include "sc500ai_cmos.h"

typedef enum SensorType {
  SC500AI = 0,
} SensorType;

#pragma pack(push, 1)
struct RTPHeader {
  uint8_t version;
  uint8_t payload_type;
  uint16_t sequence;
  uint32_t timestamp;
  uint32_t ssrc_id;
};
#pragma pack(pop)

void* __ISP_THREAD__(void* param);
int processStream(ot_venc_chn channel_id, int socket_handle,
  struct sockaddr* dst_address, uint16_t max_frame_size);
void sendPacket(uint8_t* pack_data, uint32_t pack_size, int socket_handle,
  struct sockaddr* dst_address, uint32_t max_size);
td_s32 getGOPAttributes(ot_venc_gop_mode enGopMode, ot_venc_gop_attr* pstGopAttr);

int mipi_set_hs_mode(int device, lane_divide_mode_t mode);
int mipi_enable_clock(int device, uint32_t device_id, int enable);
int mipi_set_reset(int device, uint32_t device_id, int enable);
int mipi_enable_sensor_clock(
  int device, sns_clk_source_t sensor_id, int enable);
int mipi_set_sensor_reset(int device, sns_clk_source_t sensor_id, int enable);
int mipi_configure(int device, combo_dev_attr_t* config);

/* --- Console arguments parser --- */
#define __BeginParseConsoleArguments__(printHelpFunction) if (argc < 2 \
  || (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp( argv[ 1 ], "/?" ) \
  || !strcmp( argv[1], "/h" )))) { printHelpFunction(); return 1; } \
  for (int ArgID = 1; ArgID < argc; ArgID++) {const char* Arg = argv[ArgID];

#define __EndParseConsoleArguments__ \
  else { printf("ERROR: Unknown argument\n"); return 1; } }

#define __OnArgument(Name) if(!strcmp(Arg, Name))
#define __ArgValue (argc > ArgID + 1 ? argv[++ArgID] : "")
