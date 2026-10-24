#!/bin/bash
set -euo pipefail
build=/root/nabu-build/nabu-fastrpc
w=/root/nabu-package
mkdir -p "$build"
cp "$w/port/drivers/fastrpc.c" "$build/fastrpc.c"
printf 'obj-m := fastrpc.o\n' > "$build/Makefile"
make -C /root/nabu-build/linux-6.17 M="$build" ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j2 modules
cp "$build/fastrpc.ko" "$w/port/out/fastrpc.ko"
