#!/bin/bash
set -euo pipefail
cd /root/nabu-build/dropbear
cat > localoptions.h <<'EOF'
#define DROPBEAR_SVR_PASSWORD_AUTH 0
#define DROPBEAR_SVR_PAM_AUTH 0
#define DROPBEAR_DEFAULT_PATH "/system/bin:/vendor/bin:/debug"
EOF
CC=aarch64-linux-gnu-gcc AR=aarch64-linux-gnu-ar RANLIB=aarch64-linux-gnu-ranlib \
CFLAGS='-Os -ffunction-sections -fdata-sections' LDFLAGS='-static -Wl,--gc-sections' \
./configure --host=aarch64-linux-gnu --disable-zlib --disable-shadow --disable-syslog \
 --disable-lastlog --disable-utmp --disable-utmpx --disable-wtmp --disable-wtmpx \
 --enable-bundled-libtom > configure-nabu.log 2>&1
make -j8 PROGRAMS='dropbear dropbearkey scp' MULTI=1 STATIC=1 \
 LDFLAGS='-static -no-pie -Wl,--gc-sections' > build-nabu.log 2>&1 || {
 tail -n 45 build-nabu.log
 exit 1
}
aarch64-linux-gnu-strip dropbearmulti
if aarch64-linux-gnu-readelf -l dropbearmulti | grep -q INTERP; then
 echo 'Unexpected dynamic interpreter' >&2
 exit 1
fi
cp dropbearmulti /root/nabu-package/port/out/dropbearmulti
ls -l /root/nabu-package/port/out/dropbearmulti
