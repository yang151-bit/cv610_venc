#!/bin/bash
DL="https://github.com/yang151-bit/cv610_venc/releases/tag/toolchain"

CC=arm-v01c02-linux-musleabi-gcc

GCC=$PWD/toolchain/$CC/bin/arm-linux-musleabi-gcc

if [ ! -e toolchain/$CC ]; then
        wget -c -q --show-progress $DL/$CC.tgz -P $PWD
        mkdir -p toolchain/$CC
        tar -xf $CC.tgz -C toolchain/$CC --strip-components=1 || exit 1
        rm -f $CC.tgz
fi

if [ "$1" = "venc-hisi" ]; then
	DRV=$PWD/sdk/hi3516cv610/lib
	make -C venc -B CC=$GCC DRV=$DRV $1
else
	echo "Usage: $0 [venc-hisi]"
	exit 1
fi
