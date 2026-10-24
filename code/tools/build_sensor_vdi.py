from pathlib import Path
import subprocess,shutil
w=Path('/root/nabu-package');b=Path('/root/nabu-build/graphics')
tc=Path('/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908')
gen=Path('/root/nabu-build/sensor-generated')
shutil.copytree(w/'port/generated/sensor',gen,dirs_exist_ok=True)
for p in list(gen.rglob('*.h')):
    alias=gen/str(p.relative_to(gen)).replace('/','\\')
    if not alias.exists():alias.symlink_to(p)
includes=[w/'port',w/'sources/drivers_peripheral/sensor/interfaces/include',
 w/'sources/drivers_peripheral/sensor/interfaces/v1_0',gen,gen/'sensor',
 b/'commonlibrary_c_utils/base/include',b/'communication_ipc/interfaces/innerkits/ipc_core/include',
 b/'drivers_hdf_core/interfaces/inner_api/utils',b/'drivers_hdf_core/interfaces/inner_api/host/uhdf',
 b/'drivers_hdf_core/interfaces/inner_api/osal/uhdf',b/'drivers_hdf_core/interfaces/inner_api/hdi',
 b/'drivers_hdf_core/interfaces/inner_api/hdi/base',b/'drivers_hdf_core/interfaces/inner_api/ipc',
 b/'hiviewdfx_hilog/interfaces/native/innerkits/include',b/'third_party_bounds_checking_function/include']
common=['--target=aarch64-linux-ohosmusl','--sysroot=/root/nabu-build/ohos-sysroot',
 '-fPIC','-fno-emulated-tls','-O2',*[f'-I{p}' for p in includes]]
objects=[]
for name in ['sensor-hal','ssc-client']:
    obj=Path('/root/nabu-build')/(name+'.o');objects.append(obj)
    subprocess.run([str(tc/'bin/clang'),*common,'-std=c11','-c',str(w/f'port/{name}.c'),'-o',str(obj)],check=True)
lib=Path('/root/nabu-rootfs/system-lib/lib64')
libdirs=[lib,lib/'platformsdk',lib/'chipset-sdk',lib/'chipset-pub-sdk',lib/'chipset-sdk-sp']
subprocess.run([str(tc/'bin/clang++'),*common,'-std=c++17','-nostdinc++','-isystem',
 str(tc/'include/libcxx-ohos/include/c++/v1'),'-shared','-Wl,-z,defs',
 '-Wl,-soname,libhdi_sensor_impl.z.so',str(w/'port/sensor-vdi.cpp'),*map(str,objects),
 *[f'-L{p}' for p in libdirs],*[f'-Wl,-rpath-link,{p}' for p in libdirs],
 '-l:libutils.z.so','-l:libhilog.so','-l:libhdf_utils.z.so','-l:libipc_single.z.so','-l:libsec_shared.z.so',
 '-o',str(w/'port/out/libhdi_sensor_impl.z.so')],check=True)
print('NABU_SENSOR_VDI_BUILT')
