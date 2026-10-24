#!/bin/bash
set -euo pipefail
[ "$#" -eq 4 ] || { echo 'Usage: bootstrap-build.sh KIT LOCAL_SYSTEM_IMG LOCAL_SYSROOT LOCAL_LINUX_CLANG_ARCHIVE';exit 1; }
kit=$(realpath "$1");system=$(realpath "$2");sysroot=$(realpath "$3");clang=$(realpath "$4")
[ "$(id -u)" -eq 0 ] || { echo 'Run as root in WSL';exit 1; }
[ ! -e /root/nabu-build ] && [ ! -e /root/nabu-package ] || { echo 'Build directories already exist. Use a clean WSL build environment for bootstrap.';exit 1; }
cp -a "$kit/code" /root/nabu-package
mkdir -p /root/nabu-build/linux-6.17 /root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908
tar -xzf "$kit/code/upstream/linux-sm8150-6.17.tar.gz" --strip-components=1 -C /root/nabu-build/linux-6.17
for patch in ohos-linux-6.17.patch ohos-spm-api26.patch nabu-battery-name.patch;do
 patch -d /root/nabu-build/linux-6.17 -p1 < "$kit/code/port/patches/$patch"
done
cp "$kit/code/port/out/kernel-ohos.config" /root/nabu-build/linux-6.17/.config
tar -xzf "$kit/code/upstream/build-dependencies.tar.gz" -C /root/nabu-build
tar -xf "$clang" --strip-components=1 -C /root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908
cp -a "$sysroot" /root/nabu-build/ohos-sysroot
extract() { mkdir -p "/root/nabu-build/$2";tar -xf "$kit/code/upstream/$1" --strip-components=1 -C "/root/nabu-build/$2"; }
extract busybox.tar.bz2 busybox-1.37.0
extract dropbear-source.tar.gz dropbear
extract hexagonrpc.tar.gz hexagonrpc-main
extract tinyalsa.tar.gz tinyalsa-master
extract qrtr.tar.gz qrtr-master
extract rmtfs.tar.gz rmtfs-master
extract tqftpserv.tar.gz tqftpserv-master
extract libudev-zero.tar.gz libudev-zero-master
mkdir -p /root/nabu-rootfs/system-lib /root/nabu-sdk-mount
mount -o loop,ro,noload "$system" /root/nabu-sdk-mount
trap 'umount /root/nabu-sdk-mount' EXIT
cp -a /root/nabu-sdk-mount/system/lib64 /root/nabu-rootfs/system-lib/
umount /root/nabu-sdk-mount
trap - EXIT
echo BUILD_INPUTS_READY
