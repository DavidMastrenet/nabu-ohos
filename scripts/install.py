"""Windows entry point; does not contain official HarmonyOS system images."""
from pathlib import Path
import argparse,gzip,json,os,shutil,subprocess,sys,tarfile,urllib.request
root=Path(__file__).resolve().parents[1];work=root/'work';work.mkdir(exist_ok=True)
p=argparse.ArgumentParser();p.add_argument('action',choices=['tools','recovery','capture','images','prepare','plan','partition','deploy','boot','status']);p.add_argument('--base',type=Path);p.add_argument('--hardware',type=Path);p.add_argument('--apply',action='store_true');p.add_argument('--proxy',default='http://127.0.0.1:7897');a=p.parse_args()
adb=root/'tools/platform-tools/adb.exe';fastboot=root/'tools/platform-tools/fastboot.exe'
def run(*cmd,**kw):print('>',*cmd,flush=True);return subprocess.run(list(map(str,cmd)),check=True,**kw)
def bridge(*cmd):return run(adb,*cmd)
def stage():
 bridge('wait-for-device');bridge('shell','mkdir -p /tmp/nabu-ohos');bridge('push',root/'scripts/recovery-install.sh','/tmp/nabu-ohos/install.sh')
def wslpath(path):return subprocess.check_output(['wsl','-u','root','--','wslpath','-u',str(path.resolve())],text=True).strip()
if a.action=='tools':
 import zipfile
 op=urllib.request.build_opener(urllib.request.ProxyHandler({'https':a.proxy,'http':a.proxy}))
 target=work/'platform-tools.zip'
 with op.open('https://dl.google.com/android/repository/platform-tools-latest-windows.zip',timeout=60) as r,target.open('wb') as f:shutil.copyfileobj(r,f)
 with zipfile.ZipFile(target) as z:z.extractall(root/'tools')
 print('Android platform tools ready')
elif a.action=='recovery':
 product=subprocess.run([str(fastboot),'getvar','product'],capture_output=True,text=True,check=True)
 if 'nabu' not in product.stdout+product.stderr:raise SystemExit('Expected Xiaomi Pad 5 nabu')
 run(fastboot,'boot',root/'boot/recovery-nabu.img');bridge('wait-for-device')
elif a.action=='capture':
 stage();backup=work/'android-backup';backup.mkdir(exist_ok=True)
 # Host writes binary output directly; PowerShell text pipelines must not carry images.
 for name in ['boot_a','boot_b','dtbo_a','dtbo_b','vendor_boot_a','vendor_boot_b','vbmeta_a','vbmeta_b']:
  with (backup/(name+'.img')).open('wb') as output:run(adb,'exec-out','dd','if=/dev/block/bootdevice/by-name/'+name,'bs=1048576',stdout=output)
 bridge('shell','sgdisk -b /tmp/nabu-ohos/gpt-before.bin /dev/block/sda');bridge('pull','/tmp/nabu-ohos/gpt-before.bin',backup/'gpt-before.bin')
 bridge('push',root/'scripts/capture-android.sh','/tmp/nabu-ohos/capture.sh');bridge('shell','sh /tmp/nabu-ohos/capture.sh');bridge('pull','/tmp/nabu-android-input.tar.gz',work/'android-input.tar.gz')
 capture=work/'android-input';capture.mkdir(exist_ok=True)
 with tarfile.open(work/'android-input.tar.gz') as t:t.extractall(capture,filter='data')
 run(sys.executable,root/'scripts/import-hardware.py','--input',capture,'--output',work/'hardware')
elif a.action=='images':
 if not a.base:raise SystemExit('Supply --base <your local DevEco ARM64 tablet image folder>')
 hardware=a.hardware or work/'hardware'
 run('wsl','-u','root','--','python3',wslpath(root/'scripts/apply-images.py'),'--base',wslpath(a.base),'--hardware',wslpath(hardware),'--output',wslpath(work/'images'))
 for image in (work/'images').glob('*.img'):
  print('Compressing',image.name,flush=True)
  with image.open('rb') as x,gzip.open(image.with_suffix('.img.gz'),'wb',compresslevel=1) as y:shutil.copyfileobj(x,y,1024*1024)
elif a.action=='prepare':
 key=work/'debug_key';sshkeygen=root/'tools/openssh/ssh-keygen.exe'
 if not key.exists():run(sshkeygen,'-q','-t','ed25519','-N','','-f',key)
 firmware=a.hardware or work/'hardware';dst=root/'code/port/firmware'
 for name in ['qcom/a630_sqe.fw','qcom/a640_gmu.bin','qcom/sm8150/xiaomi/nabu/a640_zap.mbn','novatek/novatek_nt36523_fw.bin']:
  d=dst/name;d.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(firmware/'firmware'/name,d)
 generated=root/'code/port/keys';generated.mkdir(exist_ok=True);shutil.copyfile(key.with_suffix('.pub'),generated/'nabu_debug_ed25519.pub')
 efi=work/'efi';efi.mkdir(exist_ok=True)
 with tarfile.open(root/'boot/efi-assets.tar.gz') as t:t.extractall(efi,filter='data')
 harmony=efi/'EFI/HarmonyOS';harmony.mkdir(exist_ok=True)
 for filename,extra in [('nabu-ohos-standalone.efi',[]),('nabu-ohos-recovery.efi',['--recovery-only'])]:
  run(sys.executable,root/'code/tools/build_uki.py','--output-name',filename,'--dtb',root/'code/port/out/nabu-ufs-slowdown.dtb',*extra)
  shutil.copyfile(root/'code/port/out'/filename,harmony/filename)
 with tarfile.open(work/'efi.tar.gz','w:gz') as t:t.add(efi/'EFI',arcname='EFI')
 print('Prepared UEFI files and this PC\'s SSH key in work/')
elif a.action in ['plan','partition']:
 stage();bridge('shell','sh /tmp/nabu-ohos/install.sh plan')
 if a.action=='partition':
  if not a.apply:raise SystemExit('Partition plan printed. Run partition --apply to execute.')
  if not(work/'android-backup/gpt-before.bin').exists():raise SystemExit('Run capture before partitioning')
  bridge('shell','sh /tmp/nabu-ohos/install.sh partition --apply');print('Next target: FASTBOOT, then temporary Recovery.');bridge('reboot','bootloader')
elif a.action=='deploy':
 stage();bridge('shell','sh /tmp/nabu-ohos/install.sh mount')
 for name in ['system','vendor','sys_prod','userdata']:bridge('push',work/'images'/(name+'.img.gz'),'/mnt/ohos-runtime/install/')
 bridge('push',work/'debug_key.pub','/mnt/ohos-runtime/install/');bridge('push',work/'efi.tar.gz','/mnt/ohos-runtime/install/')
 bridge('shell','sh /tmp/nabu-ohos/install.sh deploy')
 print('Next target: FASTBOOT, to install the UEFI boot image.');bridge('reboot','bootloader')
elif a.action=='boot':
 for slot in ['a','b']:run(fastboot,'flash','boot_'+slot,root/'boot/uefi-nabu.img')
 run(fastboot,'set_active','a');print('Next target: OHOS.');run(fastboot,'reboot')
else:
 ssh=root/'tools/openssh/ssh.exe'
 run(ssh,'-i',work/'debug_key','-p','2222','root@172.30.55.1','param get bootevent.boot.completed; df -h /data; pidof render_service')
