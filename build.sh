#!/bin/bash
CC=arm-v01c02-linux-musleabi-gcc

GCC=$PWD/toolchain/$CC/bin/arm-linux-musleabi-gcc

if [ "$1" = "venc-hisi" ]; then
	DRV=$PWD/sdk/hi3516cv610/lib
	make -C venc -B CC=$GCC DRV=$DRV $1
else
	echo "Usage: $0 [venc-hisi]"
	exit 1
fi
