#!/bin/bash
set -euo pipefail
cd /root/nabu-build/tinyalsa-master
make -C src CROSS_COMPILE=aarch64-linux-gnu- CFLAGS=-O2 libtinyalsa.a
for prog in tinyplay tinymix tinycap tinypcminfo; do
 aarch64-linux-gnu-gcc -static -O2 -Iinclude "utils/$prog.c" src/libtinyalsa.a -ldl \
  -o "/root/nabu-package/port/out/$prog"
done
