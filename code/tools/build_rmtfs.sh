#!/bin/bash
set -euo pipefail
b=/root/nabu-build
w=/root/nabu-package
cc=aarch64-linux-gnu-gcc
python3 - <<'PY'
from pathlib import Path
for name in ['rmtfs-master/rmtfs.c','tqftpserv-master/tqftpserv.c']:
    p=Path('/root/nabu-build')/name
    s=p.read_text()
    marker='int main(int argc, char **argv)\n{'
    if 'setvbuf(stdout, NULL, _IOLBF, 0);' not in s:
        if marker not in s: raise ValueError(name)
        p.write_text(s.replace(marker,marker+'\n\tsetvbuf(stdout, NULL, _IOLBF, 0);',1))
PY
mkdir -p "$b/rmtfs-build/include"
cp "$b/libudev-zero-master/udev.h" "$b/rmtfs-build/include/libudev.h"
make -C "$b/libudev-zero-master" CC="$cc" AR=aarch64-linux-gnu-ar CFLAGS=-O2 libudev.a
q="$b/qrtr-master"
r="$b/rmtfs-master"
common=(-O2 -static -I"$q/include" -I"$b/rmtfs-build/include")
"$cc" "${common[@]}" "$r"/{qmi_rmtfs,rmtfs,rproc,sharedmem,storage,util}.c \
 "$q"/lib/{logging,qmi,qrtr}.c "$b/libudev-zero-master/libudev.a" -pthread -o "$w/port/out/rmtfs"
"$cc" "${common[@]}" "$q/src/lookup.c" "$q"/lib/{logging,qmi,qrtr}.c -o "$w/port/out/qrtr-lookup"
"$cc" "${common[@]}" "$q/src/addr.c" "$q/src/cfg.c" "$q"/lib/{logging,qmi,qrtr}.c -o "$w/port/out/qrtr-cfg"
file "$w/port/out/rmtfs" "$w/port/out/qrtr-lookup"
"$cc" "${common[@]}" -DANDROID "$b/tqftpserv-master/"{tqftpserv,translate}.c \
 "$q"/lib/{logging,qmi,qrtr}.c -o "$w/port/out/tqftpserv"
