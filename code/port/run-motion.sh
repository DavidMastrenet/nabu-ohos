#!/debug/busybox sh
export PATH=/debug:/system/bin:/bin
exec /debug/nabu-motion >>/data/local/tmp/nabu-motion.log 2>&1
