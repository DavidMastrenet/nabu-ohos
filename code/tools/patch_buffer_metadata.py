"""Implement cross-process buffer metadata for the nabu display allocator."""
from pathlib import Path
import difflib,re
w=Path('/root/nabu-package')
root=Path('/root/nabu-build/graphics/nabu-display')
changes=[]
def write(name,s):
    p=root/name
    original=(w/'sources/device_soc_oniro/x86_general/hardware/display'/name).read_text()
    if p.read_text()!=s:p.write_text(s)
    changes.extend(difflib.unified_diff(original.splitlines(True),s.splitlines(True),fromfile='a/'+name,tofile='b/'+name))

name='src/display_gralloc/allocator.cpp'
s=(root/name).read_text()
if '#include "nabu_buffer_metadata.h"' not in s:s=s.replace('#include "allocator.h"','#include "allocator.h"\n#include "nabu_buffer_metadata.h"')
if 'NabuBufferMetadata::Attach' not in s:
    s=s.replace('    *handle = priBuffer;','''    ret = NabuBufferMetadata::Attach(priBuffer);
    if (ret != DISPLAY_SUCCESS) {
        FreeMem(priBuffer);
        return ret;
    }
    *handle = priBuffer;''',1)
write(name,s)
name='src/display_gralloc/display_buffer_vdi_impl.cpp'
s=(root/name).read_text()
if '#include "nabu_buffer_metadata.h"' not in s:s=s.replace('#include "display_buffer_vdi_impl.h"','#include "display_buffer_vdi_impl.h"\n#include "nabu_buffer_metadata.h"')
for method,expression in [('RegisterBuffer','Register(handle)'),('SetMetadata','Set(handle, key, value)'),
                          ('GetMetadata','Get(handle, key, value)'),('ListMetadataKeys','List(handle, keys)'),
                          ('EraseMetadataKey','Erase(handle, key)')]:
    pattern=r'(int32_t DisplayBufferVdiImpl::'+method+r'\([^)]*\)\s*\{)[^}]*\}'
    s,n=re.subn(pattern,lambda m:m[1]+'\n    return NabuBufferMetadata::'+expression+';\n}',s,count=1)
    assert n==1,method
write(name,s)
(w/'port/patches/nabu-buffer-metadata.patch').write_text(''.join(changes))
print('BUFFER_METADATA_PATCHED')
