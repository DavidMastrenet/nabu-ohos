#!/system/bin/sh
set -eu
export PATH=/system/bin:/sbin:/bin:$PATH
out=/tmp/nabu-android-input
mkdir -p "$out/vendor" "$out/modem"
twrp mount /vendor >/dev/null 2>&1 || mount /vendor 2>/dev/null || true
found=0
for src in /vendor/firmware /vendor/firmware_mnt /vendor/etc/sensors /vendor/etc/firmware;do
 [ -d "$src" ] || continue
 cp -aL "$src" "$out/vendor/"
 found=1
done
for label in modem_a modem;do
 dev=/dev/block/bootdevice/by-name/$label
 [ -e "$dev" ] || continue
 mkdir -p /tmp/nabu-modem
 if mount -o ro "$dev" /tmp/nabu-modem;then
  [ ! -d /tmp/nabu-modem/image ] || cp -aL /tmp/nabu-modem/image "$out/modem/"
  umount /tmp/nabu-modem;found=1;break
 fi
done
[ "$found" = 1 ] || { echo 'Mount vendor in Recovery or supply firmware extracted from your own MIUI package.';exit 1; }
tar -czf /tmp/nabu-android-input.tar.gz -C "$out" .
echo ANDROID_INPUT_CAPTURED
