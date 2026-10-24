"""Normalize firmware privately extracted from the user's own Android firmware."""
from pathlib import Path
import argparse,shutil,struct
p=argparse.ArgumentParser();p.add_argument('--input',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
files={}
for f in a.input.rglob('*'):
 if f.is_file():files.setdefault(f.name,[]).append(f)
a.output.mkdir(parents=True,exist_ok=True)
def copy(names,destination,required=True):
 for name in names:
  if name in files:
   dest=a.output/destination;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(files[name][0],dest);return True
 if required:raise FileNotFoundError('Required firmware: '+', '.join(names))
 return False
def assemble_mdt(src,dest):
 data=bytearray(src.read_bytes());is64=data[4]==2
 if is64:phoff=struct.unpack_from('<Q',data,32)[0];ents,num=struct.unpack_from('<HH',data,54)
 else:phoff=struct.unpack_from('<I',data,28)[0];ents,num=struct.unpack_from('<HH',data,42)
 for i in range(num):
  off=phoff+i*ents
  if is64:kind,flags,fileoff,va,pa,filesz,memsz,align=struct.unpack_from('<IIQQQQQQ',data,off)
  else:kind,fileoff,va,pa,filesz,memsz,flags,align=struct.unpack_from('<IIIIIIII',data,off)
  if filesz==0:continue
  piece=src.with_suffix('.b%02d'%i)
  if not piece.exists():
   if fileoff+filesz<=len(data):continue
   raise FileNotFoundError(piece)
  part=piece.read_bytes();assert len(part)==filesz,str(piece)+' has unexpected segment size'
  if len(data)<fileoff+filesz:data.extend(bytes(fileoff+filesz-len(data)))
  data[fileoff:fileoff+filesz]=part
 dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data)
prefix=Path('firmware/qcom/sm8150/xiaomi/nabu')
for target,aliases in {'adsp':['adsp'],'modem':['modem'],'wlanmdsp':['wlanmdsp'],'slpi_nb':['slpi_nb','slpi'],'cdsp':['cdsp'],'venus':['venus'],'a640_zap':['a640_zap']}.items():
 dest=a.output/prefix/(target+'.mbn')
 if copy([n+'.mbn' for n in aliases],dest.relative_to(a.output),False):continue
 source=next((files[n+'.mdt'][0] for n in aliases if n+'.mdt' in files),None)
 if source is None:raise SystemExit('Missing firmware ELF/MDT: '+target)
 assemble_mdt(source,dest)
copy(['modemuw.jsn'],prefix/'modemuw.jsn',False)
copy(['a630_sqe.fw'],'firmware/qcom/a630_sqe.fw')
copy(['a640_gmu.bin'],'firmware/qcom/a640_gmu.bin')
touch=['novatek_nt36523_fw.bin','novatek_ts_fw.bin','nvt_ts_fw.bin']
if not any(n in files for n in touch):
 candidates=[f for name,entries in files.items() for f in entries if f.stat().st_size==196608 and 'novatek' in str(f).lower()]
 if len(candidates)==1:files[touch[0]]=candidates
copy(touch,'firmware/novatek/novatek_nt36523_fw.bin')
if not copy(['board-2.bin'],'firmware/ath10k/WCN3990/hw1.0/board-2.bin',False):
 source=next((files[n][0] for n in ['bdwlan.bin','bdwlan.b67','bdwlan.bff'] if n in files),None)
 if source is None:raise SystemExit('Missing WCN3990 board-2.bin or bdwlan.bin')
 def ie(key,value):return struct.pack('<II',key,len(value))+value+bytes((-len(value))%4)
 payload=source.read_bytes();board=b'QCA-ATH10K-BOARD\0';board+=bytes((-len(board))%4)
 for name in [b'bus=snoc,qmi-board-id=67',b'bus=snoc,qmi-board-id=ff']:board+=ie(0,ie(0,name)+ie(1,payload))
 dest=a.output/'firmware/ath10k/WCN3990/hw1.0/board-2.bin';dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(board)
def ie(key,value):return struct.pack('<II',key,len(value))+value+bytes((-len(value))%4)
desc=b'QCA-ATH10K\0';desc+=bytes((-len(desc))%4)
desc+=ie(1,struct.pack('<I',0))+ie(2,bytes([0x40,0,0x0c]))+ie(5,struct.pack('<I',4))+ie(6,struct.pack('<I',3))
(a.output/'firmware/ath10k/WCN3990/hw1.0/firmware-5.bin').write_bytes(desc)
for n in ['regulatory.db','regulatory.db.p7s']:copy([n],Path('firmware')/n,False)
for n in ['crnv32u.bin','crnv32.bin','crbtfw32.tlv']:copy([n],Path('firmware/qca')/n,False)
config=a.output/'sensors/sensors/config';config.mkdir(parents=True,exist_ok=True)
for name,entries in files.items():
 if name.endswith('.json'):
  chosen=next((f for f in entries if 'sensors' in str(f).lower() and 'config' in str(f).lower()),None)
  if chosen:shutil.copyfile(chosen,config/name)
if not any(config.iterdir()):raise SystemExit('No sensors/config/*.json found in the local Android capture')
print('HARDWARE_INPUT_READY',a.output)
