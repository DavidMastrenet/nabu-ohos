#!/debug/busybox sh
exec /debug/rmtfs -v -o /data/vendor/rmtfs >> /data/local/tmp/rmtfs.log 2>&1
