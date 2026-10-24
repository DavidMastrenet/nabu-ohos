#!/bin/busybox sh
rm -f /state/ohos/start-native
exec > /state/ohos/logs/native-init.log 2>&1
set -ex
echo 'Switching real PID1 to HarmonyOS'
# The EFI RTC is unreadable on nabu; PM8XXX is the battery-backed clock.
[ ! -e /dev/rtc1 ] || /bin/busybox hwclock -s -u -f /dev/rtc1 || true
mkdir -p /harmony /state/ohos/root
mountpoint -q /state/ohos/root || mount -o loop,rw /state/ohos/images/system.img /state/ohos/root
for volume in vendor sys_prod; do
 mountpoint -q /state/ohos/root/$volume || mount -o loop,rw /state/ohos/images/$volume.img /state/ohos/root/$volume
done
data_device=''
for entry in /sys/class/block/*/uevent; do
 if grep -qs '^PARTNAME=userdata$' "$entry"; then
  data_device=/dev/$(sed -n 's/^DEVNAME=//p' "$entry")
  break
 fi
done
if [ -z "$data_device" ] && [ -f /state/ohos/physical-userdata ]; then
 data_device=$(cat /state/ohos/physical-userdata)
fi
if ! mountpoint -q /state/ohos/root/data; then
 if [ -n "$data_device" ]; then
  mount -t ext4 -o rw "$data_device" /state/ohos/root/data
 else
  mount -o loop,rw /state/ohos/images/userdata.img /state/ohos/root/data
 fi
fi
mount --move /state/ohos/root /harmony
mount -t tmpfs -o mode=755 tmpfs /harmony/mnt
/sbin/mount-propagation slave /harmony/mnt
mkdir -p /harmony/mnt/data
mount -t tmpfs -o mode=755 tmpfs /harmony/mnt/data
/sbin/mount-propagation shared /harmony/mnt/data
mount -t tmpfs -o mode=755,noexec,nodev,nosuid tmpfs /harmony/storage
mkdir -p /harmony/mnt/nabu-state /harmony/mnt/nabu-efi /harmony/debug
if mountpoint -q /recovery; then
 mkdir -p /harmony/debug/early-root
 mount --move /recovery /harmony/debug/early-root
fi
cp /bin/busybox /harmony/debug/busybox
cp /sbin/nabu-reboot /harmony/debug/nabu-reboot
cp /sbin/mount-propagation /harmony/debug/mount-propagation
insmod /sbin/ashmem.ko
chmod 0666 /dev/ashmem
fuse_module=/harmony/system/lib/modules/6.17.0-xiaomi-nabu-ohos/kernel/fs/fuse/fuse.ko
[ ! -f "$fuse_module" ] || insmod "$fuse_module"
ln -sf busybox /harmony/debug/modprobe
echo /debug/modprobe > /proc/sys/kernel/modprobe
if [ -e /sys/class/backlight/ktz8866-backlight/brightness ]; then
 chown 3036:3036 /sys/class/backlight/ktz8866-backlight/brightness
 chmod 0664 /sys/class/backlight/ktz8866-backlight/brightness
fi
mkdir -p /dev/block/by-name /dev/socket /dev/unix/socket
for item in /sys/class/block/loop*/loop/backing_file; do
 backing=$(cat "$item")
 case "$backing" in
  */ohos/images/system.img) label=system; v=vda;;
  */ohos/images/vendor.img) label=vendor; v=vdb;;
  */ohos/images/sys_prod.img) label=sys_prod; v=vdc;;
  */ohos/images/userdata.img) label=userdata; v=vdd;;
  *) continue;;
 esac
 block=${item%/loop/backing_file}; block=${block##*/}
 ln -sf /dev/$block /dev/block/$v
 ln -sf /dev/$block /dev/block/by-name/$label
done
if [ -n "$data_device" ]; then
 ln -sf "$data_device" /dev/block/vdd
 ln -sf "$data_device" /dev/block/by-name/userdata
fi
printf %s /data/vendor/firmware > /sys/module/firmware_class/parameters/path
mkdir -p /sys/fs/selinux
mount -t selinuxfs selinuxfs /sys/fs/selinux
dd if=/harmony/vendor/etc/selinux/prebuild_sepolicy/developer_policy of=/sys/fs/selinux/load bs=4M
printf 'u:r:init:s0\0' > /proc/self/attr/current || echo 'Init domain transition will be handled by Harmony init'
sync
mount --move /efi /harmony/mnt/nabu-efi
mount --move /state /harmony/mnt/nabu-state
mount --move /dev /harmony/dev
mount --move /proc /harmony/proc
mount --move /sys /harmony/sys
sync
exec switch_root /harmony /system/bin/init --second-stage 0
