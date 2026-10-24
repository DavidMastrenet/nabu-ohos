#!/bin/bash
set -euo pipefail
kernelroot=/root/nabu-build/linux-6.17
workspaceroot=/root/nabu-package
python3 "$workspaceroot/tools/patch_kernel_ohos.py"
cd "$kernelroot"
python3 -c 'import pathlib; pathlib.Path(".config").write_text(pathlib.Path("/root/nabu-package/analysis/nabu-kernel.config").read_text().replace("\r\n","\n"))'
scripts/config --set-str LOCALVERSION '-xiaomi-nabu-ohos' --disable LOCALVERSION_AUTO \
  --enable ACCESS_TOKENID --enable BINDER_SENDER_INFO \
  --enable MEMCG_V1 --enable CPUSETS_V1 \
  --enable DRM_PANEL_NOVATEK_NT36523 --enable BACKLIGHT_KTZ8866 \
  --enable TOUCHSCREEN_NT36523_SPI --enable SPI_QCOM_GENI \
  --enable SECURITY_NETWORK --enable SECURITY_SELINUX \
  --disable RTC_DRV_EFI --enable RTC_DRV_PM8XXX \
  --disable WERROR --enable DEBUG_INFO_NONE --disable DEBUG_INFO_DWARF_TOOLCHAIN_DEFAULT \
  --disable DEBUG_INFO_BTF
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- olddefconfig > /root/nabu-build/kernel-config.log 2>&1
grep -E '^CONFIG_(ACCESS_TOKENID|BINDER_SENDER_INFO|MEMCG_V1|CPUSETS_V1|DRM_PANEL_NOVATEK_NT36523|BACKLIGHT_KTZ8866|TOUCHSCREEN_NT36523_SPI|SPI_QCOM_GENI|SECURITY_SELINUX)=' .config
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j10 Image > /root/nabu-build/kernel-build.log 2>&1 || {
  grep -n -A4 -B3 -E 'error:|undefined reference|Error [0-9]' /root/nabu-build/kernel-build.log | tail -n 100
  exit 1
}
cp arch/arm64/boot/Image "$workspaceroot/port/out/Image-nabu-ohos"
cp .config "$workspaceroot/port/out/kernel-ohos.config"
echo 'Built port/out/Image-nabu-ohos'
