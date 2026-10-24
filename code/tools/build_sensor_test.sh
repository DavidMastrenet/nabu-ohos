#!/bin/bash
set -euo pipefail
/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908/bin/clang \
 --target=aarch64-linux-ohosmusl --sysroot=/root/nabu-build/ohos-sysroot -O2 \
 /root/nabu-package/port/sensor-framework-test.c -lohsensor -Wl,--allow-shlib-undefined \
 -o /root/nabu-package/port/out/sensor-framework-test
