#!/bin/bash
set -euo pipefail
cd /root/nabu-build/hexagonrpc-main
python3 - <<'PY'
from pathlib import Path
p=Path('hexagonrpcd/rpcd.c')
s=p.read_text()
if 'setvbuf(stdout' not in s:
    pos=s.index('{',s.index('int main('))+1
    s=s[:pos]+'\n\tsetvbuf(stdout, NULL, _IOLBF, 0);\n\tsetvbuf(stderr, NULL, _IOLBF, 0);'+s[pos:]
    p.write_text(s)
PY
aarch64-linux-gnu-gcc -static -O2 -D_GNU_SOURCE -DHEXAGONRPC_VERBOSE \
 -Iinclude -Ihexagonrpcd libhexagonrpc/*.c libhexagonrpc/interface/*.c \
 hexagonrpcd/*.c hexagonrpcd/interface/*.c -pthread \
 -o /root/nabu-package/port/out/hexagonrpcd
echo HEXAGONRPC_NATIVE_BUILT
