#!/bin/bash
set -euo pipefail
workspace=/root/nabu-package
kernel=/root/nabu-build/linux-6.17
module=/root/nabu-build/ashmem
mkdir -p "$module"
cp -r "$workspace/port/ashmem/"* "$module/"
make -C "$kernel" ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules_prepare
if [ ! -f "$kernel/Module.symvers" ]; then
    cp "$kernel/vmlinux.symvers" "$kernel/Module.symvers"
fi
make -C "$kernel" ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M="$module" -j4 modules
cp "$module/ashmem.ko" "$workspace/port/out/ashmem.ko"
