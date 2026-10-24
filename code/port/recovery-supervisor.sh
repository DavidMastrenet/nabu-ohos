#!/bin/busybox sh
# Runs entirely in the recovery tmpfs, including after switch_root.
export PATH=/bin:/sbin
ssh_pid=''
dhcp_pid=''
finish() {
 [ -z "$ssh_pid" ] || kill "$ssh_pid" 2>/dev/null
 [ -z "$dhcp_pid" ] || kill "$dhcp_pid" 2>/dev/null
 exit 0
}
trap finish TERM INT
while true; do
 # NetManager may replace network policy while Wi-Fi starts or changes.
 # Keep the existing gadget; only maintain its address and return route.
 if [ -e /sys/class/net/usb0 ]; then
  ip -o address show dev usb0 | grep -q 'inet 172.30.55.1/30 ' ||
   ip address add 172.30.55.1/30 dev usb0
  ip link show dev usb0 | grep -q '<[^>]*UP[,>]' || ip link set usb0 up
  ip rule show | grep -q '9000:.*to 172.30.55.0/30 lookup main' ||
   ip rule add pref 9000 to 172.30.55.0/30 lookup main
 fi
 if [ -f /etc/usb-dhcp.conf ] && { [ -z "$dhcp_pid" ] || ! kill -0 "$dhcp_pid" 2>/dev/null; }; then
  [ -z "$dhcp_pid" ] || wait "$dhcp_pid" 2>/dev/null
  /bin/busybox udhcpd -f -a 50 /etc/usb-dhcp.conf >>/run/usb-dhcp.log 2>&1 </dev/null &
  dhcp_pid=$!
 fi
 if [ -z "$ssh_pid" ] || ! kill -0 "$ssh_pid" 2>/dev/null; then
  [ -z "$ssh_pid" ] || wait "$ssh_pid" 2>/dev/null
  /sbin/dropbearmulti dropbear -F -r /sbin/early_host_ed25519 -D /root/.ssh \
   -p 2223 -P /run/nabu-early-ssh.pid >>/run/nabu-early-ssh.log 2>&1 </dev/null &
  ssh_pid=$!
  echo "Recovery SSH supervisor started child $ssh_pid"
 fi
 sleep 3
done
