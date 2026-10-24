#!/debug/busybox sh
set -eu
export PATH=/debug:/system/bin:/bin
mkdir -p /data/vendor/rmtfs /data/vendor/sensors/socinfo
# Read this tablet's own EFS partitions; never include another tablet's EFS.
copy_efs() {
 label=$1; destination=$2
 [ ! -s "/data/vendor/rmtfs/$destination" ] || return 0
 for info in /sys/class/block/*/uevent; do
  grep -qx "PARTNAME=$label" "$info" || continue
  node=$(sed -n 's/^DEVNAME=//p' "$info")
  /debug/busybox dd if="/dev/$node" of="/data/vendor/rmtfs/$destination" bs=1048576
  return 0
 done
 echo "EFS partition missing: $label" >&2; return 1
}
copy_efs modemst1 modem_fs1
copy_efs modemst2 modem_fs2
copy_efs fsg modem_fsg
copy_efs fsc modem_fsc
soc=/data/vendor/sensors/socinfo
echo 339 > "$soc/soc_id"
echo 48 > "$soc/hw_platform"
echo 0 > "$soc/platform_subtype"
echo 0 > "$soc/platform_subtype_id"
echo 1638400 > "$soc/platform_version"
echo 2.2 > "$soc/revision"
mkdir -p /data/vendor/sensors/sensors/registry
chmod 0755 /data/vendor /data/vendor/sensors /data/vendor/sensors/sensors
