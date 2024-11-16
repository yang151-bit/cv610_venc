#!/bin/bash
CC=cortex_a7_thumb2-gcc13-musl-4_9

GCC=$PWD/toolchain/$CC/bin/arm-linux-gcc

if [ "$1" = "venc-hisi" ]; then
	DRV=$PWD/sdk/hi3516cv610/lib
	make -C venc -B CC=$GCC DRV=$DRV $1
else
	echo "Usage: $0 [venc-hisi]"
	exit 1
fi
