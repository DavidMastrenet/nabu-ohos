#!/bin/bash
set -euo pipefail
taskroot=/root/nabu-build
mkdir -p "$taskroot"
cd "$taskroot"
python3 -m tarfile -e /root/nabu-package/tools/busybox.tar.bz2 .
cd busybox-1.37.0
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig >/dev/null
sed -i -e 's/# CONFIG_STATIC is not set/CONFIG_STATIC=y/' -e 's/CONFIG_TC=y/# CONFIG_TC is not set/' -e 's/CONFIG_SHA1_HWACCEL=y/# CONFIG_SHA1_HWACCEL is not set/' -e 's/CONFIG_SHA256_HWACCEL=y/# CONFIG_SHA256_HWACCEL is not set/' .config
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- oldconfig </dev/null >/dev/null
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j12 >build.log 2>&1 || { grep -n -A5 -B2 -E 'error:|undefined reference|Error [0-9]' build.log; tail -n 15 build.log; exit 1; }
cp busybox /root/nabu-package/tools/busybox-aarch64
file busybox
