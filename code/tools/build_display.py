from pathlib import Path
import subprocess,shutil,re,json
b=Path('/root/nabu-build/graphics')
w=Path('/root/nabu-package')
tc=Path('/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908')
src=b/'nabu-display'
if not src.exists(): shutil.copytree(w/'sources/device_soc_oniro/x86_general/hardware/display',src)
p=src/'src/display_device/drm/drm_connector.cpp'
s=p.read_text();t=s.replace('DrmMode { *mode, i }','DrmMode { *mode, static_cast<uint32_t>(i) }')
if t!=s:p.write_text(t)
p=src/'src/display_device/drm/drm_connector.cpp'
s=p.read_text()
start=s.find('    memcpy_s(const_cast<char*>(cap.name.c_str())')
end=s.find('    cap.supportLayers',start)
if start>=0 and end>start:
 s=s[:start]+'    cap.name = mName;\n'+s[end:]
 p.write_text(s)
p=src/'src/display_device/drm/drm_display.cpp'
s=p.read_text()
if 'ReadNabuBacklight' not in s:
 helper='''
static unsigned ReadNabuBacklight(const char *node)
{
    char path[160];
    snprintf(path, sizeof(path), "/sys/class/backlight/ktz8866-backlight/%s", node);
    FILE *fp = fopen(path, "r");
    unsigned value = 0;
    if (fp) { fscanf(fp, "%u", &value); fclose(fp); }
    return value;
}
'''
 s=s.replace('namespace OHOS {','#include <cstdio>\n'+helper+'\nnamespace OHOS {',1)
 s=s.replace('    return mConnector->GetBrightness(*value);','''    int result = mConnector->GetBrightness(*value);
    if (result == DISPLAY_SUCCESS) return result;
    unsigned maximum = ReadNabuBacklight("max_brightness");
    if (!maximum) return result;
    *value = (ReadNabuBacklight("brightness") * 255U + maximum / 2U) / maximum;
    return DISPLAY_SUCCESS;''',1)
 s=s.replace('    return mConnector->SetBrightness(value);','''    int result = mConnector->SetBrightness(value);
    if (result == DISPLAY_SUCCESS) return result;
    unsigned maximum = ReadNabuBacklight("max_brightness");
    if (!maximum || value > 255U) return DISPLAY_PARAM_ERR;
    FILE *fp = fopen("/sys/class/backlight/ktz8866-backlight/brightness", "w");
    if (!fp) return DISPLAY_FAILURE;
    int written = fprintf(fp, "%u", (value * maximum + 127U) / 255U);
    int closed = fclose(fp);
    return written > 0 && closed == 0 ? DISPLAY_SUCCESS : DISPLAY_FAILURE;''',1)
 p.write_text(s)
p=src/'src/display_gralloc/display_buffer_vdi_impl.cpp'
s=p.read_text()
t=s.replace('    AllocatorManager::GetInstance().DeInit();','    // Shared allocator ownership lasts for the process; other VDI clients may remain alive.')
t=t.replace('    return AllocatorManager::GetInstance().GetAllocator(info.usage)->AllocMem(info, &handle);','    auto *allocator = AllocatorManager::GetInstance().GetAllocator(info.usage);\n    return allocator ? allocator->AllocMem(info, &handle) : DISPLAY_FAILURE;')
if t!=s:p.write_text(t)
import runpy
runpy.run_path(str(w/'tools/patch_display_runtime.py'))
runpy.run_path(str(w/'tools/patch_buffer_metadata.py'))
newest_header=max(p.stat().st_mtime for p in src.rglob('*.h'))
newest_header=max(newest_header,(w/'port/graphics-include/nabu_buffer_metadata.h').stat().st_mtime)
gen=b/'display-generated'
if not gen.exists(): shutil.copytree(w/'port/generated',gen)
# Preserve generator output: supply file aliases for Windows IDL include paths.
for domain in ['buffer','composer']:
 root=gen/domain
 for p in list(root.rglob('*.h')):
  alias=root/str(p.relative_to(root)).replace('/','\\')
  if not alias.exists():alias.symlink_to(p)
includes=[src/'include', *[p for p in (src/'src').rglob('*') if p.is_dir()],
 w/'port/graphics-include',
 w/'sources/drivers_peripheral/base',
 w/'sources/drivers_peripheral/display/buffer/hdi_service/include',
 w/'sources/drivers_peripheral/display/composer/hdi_service/include',
 w/'sources/drivers_peripheral/display/utils/include',
 w/'sources/drivers_peripheral/display/interfaces/include',
 w/'sources/drivers_interface/display/composer',
 w/'sources/drivers_interface/display/composer/hdifd_parcelable',
 w/'sources/drivers_interface/display/composer/common/include',
 gen/'buffer',gen/'buffer/display/buffer',gen/'composer',gen/'composer/display/composer',
 b/'graphic_graphic_surface/interfaces/inner_api/buffer_handle',
 b/'commonlibrary_c_utils/base/include',
 b/'communication_ipc/interfaces/innerkits/ipc_core/include',
 b/'drivers_hdf_core/interfaces/inner_api/utils',
 b/'drivers_hdf_core/interfaces/inner_api/osal/uhdf',
 b/'drivers_hdf_core/interfaces/inner_api/hdi/base',
 b/'drivers_hdf_core/interfaces/inner_api/hdi',
 b/'drivers_hdf_core/interfaces/inner_api/ipc',
 b/'hiviewdfx_hilog/interfaces/native/innerkits/include',
 b/'third_party_bounds_checking_function/include',
 b/'install/include',b/'install/include/libdrm']
libs=Path('/root/nabu-rootfs/system-lib/lib64')
libdirs=[libs,libs/'platformsdk',libs/'chipset-sdk',libs/'chipset-pub-sdk',libs/'chipset-sdk-sp',b/'install/lib']
out=b/'display-out';out.mkdir(exist_ok=True)
common=[str(tc/'bin/clang++'),'--target=aarch64-linux-ohosmusl','--sysroot=/root/nabu-build/ohos-sysroot','-std=c++17','-nostdinc++','-isystem',str(tc/'include/libcxx-ohos/include/c++/v1'),'-fPIC','-fno-emulated-tls','-O2','-DGRALLOC_GBM_SUPPORT','-Wno-macro-redefined',*[f'-I{p}' for p in includes]]
targets=[
 ('libdisplay_gfx.z.so',['src/display_gfx/display_gfx.c'],[]),
 ('libdisplay_buffer_vendor.z.so',['src/display_gralloc/'+f+'.cpp' for f in ['allocator','allocator_manager','drm_allocator']],[]),
 ('libdisplay_buffer_vdi_impl.z.so',['src/display_gralloc/display_buffer_vdi_impl.cpp'],['libdisplay_buffer_vendor.z.so']),
 ('libdisplay_composer_vendor.z.so',[str(p.relative_to(src)) for p in (src/'src/display_device').rglob('*.cpp') if p.name in ['hdi_composer.cpp','hdi_gfx_composition.cpp','hdi_device_interface.cpp','hdi_display.cpp','hdi_layer.cpp','hdi_session.cpp','drm_connector.cpp','drm_crtc.cpp','drm_device.cpp','drm_display.cpp','drm_encoder.cpp','drm_plane.cpp','drm_vsync_worker.cpp','hdi_drm_composition.cpp','hdi_drm_layer.cpp']],['libdisplay_buffer_vdi_impl.z.so']),
 ('libdisplay_composer_vdi_impl.z.so',['src/display_device/display_composer_vdi_impl.cpp'],['libdisplay_composer_vendor.z.so'])]
for name,files,extra in targets:
 objects=[]
 for f in files:
  obj=out/(f.replace('/','_')+'.o');objects.append(obj)
  if obj.exists() and obj.stat().st_mtime>max((src/f).stat().st_mtime,newest_header):continue
  result=subprocess.run(common+['-c',str(src/f),'-o',str(obj)],capture_output=True,text=True)
  if result.returncode:
   print(result.stdout+result.stderr);raise SystemExit(result.returncode)
 cmd=common+['-shared','-Wl,-z,defs','-Wl,-soname,'+name,*map(str,objects),'-o',str(out/name),'-L'+str(out),*[f'-L{p}' for p in libdirs],*[f'-Wl,-rpath-link,{p}' for p in libdirs],*[f'-l:{l}' for l in extra],'-ldrm','-l:libhilog.so','-l:libsec_shared.z.so','-l:libutils.z.so','-l:libhdf_utils.z.so','-l:libipc_single.z.so','-l:libhitrace_meter.so']
 result=subprocess.run(cmd,capture_output=True,text=True)
 if result.returncode:
  print(result.stdout+result.stderr);raise SystemExit(result.returncode)
 print('BUILT',name,flush=True)
subprocess.run(common+[str(w/'port/buffer-metadata-test.cpp'),'-o',str(w/'port/out/buffer-metadata-test')],check=True)
subprocess.run(common+[str(w/'port/buffer-driver-probe.cpp'),'-o',str(w/'port/out/buffer-driver-probe'),'-L'+str(out),*[f'-L{p}' for p in libdirs],*[f'-Wl,-rpath-link,{p}' for p in libdirs],'-l:libdisplay_buffer_vdi_impl.z.so','-Wl,--allow-shlib-undefined'],check=True)
cmd=common+[str(w/'port/display-probe.cpp'),'-o',str(w/'port/out/display-probe'),'-L'+str(out),*[f'-L{p}' for p in libdirs],*[f'-Wl,-rpath-link,{p}' for p in libdirs],'-l:libdisplay_composer_vdi_impl.z.so','-l:libdisplay_buffer_vdi_impl.z.so','-Wl,--allow-shlib-undefined']
subprocess.run(cmd,check=True)
cmd=common+[str(w/'port/display-control.cpp'),'-o',str(w/'port/out/display-control'),*[f'-L{p}' for p in libdirs],*[f'-Wl,-rpath-link,{p}' for p in libdirs],'-l:libdisplay_composer_hdi_impl_1.5.z.so','-Wl,--allow-shlib-undefined']
subprocess.run(cmd,check=True)
