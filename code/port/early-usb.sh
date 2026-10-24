#!/bin/busybox sh
# This runs from RAM before waiting for UFS or mounting the system images.
export PATH=/bin:/sbin
mkdir -p /sys/kernel/config /sys/kernel/debug /run
mount -t debugfs debugfs /sys/kernel/debug 2>/dev/null || true
mount -t configfs configfs /sys/kernel/config 2>/dev/null || true
# a600000 is the external USB-C port; a800000 is the keyboard host port.
mode=/sys/kernel/debug/usb/a600000.usb/mode
if [ -w "$mode" ] && [ "$(cat "$mode")" != device ]; then
 echo device > "$mode"
fi
g=/sys/kernel/config/usb_gadget/nabu_ssh
mkdir -p "$g"
echo 0x1d6b > "$g/idVendor"
echo 0x0104 > "$g/idProduct"
echo 0x0200 > "$g/bcdUSB"
echo 0x0100 > "$g/bcdDevice"
echo 0xEF > "$g/bDeviceClass"
echo 0x02 > "$g/bDeviceSubClass"
echo 0x01 > "$g/bDeviceProtocol"
mkdir -p "$g/strings/0x409" "$g/configs/c.1/strings/0x409"
echo nabu-ohos > "$g/strings/0x409/serialnumber"
echo Nabu > "$g/strings/0x409/manufacturer"
echo 'Nabu OHOS debug' > "$g/strings/0x409/product"
echo 'USB network and serial' > "$g/configs/c.1/strings/0x409/configuration"
echo 250 > "$g/configs/c.1/MaxPower"
mkdir -p "$g/functions/rndis.usb0" "$g/functions/acm.gs0"
echo 02:4e:41:42:55:01 > "$g/functions/rndis.usb0/dev_addr"
echo 02:4e:41:42:55:02 > "$g/functions/rndis.usb0/host_addr"
echo 1 > "$g/os_desc/use"
echo 0xcd > "$g/os_desc/b_vendor_code"
echo MSFT100 > "$g/os_desc/qw_sign"
echo RNDIS > "$g/functions/rndis.usb0/os_desc/interface.rndis/compatible_id"
echo 5162001 > "$g/functions/rndis.usb0/os_desc/interface.rndis/sub_compatible_id"
ln -s "$g/functions/rndis.usb0" "$g/configs/c.1/rndis.usb0"
ln -s "$g/functions/acm.gs0" "$g/configs/c.1/acm.gs0"
ln -s "$g/configs/c.1" "$g/os_desc/c.1"
for attempt in $(seq 1 30); do
 udc=$(ls /sys/class/udc | head -n 1)
 [ -n "$udc" ] && break
 sleep 1
done
if [ -n "$udc" ]; then
 echo "$udc" > "$g/UDC"
 ip address add 172.30.55.1/30 dev usb0
 ip link set usb0 up
 ip rule add pref 9000 to 172.30.55.0/30 lookup main
 # A separate tmpfs survives switch_root's deletion of the old initramfs.
 r=/recovery
 mkdir -p "$r"
 mount -t tmpfs -o mode=0700,size=64m tmpfs "$r"
 mkdir -p "$r/bin" "$r/sbin" "$r/etc" "$r/root" "$r/dev" "$r/proc" "$r/sys" "$r/run"
 cp -a /bin/. "$r/bin/"
 cp -a /etc/. "$r/etc/"
 cp -a /root/. "$r/root/"
 cp /sbin/dropbearmulti "$r/sbin/"
 if [ -d /tools ]; then
  cp -a /tools "$r/tools"
  for directory in dev proc sys; do
   mount --bind "/$directory" "$r/tools/$directory"
  done
 fi

 mount --bind /dev "$r/dev"
 mount --bind /proc "$r/proc"
 mount --bind /sys "$r/sys"
 chroot "$r" /sbin/dropbearmulti dropbearkey -t ed25519 -f /sbin/early_host_ed25519 >/dev/null
 chmod 0600 "$r/sbin/early_host_ed25519"
 chroot "$r" /bin/recovery-supervisor >"$r/run/recovery-supervisor.log" 2>&1 </dev/null &
 chroot "$r" /bin/sh -i </dev/ttyGS0 >/dev/ttyGS0 2>&1 &
 echo 'NABU early USB SSH2223 ready; waiting for storage' >/dev/kmsg
fi
