#!/bin/bash
set -euo pipefail
tc=/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908
"$tc/bin/clang" --target=aarch64-linux-ohosmusl --sysroot=/root/nabu-build/ohos-sysroot \
 -O2 /root/nabu-package/port/audio-framework-test.c -lm -lohaudio \
 -Wl,--allow-shlib-undefined -o /root/nabu-package/port/out/audio-framework-test
