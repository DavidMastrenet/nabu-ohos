import gzip
import pathlib
import stat
import struct
import sys
import argparse
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))

root = pathlib.Path(__file__).resolve().parent.parent
out = root / 'port/out'
out.mkdir(parents=True, exist_ok=True)
parser=argparse.ArgumentParser()
parser.add_argument('--output-name',default='nabu-harmony-bringup.efi')
parser.add_argument('--dtb',type=pathlib.Path,help='Explicit device tree override; the original is kept by default')
parser.add_argument('--efi-directory',default='/EFI/HarmonyOS',help='Loader directory used in the generated menu snippet')
parser.add_argument('--recovery-only',action='store_true')
args=parser.parse_args()
if pathlib.Path(args.output_name).name!=args.output_name:raise ValueError('Output name must be a filename')
efi_directory=pathlib.PurePosixPath(args.efi_directory)
if not efi_directory.is_absolute() or '..' in efi_directory.parts:raise ValueError('EFI directory must be an absolute firmware path')
align = lambda n, a: (n+a-1)//a*a

def cpio_entry(name, data, mode, inode):
    fields = [inode, mode, 0, 0, 1, 0, len(data), 0, 0, 0, 0, len(name.encode())+1, 0]
    header = b'070701' + ''.join(f'{n:08x}' for n in fields).encode()
    block = header + name.encode() + b'\0'
    block += bytes(align(len(block),4)-len(block))
    block += data
    block += bytes(align(len(data),4)-len(data))
    return block

files = [('bin', b'', stat.S_IFDIR|0o755), ('sbin',b'',stat.S_IFDIR|0o755), ('proc',b'',stat.S_IFDIR|0o755), ('sys',b'',stat.S_IFDIR|0o755), ('dev',b'',stat.S_IFDIR|0o755)]
files += [('bin/busybox',(root/'tools/busybox-aarch64').read_bytes(),stat.S_IFREG|0o755), ('bin/sh',b'busybox',stat.S_IFLNK|0o777), ('sbin/nabu-reboot',(out/'nabu-reboot').read_bytes(),stat.S_IFREG|0o755), ('init',(root/'port/bringup-init.sh').read_bytes().replace(b'\r\n',b'\n'),stat.S_IFREG|0o755)]
files.append(('sbin/start-native',(root/'port/start-native.sh').read_bytes().replace(b'\r\n',b'\n'),stat.S_IFREG|0o755))
files += [('sbin/mount-propagation',(out/'mount-propagation').read_bytes(),stat.S_IFREG|0o755), ('sbin/ashmem.ko',(out/'ashmem.ko').read_bytes(),stat.S_IFREG|0o644)]
from prepare_early_debug import recovery_entries
files += [entry for entry in recovery_entries() if entry[0] not in {item[0] for item in files}]
firmware=root/'port/firmware'
if firmware.exists():
    files += [('lib',b'',stat.S_IFDIR|0o755),('lib/firmware',b'',stat.S_IFDIR|0o755)]
    for p in sorted(firmware.rglob('*')):
        name='lib/firmware/'+p.relative_to(firmware).as_posix()
        files.append((name,b'' if p.is_dir() else p.read_bytes(),(stat.S_IFDIR|0o755) if p.is_dir() else (stat.S_IFREG|0o644)))
archive = b''.join(cpio_entry(n,d,m,i+1) for i,(n,d,m) in enumerate(files)) + cpio_entry('TRAILER!!!',b'',0,0)
ramdisk = gzip.compress(archive, mtime=0)
(out/(pathlib.Path(args.output_name).stem+'.initrd.img')).write_bytes(ramdisk)
efi = bytearray((root/'analysis/uki-template.efi').read_bytes())
pe = struct.unpack_from('<I',efi,60)[0]
count = struct.unpack_from('<H',efi,pe+6)[0]
optsize = struct.unpack_from('<H',efi,pe+20)[0]
opt = pe+24
section_table = opt+optsize
sections = []
for i in range(count):
    offset = section_table+i*40
    name, vs, va, rs, rp = struct.unpack_from('<8sIIII',efi,offset)
    sections.append((name.rstrip(b'\0'),vs,va,rs,rp,offset))
    if name.rstrip(b'\0') == b'.cmdline':
        cmd = b'console=tty0 rdinit=/init fw_devlink=permissive cma=256M loglevel=7 ignore_loglevel panic=5 lsm=capability,selinux selinux=1 enforcing=0 ohos.boot.hardware=hmos.emulator ohos.boot.selinux=permissive\0'
        if args.recovery_only:cmd=cmd.rstrip(b'\0')+b' nabu.recovery=1\0'
        if len(cmd)>rs: raise ValueError('cmdline too long')
        efi[rp:rp+rs] = cmd+bytes(rs-len(cmd))
        struct.pack_into('<I',efi,offset+8,len(cmd))
if args.dtb:
    device_tree=args.dtb.read_bytes()
    if len(device_tree)<40 or struct.unpack_from('>I',device_tree)[0]!=0xd00dfeed:
        raise ValueError('Invalid FDT header')
    if struct.unpack_from('>I',device_tree,4)[0]!=len(device_tree):
        raise ValueError('Device tree length does not match its header')
    index=next(i for i,s in enumerate(sections) if s[0]==b'.dtb')
    name,vs,va,rs,rp,offset=sections[index]
    if len(device_tree)>rs:raise ValueError('Device tree override exceeds the original PE allocation')
    efi[rp:rp+rs]=device_tree+bytes(rs-len(device_tree))
    struct.pack_into('<I',efi,offset+8,len(device_tree))
    sections[index]=(name,len(device_tree),va,rs,rp,offset)
sa, fa = struct.unpack_from('<II',efi,opt+32)
kernel_path = out/'Image-nabu-ohos'
if kernel_path.exists():
    kernel_image = kernel_path.read_bytes()
    index = next(i for i,s in enumerate(sections) if s[0] == b'.linux')
    name,vs,kva,old_rs,krp,offset = sections[index]
    if krp+old_rs != len(efi): raise ValueError('Kernel must be last original raw section')
    new_rs = align(len(kernel_image),fa)
    efi[krp:] = kernel_image+bytes(new_rs-len(kernel_image))
    struct.pack_into('<I',efi,offset+8,len(kernel_image))
    struct.pack_into('<I',efi,offset+16,new_rs)
    struct.pack_into('<I',efi,opt+8,struct.unpack_from('<I',efi,opt+8)[0]+new_rs-old_rs)
    sections[index] = (name,len(kernel_image),kva,new_rs,krp,offset)
    for s in sections:
        if s[0] == b'.uname':
            uname=b'6.17.0-xiaomi-nabu-ohos\0'
            if len(uname)>s[3]: raise ValueError('uname section too small')
            efi[s[4]:s[4]+s[3]]=uname+bytes(s[3]-len(uname))
            struct.pack_into('<I',efi,s[5]+8,len(uname))
va = align(max(s[2]+max(s[1],s[3]) for s in sections),sa)
rp = align(len(efi),fa)
rs = align(len(ramdisk),fa)
header = struct.pack('<8sIIIIIIHHI',b'.initrd\0',len(ramdisk),va,rs,rp,0,0,0,0,0x40000040)
headerpos = section_table+count*40
if headerpos+40 > min(s[4] for s in sections): raise ValueError('No room for PE section')
efi[headerpos:headerpos+40] = header
struct.pack_into('<H',efi,pe+6,count+1)
struct.pack_into('<I',efi,opt+56,align(va+len(ramdisk),sa))
struct.pack_into('<I',efi,opt+8,struct.unpack_from('<I',efi,opt+8)[0]+rs)
struct.pack_into('<I',efi,opt+64,0)
efi += bytes(rp-len(efi)) + ramdisk + bytes(rs-len(ramdisk))
(out/args.output_name).write_bytes(efi)
print(f'Built EFI: {len(efi)} bytes, initrd {len(ramdisk)} bytes, {count+1} sections')

print('UEFI image prepared for OHOS')
