#!/bin/bash
set -euo pipefail
python3 /root/nabu-package/tools/prepare_mesa.py
cd /root/nabu-build/graphics
if [ ! -f build-mesa/build.ninja ]; then
 meson setup build-mesa mesa-ohos5 --cross-file ohos.cross --prefix=/root/nabu-build/graphics/install --libdir=lib \
  --wrap-mode=nofallback -Dplatforms=ohos -Degl-native-platform=ohos -Dgbm=enabled \
  -Dgallium-drivers=freedreno,swrast -Dvulkan-drivers= -Dbuildtype=release \
  -Degl=enabled -Dgles1=enabled -Dgles2=enabled -Dopengl=true -Dcpp_rtti=false \
  -Dglx=disabled -Dtools= -Dglvnd=false -Dgallium-va=disabled -Dgallium-vdpau=disabled \
  -Dshared-glapi=enabled -Dshader-cache=enabled -Dzlib=enabled -Dzstd=disabled \
  -Dllvm=disabled -Dlmsensors=disabled -Dlibunwind=disabled -Dbuild-tests=false \
  -Ddri-drivers-path=/vendor/lib64/dri
fi
ninja -C build-mesa -j4
ninja -C build-mesa install
