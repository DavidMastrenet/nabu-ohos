#!/debug/busybox sh
export PATH=/debug:/system/bin:/bin
exec >>/data/local/tmp/nabu-storage.log 2>&1
# Use the upstream local-storage layout when the kernel has no HMDFS.
for attempt in $(seq 1 60); do
 account=/data/service/el2/100/hmdfs/account
 if [ -d "$account/files" ]; then
  mkdir -p /storage/media/100/local /storage/cloud/100
  if ! grep -qw hmdfs /proc/filesystems; then
   mountpoint -q /storage/media/100/local || mount --bind "$account" /storage/media/100/local
   mountpoint -q /storage/cloud/100 || mount --bind "$account" /storage/cloud/100
  fi
  # The emulator's default user has no credential. Do not apply this to a PIN user.
  info=/data/service/el1/public/account/100/account_info.json
  if [ -r "$info" ] && grep -q '"credentialId"[[:space:]]*:[[:space:]]*"0"' "$info"; then
   /debug/account-state --mark-unlocked && exit 0
  else
   echo 'Local storage ready; account has a credential or is not ready'
   exit 0
  fi
 fi
 sleep 2
done
echo 'Account/storage initialization did not finish within the startup window'
