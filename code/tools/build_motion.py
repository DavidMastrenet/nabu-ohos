from pathlib import Path
import subprocess,shutil
w=Path('/root/nabu-package');b=Path('/root/nabu-build/graphics')
tc=Path('/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908')
gen=Path('/root/nabu-build/motion-generated')
shutil.copytree(w/'port/generated/motion',gen,dirs_exist_ok=True)
for p in list(gen.rglob('*.h')):
    alias=gen/str(p.relative_to(gen)).replace('/','\\')
    if not alias.exists():alias.symlink_to(p)
includes=[w/'port',gen,gen/'motion',
 b/'commonlibrary_c_utils/base/include',b/'communication_ipc/interfaces/innerkits/ipc_core/include',
 b/'drivers_hdf_core/interfaces/inner_api/utils',b/'drivers_hdf_core/interfaces/inner_api/host/uhdf',
 b/'drivers_hdf_core/interfaces/inner_api/osal/uhdf',b/'drivers_hdf_core/interfaces/inner_api/hdi',
 b/'drivers_hdf_core/interfaces/inner_api/hdi/base',b/'drivers_hdf_core/interfaces/inner_api/ipc',
 b/'drivers_hdf_core/interfaces/inner_api/core',
 b/'hiviewdfx_hilog/interfaces/native/innerkits/include',b/'third_party_bounds_checking_function/include']
lib=Path('/root/nabu-rootfs/system-lib/lib64')
dirs=[lib,lib/'platformsdk',lib/'chipset-sdk',lib/'chipset-pub-sdk',lib/'chipset-sdk-sp',lib/'ndk']
sources=[w/'port/motion-service.cpp',gen/'motion/v1_0/motion_interface_stub.cpp',
 gen/'motion/v1_0/motion_callback_proxy.cpp',gen/'motion/v1_0/motion_types.cpp',gen/'motion/v1_1/motion_interface_stub.cpp']
subprocess.run([str(tc/'bin/clang++'),'--target=aarch64-linux-ohosmusl','--sysroot=/root/nabu-build/ohos-sysroot',
 '-O2','-fno-emulated-tls','-fno-rtti','-std=c++17','-nostdinc++','-isystem',str(tc/'include/libcxx-ohos/include/c++/v1'),
 *[f'-I{p}' for p in includes],*map(str,sources),*[f'-L{p}' for p in dirs],*[f'-Wl,-rpath-link,{p}' for p in dirs],
 '-l:libutils.z.so','-l:libhilog.so','-l:libhdf_utils.z.so','-l:libipc_single.z.so','-l:libsec_shared.z.so',
 '-l:libhdi.z.so','-l:libhdi_base.z.so','-l:libhdf_ipc_adapter.z.so','-lohsensor','-lm','-Wl,--allow-shlib-undefined',
 '-o',str(w/'port/out/nabu-motion')],check=True)
print('NABU_MOTION_BUILT')
