"""Run under WSL as root; generate images from the installer's local SDK copy."""
from pathlib import Path
import argparse,copy,gzip,json,os,shutil,subprocess,tarfile
p=argparse.ArgumentParser();p.add_argument('--base',type=Path,required=True);p.add_argument('--hardware',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
a=p.parse_args();kit=Path(__file__).resolve().parents[1]
if os.geteuid()!=0:raise SystemExit('Run with wsl -u root')
def command(*args,accepted=(0,)): 
 r=subprocess.run(list(map(str,args)))
 if r.returncode not in accepted:raise RuntimeError((args,r.returncode))
def pointer(document,path):
 tokens=[x.replace('~1','/').replace('~0','~') for x in path.strip('/').split('/')]
 for token in tokens[:-1]:document=document[int(token)] if isinstance(document,list) else document[token]
 return document,tokens[-1]
def json_patch(doc,operations):
 for op in operations:
  parent,key=pointer(doc,op['path']);i=int(key) if isinstance(parent,list) else key
  if op['op']=='remove':parent.pop(i)
  elif op['op']=='add' and isinstance(parent,list):parent.insert(i,copy.deepcopy(op['value']))
  else:parent[i]=copy.deepcopy(op['value'])
 return doc
def apply(root,item):
 path=root/item['path'];action=item['action'];path.parent.mkdir(parents=True,exist_ok=True)
 if action=='remove':path.unlink(missing_ok=True);return
 if action=='symlink':
  if path.exists() or path.is_symlink():path.unlink()
  path.symlink_to(item['target']);return
 if action=='mkdir':path.mkdir(exist_ok=True)
 elif action=='new_text':path.write_text(item['text'])
 elif action=='json_patch':path.write_text(json.dumps(json_patch(json.loads(path.read_text()),item['operations']),ensure_ascii=False,indent=2)+'\n')
 elif action=='line_patch':
  lines=path.read_text().splitlines(True)
  for edit in reversed(item['edits']):
   i=edit['start_line'];old=edit['old'];assert lines[i:i+len(old)]==old, str(path)+' differs from the supported SDK release'
   lines[i:i+len(old)]=edit['new']
  path.write_text(''.join(lines))
 elif action=='binary_edits':
  data=bytearray(path.read_bytes())
  for edit in item['edits']:
   i=edit['offset'];before=bytes.fromhex(edit['before']);after=bytes.fromhex(edit['after'])
   assert data[i:i+len(before)]==before,str(path)+' differs from the supported SDK release'
   data[i:i+len(before)]=after
  path.write_bytes(data)
 if item.get('mode'):os.chmod(path,int(item['mode'],8))
 if item.get('uid') is not None:os.chown(path,item['uid'],item['gid'])

def main():
 for f in ['system.img','vendor.img','sys_prod.img']:
  if not(a.base/f).is_file():raise SystemExit('Missing local SDK image: '+str(a.base/f))
 for f in ['firmware/qcom/a630_sqe.fw','firmware/qcom/a640_gmu.bin','firmware/qcom/sm8150/xiaomi/nabu/a640_zap.mbn','firmware/novatek/novatek_nt36523_fw.bin','firmware/qcom/sm8150/xiaomi/nabu/slpi_nb.mbn','firmware/ath10k/WCN3990/hw1.0/board-2.bin']:
  if not(a.hardware/f).is_file():raise SystemExit('Missing hardware input: '+str(a.hardware/f))
 a.output.mkdir(parents=True,exist_ok=True);work=Path('/root/nabu-image-work');work.mkdir(exist_ok=True)
 changes=json.loads((kit/'patches/filesystem-changes.json').read_text())['changes'];mounts={}
 with tarfile.open(kit/'patches/open-runtime-overlay.tar.gz') as overlay:
  try:
   for name in ['system','vendor','sys_prod']:
    image=a.output/(name+'.img')
    if image.exists():raise SystemExit('Output already exists; choose a new output folder: '+str(image))
    command('cp','--sparse=always',a.base/(name+'.img'),image)
    command('e2fsck','-fy',image,accepted=(0,1))
    d=work/name;d.mkdir(exist_ok=True);command('mount','-o','loop,rw',image,d);mounts[name]=d
   for item in changes:apply(mounts[item['image']],item)
   for member in overlay:
    image,rel=member.name.split('/',1)
    assert image in mounts and '..' not in Path(rel).parts
    member.name=rel;overlay.extract(member,path=mounts[image],filter='fully_trusted')
   root=mounts['system'];debug=root/'debug';debug.mkdir(exist_ok=True)
   for name in ['first-boot.sh','run-sshd.sh']:
    shutil.copyfile(kit/'scripts'/name,debug/name);os.chmod(debug/name,0o755)
   hw=debug/'load-hardware.sh';text=hw.read_text()
   text=text.replace('echo "Hardware load $(date)"','echo "Hardware load $(date)"\n/debug/busybox sh /debug/first-boot.sh')
   hw.write_text(text)
   cfg=mounts['vendor']/'etc/init.hmos.emulator.cfg';doc=json.loads(cfg.read_text())
   service=next(s for s in doc['services'] if s['name']=='nabu-sshd');service['path']=['/debug/busybox','sh','/debug/run-sshd.sh']
   cfg.write_text(json.dumps(doc,indent=2)+'\n')
   for item in changes:
    if item['action']=='symlink':assert (mounts[item['image']]/item['path']).is_symlink()
   command('sync')
  finally:
   for d in reversed(list(mounts.values())):command('umount',d)
 data=a.output/'userdata.img'
 if data.exists():raise SystemExit('userdata.img already exists')
 with data.open('wb') as f:f.truncate(4*1024**3)
 command('mkfs.ext4','-F','-b','4096','-m','0','-L','userdata',data)
 d=work/'data';d.mkdir(exist_ok=True);command('mount','-o','loop,rw',data,d)
 try:
  vendor=d/'vendor';vendor.mkdir(exist_ok=True)
  temporary=d/'local/tmp';temporary.mkdir(parents=True,exist_ok=True);os.chown(temporary,2000,2000);os.chmod(temporary,0o771)
  for name in ['firmware','sensors']:
   shutil.copytree(a.hardware/name,vendor/name,symlinks=True,dirs_exist_ok=True)
  command('sync')
 finally:command('umount',d)
 for name in ['system','vendor','sys_prod','userdata']:command('e2fsck','-fy',a.output/(name+'.img'),accepted=(0,1))
 print('LOCAL_IMAGES_READY',a.output)
if __name__=='__main__':main()
