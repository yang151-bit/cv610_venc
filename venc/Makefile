VENC := main.c common.c isp_profiles.c mipi_profiles.c vi_profiles.c
SENSOR = $(SDK)/sensor/sc500ai_cmos.c $(SDK)/sensor/sc500ai_sensor_ctrl.c 
BUILD_BASE = $(CC) $(VENC) $(SENSOR) -I $(SDK)/include -L $(DRV) -o venc
BUILD = $(BUILD_BASE) -Wl,--start-group $(LIB) -Wl,--end-group

venc-hisi:
	$(eval SDK = ../sdk/hi3516cv610)
	$(eval LIB =  -lss_mpi -lss_mpi_sysbind -lss_mpi_sysmem -lss_mpi_ae -lss_mpi_isp -lot_mpi_isp -lss_mpi_awb\
		 -ldehaze -lextend_stats -ldrc -lldci -ldnvqe -lupvqe -lbnr -lcalcflicker -lir_auto -lacs -lvoice_engine -lsecurec)
	$(BUILD)
