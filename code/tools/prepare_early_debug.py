"""Build the public recovery root using only this installer's generated public key."""
from pathlib import Path
import stat
w=Path(__file__).resolve().parent.parent
def recovery_entries():
 entries=[]
 for name in ['bin','sbin','etc','root','root/.ssh','dev','proc','sys','run','tmp']:
  entries.append((name,b'',stat.S_IFDIR|(0o700 if name.startswith('root') else 0o755)))
 entries += [
 ('bin/busybox',(w/'tools/busybox-aarch64').read_bytes(),stat.S_IFREG|0o755),
 ('bin/sh',b'busybox',stat.S_IFLNK|0o777),
 ('sbin/dropbearmulti',(w/'port/out/dropbearmulti').read_bytes(),stat.S_IFREG|0o755),
 ('root/.ssh/authorized_keys',(w/'port/keys/nabu_debug_ed25519.pub').read_bytes(),stat.S_IFREG|0o600),
 ('etc/passwd',b'root:x:0:0:Nabu recovery:/root:/bin/recovery-shell\n',stat.S_IFREG|0o644),
 ('etc/group',b'root:x:0:\n',stat.S_IFREG|0o644),
 ('etc/shells',b'/bin/sh\n/bin/recovery-shell\n',stat.S_IFREG|0o644),
 ('bin/recovery-shell',b'#!/bin/busybox sh\nexport PATH=/bin:/sbin\nexec /bin/sh "$@"\n',stat.S_IFREG|0o755),
 ('bin/recovery-supervisor',(w/'port/recovery-supervisor.sh').read_bytes().replace(b'\r\n',b'\n'),stat.S_IFREG|0o755),
 ('etc/usb-dhcp.conf',(w/'port/recovery-dhcp.conf').read_bytes().replace(b'\r\n',b'\n'),stat.S_IFREG|0o644),
 ('sbin/early-usb',(w/'port/early-usb.sh').read_bytes().replace(b'\r\n',b'\n'),stat.S_IFREG|0o755),
 ]
 return entries
