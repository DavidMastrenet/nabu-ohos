#!/system/bin/sh
set -eu
export PATH=/system/bin:/sbin:/bin:$PATH
disk=${NABU_DISK:-/dev/block/sda}
stage=${NABU_STAGE:-/tmp/nabu-ohos}
runtime=/mnt/ohos-runtime
esp=/mnt/ohos-esp
getnum() { sgdisk -p "$disk" | awk -v label="$1" '$NF==label {print $1}'; }
partdev() { case "$disk" in *[0-9]) echo "${disk}p$1";; *) echo "${disk}$1";; esac; }
info() { sgdisk -i "$1" "$disk" | sed -n "s/^$2 sector: \([0-9]*\).*/\1/p"; }
layout() {
 [ "$(blockdev --getss "$disk")" = 4096 ] || { echo 'This layout requires 4096-byte UFS sectors';exit 1; }
 [ -z "$(getnum ohos_runtime)" ] || { echo 'Already has OHOS partitions; use deployment/update, not Android repartition';exit 1; }
 super=$(getnum super); data=$(getnum userdata)
 [ -n "$super" ] && [ -n "$data" ] || { echo 'Stock Android super/userdata not found';exit 1; }
 [ -z "$(getnum esp)" ] || { echo 'An ESP already exists; this command is for stock Android';exit 1; }
 first=$(info "$data" First);last=$(info "$data" Last)
 start=$(( (first+255)/256*256 ));esp_end=$((start+89600-1));data_start=$((esp_end+1))
 [ $((last-data_start+1)) -gt 2097152 ] || { echo 'Not enough room for OHOS userdata';exit 1; }
 new_data=$((data+1))
 [ -z "$(sgdisk -p "$disk" | awk -v n="$new_data" '$1==n{print $1}')" ] || { echo 'New partition number is occupied';exit 1; }
 echo "OHOS runtime: current super partition $super"
 echo "ESP: partition $data sectors $start..$esp_end (350 MiB)"
 echo "OHOS userdata: partition $new_data sectors $data_start..$last"
 echo 'Android super and userdata will be replaced.'
}
case "${1:-plan}" in
 plan) layout;;
 partition)
  [ "${2:-}" = --apply ] || { echo 'Use partition --apply after reading plan';exit 1; }
  layout
  if [ "${NABU_TEST_MODE:-0}" = 1 ];then
   case "$disk" in /dev/loop*) :;; *) echo 'Test mode requires a loop device';exit 1;; esac
  else
   for m in /data /sdcard /vendor /system_root /system /metadata /cust /linux /esp; do umount "$m" 2>/dev/null || true; done
  fi
  sgdisk -b "$stage/gpt-before.bin" "$disk"
  sgdisk --delete="$data" --new="$data:$start:$esp_end" --typecode="$data:ef00" --change-name="$data:esp" \
   --new="$new_data:$data_start:$last" --typecode="$new_data:8300" --change-name="$new_data:userdata" \
   --typecode="$super:8300" --change-name="$super:ohos_runtime" --attributes="$super:clear:60" "$disk"
  sync
  echo 'PARTITION_TABLE_WRITTEN: reboot to fastboot, then boot the supplied temporary Recovery again.';;
 mount)
  r=$(getnum ohos_runtime);e=$(getnum esp);u=$(getnum userdata)
  [ -n "$r" ] && [ -n "$e" ] && [ -n "$u" ]
  [ "$(info "$e" Last)" -lt "$(info "$u" First)" ]
  mkdir -p "$runtime" "$esp"
  if ! mountpoint -q "$runtime";then mkfs.ext4 -F -m 0 -L ohos_runtime "$(partdev "$r")";mount -t ext4 "$(partdev "$r")" "$runtime";fi
  if ! mountpoint -q "$esp";then mkfs.fat -F 32 -s 1 -n OHOS_EFI "$(partdev "$e")";mount -t vfat "$(partdev "$e")" "$esp";fi
  mkdir -p "$runtime/install" "$runtime/ohos/images" "$runtime/ohos/logs" "$runtime/ohos/ssh"
  echo RUNTIME_MOUNTED;;
 deploy)
  mountpoint -q "$runtime";mountpoint -q "$esp"
  for name in system vendor sys_prod;do
   gzip -dc "$runtime/install/$name.img.gz" > "$runtime/ohos/images/$name.img"
   e2fsck -fy "$runtime/ohos/images/$name.img" || [ $? -eq 1 ]
  done
  user=$(partdev "$(getnum userdata)")
  gzip -dc "$runtime/install/userdata.img.gz" > "$user"
  e2fsck -fy "$user" || [ $? -eq 1 ]
  resize2fs "$user"
  tune2fs -L userdata -m 0 "$user"
  cp "$runtime/install/debug_key.pub" "$runtime/ohos/ssh/authorized_keys"
  tar -xzf "$runtime/install/efi.tar.gz" -C "$esp"
  sync
  echo OHOS_DEPLOYED;;
 *) echo 'Commands: plan, partition --apply, mount, deploy';exit 1;;
esac
