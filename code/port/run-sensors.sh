#!/debug/busybox sh
for pass in $(/debug/busybox seq 1 30); do
 [ ! -e /dev/fastrpc-sdsp ] || break
 /debug/busybox sleep 1
done
for r in /sys/class/remoteproc/remoteproc*; do
 if [ "$(cat "$r/name")" = slpi ]; then
  echo disabled > "/sys/kernel/debug/remoteproc/${r##*/}/recovery"
 fi
done
exec /debug/hexagonrpcd -f /dev/fastrpc-sdsp -d sdsp -s -R /data/vendor/sensors >> /data/local/tmp/hexagonrpcd-native.log 2>&1
