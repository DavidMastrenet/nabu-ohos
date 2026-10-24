# 从 Android 安装 OHOS

适用小米平板 5 Wi-Fi 版，代号 `nabu`，Bootloader 已解锁。电脑使用 Windows 和 WSL2。全程从 Android 开始，最终保留 OHOS；会替换 Android 的 super 和 userdata，清空原应用与文件。固件和校准分区保留。

准备 DevEco 的 **ARM64 平板模拟器镜像 7.0.0.107，SP8，API 26**。本地目录应包含：

```text
system.img
vendor.img
sys_prod.img
```

直接刷机采用预编译适配组件。WSL 负责挂载 ext4、应用补丁和生成本地镜像。

## 1. 准备电脑

将本包放在短路径，例如 `D:\nabu-ohos-patchkit`，在该目录打开 PowerShell。

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\tools.ps1
```

这一步从 Google 下载 ADB/Fastboot，默认使用 `http://127.0.0.1:7897` 代理。修改代理地址：

```powershell
.\tools.ps1 --proxy http://127.0.0.1:7897
```

WSL 中安装文件系统工具。外网请求经过 Windows 的 7897 代理：

```bash
NABU_GATEWAY=$(ip route show default | awk '{print $3; exit}')
export http_proxy="http://$NABU_GATEWAY:7897"
export https_proxy="$http_proxy"
sudo -E apt-get update
sudo -E apt-get install -y python3 e2fsprogs dosfstools util-linux gzip wireless-regdb
```

电脑预留至少 20 GB 空间，生成文件保存在本机 `work/`。

## 2. 启动临时 Recovery，备份自己的 Android 输入

让平板进入 fastboot 并用 USB 连接电脑，运行：

```powershell
.\recovery.ps1
.\capture.ps1
```

第一个命令检查设备代号并临时启动随包的 nabu Recovery。第二个命令备份本机引导分区和分区表，并从自己的 Android 固件提取硬件输入。

结果：

```text
work/android-backup/       原机 boot、dtbo、vendor_boot、vbmeta、分区表
work/android-input/        从自己的 Android 提取的固件和传感器配置
work/hardware/             整理后的 nabu 硬件输入
```

分区前完成这一步，并保留电脑上的备份。

如果 Recovery 未挂载 vendor，可在 Recovery 的 Mount 页面挂载 Vendor 后重跑 `capture.ps1`。也可以把自己解包的 MIUI/HyperOS 固件目录交给导入脚本：

```powershell
.\tools\python\python.exe .\scripts\import-hardware.py --input D:\my-nabu-firmware --output .\work\hardware
```

脚本支持完整 `.mbn` 和 `.mdt` 加 `.b00` 等分段固件。硬件文件目录见 [输入文件说明](05-输入文件.md)。

## 3. 在电脑生成 OHOS 镜像和引导文件

将下方目录替换为自己取得的原始模拟器镜像目录：

```powershell
.\images.ps1 --base D:\my-sdk\system-image-tablet-arm64
.\prepare.ps1
```

`images.ps1` 复制三份原始镜像并应用补丁，再新建 userdata。

`prepare.ps1` 为这台电脑生成 SSH 密钥，组装两份 EFI 引导文件。恢复 SSH 的主机密钥在目标设备的 RAM 中生成，原生 OHOS 的主机密钥在首次启动时生成。

生成结果：

```text
work/images/system.img.gz
work/images/vendor.img.gz
work/images/sys_prod.img.gz
work/images/userdata.img.gz
work/efi.tar.gz
work/debug_key
work/debug_key.pub
```

生成失败时先处理报告的缺失输入。上面四份镜像和 EFI 都生成成功，再继续分区。

## 4. 分区

平板仍在临时 Recovery。先查看按该设备分区表生成的计划：

```powershell
.\plan.ps1
```

布局为：

| 区域 | 用途 |
| --- | --- |
| 原 super | 改成 `ohos_runtime`，存放三个 OHOS 镜像 |
| 原 userdata 开头 350 MiB | 新建 FAT32 `esp`，存放 UEFI 引导文件 |
| 原 userdata 剩余空间 | OHOS 物理 `userdata` |

脚本读取目标平板 userdata 的起止位置，适用于 nabu 的 128 GB、256 GB 存储规格，要求 UFS 逻辑扇区为 4096 字节。

执行计划：

```powershell
.\partition.ps1 --apply
```

完成后目标是 **fastboot**。重新启动临时 Recovery，让内核重新读取新分区表：

```powershell
.\recovery.ps1
```

## 5. 部署与首次启动

```powershell
.\deploy.ps1
```

该命令格式化新建的运行分区和 ESP，将本地生成的镜像写入，扩展 OHOS userdata 到物理分区容量。完成后目标是 **fastboot**。

最后安装 UEFI 引导：

```powershell
.\boot.ps1
```

此步骤向 `boot_a`、`boot_b` 写入随包的 nabu UEFI 引导镜像，激活 a 槽并重启。目标是 **OHOS**，菜单里只有 `OHOS` 和 `OHOS recovery`。

首次启动会初始化系统应用和用户数据。USB 网络的设备地址为 `172.30.55.1`，电脑通过 DHCP 获取 `172.30.55.2`。

## 6. 确认结果

```powershell
.\status.ps1
```

检查桌面、触摸、亮度滑块、Wi-Fi、音量、自动旋转、键盘和存储容量。当前适配状态及尚未解决的问题见 [适配说明](04-适配状态.md)。

验证范围：现有真机运行功能、本地补丁生成和脚本检查。Android 全新刷机待真机复现。
