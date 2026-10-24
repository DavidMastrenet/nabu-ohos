#!/debug/busybox sh
set -eu
base=/data/service/el1/public/dropbear
pub=/mnt/nabu-state/ohos/ssh/authorized_keys
mkdir -p "$base/root/.ssh"
chmod 0700 "$base" "$base/root" "$base/root/.ssh"
for attempt in $(/debug/busybox seq 1 60); do
 [ ! -s "$pub" ] || break
 /debug/busybox sleep 1
done
[ -s "$pub" ] || exit 1
/debug/busybox cp "$pub" "$base/root/.ssh/authorized_keys"
chmod 0600 "$base/root/.ssh/authorized_keys"
if [ ! -s "$base/host_ed25519" ]; then
 /debug/dropbearmulti dropbearkey -t ed25519 -f "$base/host_ed25519" >/dev/null
fi
exec /debug/dropbearmulti dropbear -F -s -r "$base/host_ed25519" -D "$base/root/.ssh" -p 2222
