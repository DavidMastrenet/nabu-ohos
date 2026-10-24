#!/bin/busybox sh
export PATH=/bin:/sbin
/bin/busybox --install -s /bin
trap 'echo "NABU PID1 handoff stopped; debug USB remains available" > /dev/kmsg; sync; while true; do sleep 60; done' EXIT
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t devtmpfs devtmpfs /dev
mkdir -p /dev/pts /run /tmp /state /efi /sys/kernel/config
mount -t devpts devpts /dev/pts
echo 'NABU initramfs entered; starting independent USB recovery' > /dev/kmsg
/sbin/early-usb
echo 'NABU OHOS KERNEL: initramfs entered'
uname -a
for i in $(seq 1 180); do
 found=''
 for label in ohos_runtime; do
  for entry in /sys/class/block/*/uevent; do
   if grep -qs "^PARTNAME=$label\$" "$entry"; then
    found=$(sed -n 's/^DEVNAME=//p' "$entry")
    break
   fi
  done
  [ -z "$found" ] || break
 done
 [ -n "$found" ] && break
 sleep 1
done
if [ -n "$found" ]; then
 mount -t ext4 -o rw "/dev/$found" /state
 mkdir -p /state/ohos/logs
 dmesg > /state/ohos/logs/native-kernel.log
fi
for entry in /sys/class/block/*/uevent; do
 if grep -qs '^PARTNAME=esp$' "$entry"; then
  espdev=$(sed -n 's/^DEVNAME=//p' "$entry")
  mount -t vfat "/dev/$espdev" /efi
  break
 fi
done
if grep -qw nabu.recovery=1 /proc/cmdline; then
 touch /run/nabu-force-recovery
fi
echo 'NABU storage stage complete; RAM USB SSH2223 remains available' > /dev/kmsg
# The initial fbcon modeset can run before both DSI links settle. Re-run
# panel power sequencing once probing and USB setup have finished.
if [ -w /sys/class/graphics/fb0/blank ]; then
 echo 4 > /sys/class/graphics/fb0/blank
 sleep 1
 echo 0 > /sys/class/graphics/fb0/blank
fi
while true; do
 if [ -f /state/ohos/images/system.img ] && [ ! -e /state/ohos/hold-native ] && [ ! -e /run/nabu-force-recovery ]; then
  . /sbin/start-native
 fi
 sleep 1
done
