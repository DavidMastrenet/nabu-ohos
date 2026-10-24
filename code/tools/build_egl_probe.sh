#!/bin/bash
set -euo pipefail
tc=/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908
g=/root/nabu-build/graphics
"$tc/bin/clang" --target=aarch64-linux-ohosmusl --sysroot=/root/nabu-build/ohos-sysroot \
 -O2 -DEGL_NO_X11 /root/nabu-package/port/egl-probe.c -I"$g/install/include" -L"$g/install/lib" \
 -l:libEGL.so.1.0.0 -l:libGLESv2.so.2.0.0 -Wl,--allow-shlib-undefined \
 -o /root/nabu-package/port/out/egl-probe
