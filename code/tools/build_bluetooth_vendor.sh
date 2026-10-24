#!/bin/bash
set -euo pipefail
tc=/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908
w=/root/nabu-package
"$tc/bin/clang++" --target=aarch64-linux-ohosmusl --sysroot=/root/nabu-build/ohos-sysroot \
 -O2 -std=c++17 -fPIC -fno-emulated-tls -nostdinc++ -isystem "$tc/include/libcxx-ohos/include/c++/v1" \
 -I"$w/sources/drivers_peripheral/bluetooth/hci/hdi_service/implement" \
 -shared -Wl,-z,defs -Wl,-soname,libbt_vendor.z.so "$w/port/bluetooth-vendor.cpp" \
 -o "$w/port/out/libbt_vendor.z.so"
