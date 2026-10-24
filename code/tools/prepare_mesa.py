from pathlib import Path
import difflib
b=Path('/root/nabu-build/graphics')
ws=Path('/root/nabu-package')
pc=b/'install/lib/pkgconfig'
libs=Path('/root/nabu-rootfs/system-lib/lib64')
surface=b/'graphic_graphic_surface/interfaces/inner_api'
incs=[surface/'surface',surface/'buffer_handle',ws/'sources/drivers_peripheral/display/interfaces/include',ws/'sources/drivers_interface/display/composer/common/include']
def pkg(name,version,link,includes):
 (pc/(name+'.pc')).write_text(f'Name: {name}\nDescription: nabu OHOS target library\nVersion: {version}\nLibs: '+link+'\nCflags: '+' '.join('-I'+str(i) for i in includes)+'\n')
pkg('libsurface','1.0',f'-L{libs}/chipset-sdk-sp -l:libnative_window.so -l:libsurface.z.so',incs)
pkg('libhilog','1.0',f'-L{libs}/platformsdk -lhilog',[b/'hiviewdfx_hilog/interfaces/native/innerkits/include'])
pkg('expat','2.7.1',f'-L{libs} -l:libexpat.z.so',[ws/'port/graphics-include'])
pkg('zlib','1.2.13',f'-L{libs}/platformsdk -lz',[])
p=b/'mesa-ohos5/meson.build'
s=p.read_text()
t=s.replace('from distutils.version import StrictVersion','from packaging.version import Version as StrictVersion')
if s!=t:p.write_text(t)
# Correct a reference-counting typo in the upstream OHOS wrapper.
p=b/'mesa-ohos5/src/egl/drivers/dri2/platform_ohos.h'
s=p.read_text()
t=s.replace('ANativeWindow_release(struct ANativeWindow *window)\n{\n   NativeObjectReference(window);','ANativeWindow_release(struct ANativeWindow *window)\n{\n   NativeObjectUnreference(window);')
if s!=t:p.write_text(t)
# Serialize the last BO reference with handle-table imports. An import may
# revive a zero-reference BO while another thread is waiting to retire it.
p=b/'mesa-ohos5/src/freedreno/drm/freedreno_bo.c'
s=p.read_text()
t=s.replace('''   if (!p_atomic_dec_zero(&bo->refcnt))
      return;

   simple_mtx_lock(&table_lock);
   bo_del_or_recycle(bo);
   simple_mtx_unlock(&table_lock);''','''   simple_mtx_lock(&table_lock);
   fd_bo_del_locked(bo);
   simple_mtx_unlock(&table_lock);''',1)
# A concurrent context can grow or compact the fence array. Hold references
# to a snapshot, then flush outside table_lock (submission also takes it).
t=t.replace('''   for (int i = 0; i < bo->nr_fences; i++) {
      struct fd_bo_fence *f = &bo->fences[i];
      fd_pipe_flush(f->pipe, f->fence);
   }
}''','''   simple_mtx_lock(&table_lock);
   unsigned count = bo->nr_fences;
   struct fd_bo_fence *fences = calloc(count, sizeof(*fences));
   if (count && !fences) {
      simple_mtx_unlock(&table_lock);
      abort();
   }
   for (unsigned i = 0; i < count; i++) {
      fences[i] = bo->fences[i];
      fd_pipe_ref_locked(fences[i].pipe);
   }
   simple_mtx_unlock(&table_lock);
   for (unsigned i = 0; i < count; i++) {
      fd_pipe_flush(fences[i].pipe, fences[i].fence);
      fd_pipe_del(fences[i].pipe);
   }
   free(fences);
}''',1)
if s!=t:
 (ws/'port/patches/mesa-freedreno-bo-lifetime.patch').write_text(''.join(difflib.unified_diff(s.splitlines(True),t.splitlines(True),fromfile='a/src/freedreno/drm/freedreno_bo.c',tofile='b/src/freedreno/drm/freedreno_bo.c')))
 p.write_text(t)
p=b/'mesa-ohos5/src/freedreno/drm/freedreno_bo_cache.c'
s=p.read_text()
t=s.replace('''         list_delinit(&bo->list);
         break;''','''         list_delinit(&bo->list);
         p_atomic_set(&bo->refcnt, 1);
         break;''',1)
t=t.replace('''         p_atomic_set(&bo->refcnt, 1);
         bo->reloc_flags''','''         bo->reloc_flags''',1)
if s!=t:
 with (ws/'port/patches/mesa-freedreno-bo-lifetime.patch').open('a') as f:
  f.write(''.join(difflib.unified_diff(s.splitlines(True),t.splitlines(True),fromfile='a/src/freedreno/drm/freedreno_bo_cache.c',tofile='b/src/freedreno/drm/freedreno_bo_cache.c')))
 p.write_text(t)
print('OHOS Mesa dependency metadata ready')
