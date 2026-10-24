#!/debug/busybox sh
export PATH=/debug:/system/bin:/bin
log=/data/local/tmp/nabu-hardware-load.log
exec >> "$log" 2>&1
echo "Hardware load $(date)"
# NetManager's Wi-Fi policy must not send USB debug replies through wlan0.
ip rule show | grep -q '9000:.*to 172.30.55.0/30 lookup main' ||
 ip rule add pref 9000 to 172.30.55.0/30 lookup main
[ ! -e /dev/rtc1 ] || /debug/busybox hwclock -s -u -f /dev/rtc1
printf %s /data/vendor/firmware > /sys/module/firmware_class/parameters/path
for module in usbhid hid_generic fuse qrtr qcom_pd_mapper qcom_q6v5_pas qcom_glink_smem ath10k_snoc snd_soc_sm8150; do
 /debug/busybox modprobe "$module"
done
param set ohos.ctl.start nabu-rmtfs
param set ohos.ctl.start nabu-tqftpserv
sleep 1
for rproc in /sys/class/remoteproc/remoteproc*; do
 [ "$(cat "$rproc/name")" = modem ] || continue
 [ "$(cat "$rproc/state")" != offline ] || echo start > "$rproc/state"
done
for pass in 1 2 3 4; do
 before=$(wc -l < /proc/modules)
 for bus in platform i2c spi slimbus soundwire aprbus rpmsg auxiliary usb hid; do
  for device in /sys/bus/$bus/devices/*; do
   [ -d "$device" ] || continue
   [ -L "$device/driver" ] && continue
   if [ -f "$device/modalias" ]; then
    alias=$(cat "$device/modalias")
   else
    alias=$(sed -n 's/^MODALIAS=//p' "$device/uevent" 2>/dev/null)
   fi
   [ -n "$alias" ] || continue
   /debug/busybox modprobe "$alias"
  done
 done
 after=$(wc -l < /proc/modules)
 echo "pass=$pass modules=$after"
 [ "$before" = "$after" ] && break
 sleep 1
done
param set ohos.ctl.start nabu-sensors
param set ohos.ctl.start nabu-motion
param set ohos.ctl.start nabu-storage
# ALSA nodes appear after the DSP and codec drivers bind.
for node in /dev/snd/*; do
 [ -e "$node" ] || continue
 chown system:audio "$node"
 chmod 0660 "$node"
done
# Initialize the real amplifier route before the first framework playback.
if [ -e /dev/snd/controlC0 ] && [ -x /debug/setup-audio.sh ]; then
 /debug/busybox sh /debug/setup-audio.sh
 param set ohos.ctl.start audio_host
 param set ohos.ctl.start audio_server
fi
