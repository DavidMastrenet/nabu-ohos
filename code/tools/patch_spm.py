from pathlib import Path
import difflib
ws=Path('/root/nabu-package')
k=Path('/root/nabu-build/linux-6.17')
changes={}
def edit(name,func):
 p=k/name;s=p.read_text();t=func(s)
 if t!=s: changes[name]=(s,t);p.write_text(t)
edit('include/linux/sched.h', lambda s:s if 'ohos_userid' in s else s.replace('\tu64 ftoken;', '\tu64 ftoken;\n\tu32 ohos_userid;\n\tu8 ohos_enterprise;',1))
edit('drivers/accesstokenid/access_tokenid.c', lambda s:s if '#include "ohos_spm.h"' in s else s.replace('static long access_tokenid_ioctl(', '#include "ohos_spm.h"\n\nstatic long access_tokenid_ioctl(',1).replace('\t\t\tret = -EOPNOTSUPP;', '\t\t\tret = ohos_spm_ioctl(cmd, uarg);',1))
guard='\tif (_IOC_TYPE(cmd) != \'A\') return -ENOTTY;\n'
errorlog='\t\t\tpr_err("%s: access tokenid cmd error, cmd:%u\\n", __func__, cmd);\n'
def quiet_tty_probes(s):
 if guard not in s:
  s=s.replace('\tif (uarg == NULL) {',guard+'\n\tif (uarg == NULL) {',1)
 return s.replace(errorlog,'',1)
edit('drivers/accesstokenid/access_tokenid.c',quiet_tty_probes)
p=k/'drivers/accesstokenid/ohos_spm.h'
t=(ws/'port/ohos_spm.h').read_text()
s=p.read_text() if p.exists() else ''
if t!=s: changes['drivers/accesstokenid/ohos_spm.h']=(s,t);p.write_text(t)
# Always emit the complete patch relative to the pre-SPM AccessToken port.
# Incremental reruns must not replace it with only their latest edit.
complete={}
name='include/linux/sched.h'
current=(k/name).read_text()
base=current.replace('\n\tu32 ohos_userid;\n\tu8 ohos_enterprise;', '', 1)
complete[name]=(base,current)
name='drivers/accesstokenid/access_tokenid.c'
current=(k/name).read_text()
base=current.replace('#include "ohos_spm.h"\n\n', '', 1).replace('ret = ohos_spm_ioctl(cmd, uarg);', 'ret = -EOPNOTSUPP;', 1)
base=base.replace(guard+'\n','',1)
base=base.replace('\t\tdefault:\n\t\t\tret = -EOPNOTSUPP;', '\t\tdefault:\n'+errorlog+'\t\t\tret = -EOPNOTSUPP;',1)
complete[name]=(base,current)
complete['drivers/accesstokenid/ohos_spm.h']=('',t)
patch=''.join(''.join(difflib.unified_diff(a.splitlines(True),b.splitlines(True),fromfile='a/'+n if a else '/dev/null',tofile='b/'+n)) for n,(a,b) in complete.items())
(ws/'port/patches/ohos-spm-api26.patch').write_text(patch)
print('SPM patch files:',len(changes))
