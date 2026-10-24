#!/bin/bash
set -euo pipefail
tc=/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908
w=/root/nabu-package
"$tc/bin/clang" --target=aarch64-linux-ohosmusl --sysroot=/root/nabu-build/ohos-sysroot \
 -shared -fPIC -O2 -std=c11 -Wl,-z,defs -Wl,-soname,libhdi_sensor.z.so \
 -I"$w/sources/drivers_peripheral/sensor/interfaces/include" \
 "$w/port/sensor-hal.c" "$w/port/ssc-client.c" -pthread -lm \
 -o "$w/port/out/libhdi_sensor.z.so"
