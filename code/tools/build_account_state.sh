#!/bin/bash
set -euo pipefail
tc=/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908
lib=/root/nabu-rootfs/system-lib/lib64
"$tc/bin/clang++" --target=aarch64-linux-ohosmusl --sysroot=/root/nabu-build/ohos-sysroot \
 -O2 -fno-rtti -std=c++17 -nostdinc++ -isystem "$tc/include/libcxx-ohos/include/c++/v1" \
 /root/nabu-package/port/account-state.cpp -L"$lib/platformsdk" \
 -Wl,-rpath-link,"$lib/platformsdk" -Wl,-rpath-link,"$lib" \
 -l:libos_account_innerkits.z.so -Wl,--allow-shlib-undefined \
 -o /root/nabu-package/port/out/account-state
