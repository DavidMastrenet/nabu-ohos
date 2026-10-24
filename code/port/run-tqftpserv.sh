#!/debug/busybox sh
mkdir -p /data/vendor/tmp/tqftpserv
exec /debug/tqftpserv -d >> /data/local/tmp/tqftpserv.log 2>&1
