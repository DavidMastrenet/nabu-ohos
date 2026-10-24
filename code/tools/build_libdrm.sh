#!/bin/bash
set -euo pipefail
python3 /root/nabu-package/tools/prepare_graphics.py
cd /root/nabu-build/graphics
if [ ! -f build-libdrm/build.ninja ]; then
 meson setup build-libdrm libdrm-2.4.124 --cross-file ohos.cross --prefix=/root/nabu-build/graphics/install --libdir=lib \
  -Dbuildtype=release -Dintel=disabled -Dradeon=disabled -Damdgpu=disabled -Dnouveau=disabled \
  -Dvmwgfx=disabled -Domap=disabled -Dexynos=disabled -Dfreedreno=enabled -Dtegra=disabled \
  -Dvc4=disabled -Detnaviv=disabled -Dcairo-tests=disabled -Dman-pages=disabled -Dvalgrind=disabled -Dtests=false
fi
ninja -C build-libdrm -j6
ninja -C build-libdrm install
