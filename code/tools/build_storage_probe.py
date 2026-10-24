from pathlib import Path
import subprocess
w=Path('/root/nabu-package');b=Path('/root/nabu-build/graphics')
tc=Path('/root/nabu-build/ohos-toolchain/clang_linux-x86_64-02fbe8-20260908')
inc=[b/'commonlibrary_c_utils/base/include',b/'communication_ipc/interfaces/innerkits/ipc_core/include']
inc += [b/'drivers_hdf_core/interfaces/inner_api'/p for p in ['utils','core','ipc','hdi','hdi/base','host/uhdf','osal/uhdf']]
lib=Path('/root/nabu-rootfs/system-lib/lib64');dirs=[lib,lib/'platformsdk',lib/'chipset-pub-sdk',lib/'chipset-sdk-sp',lib/'chipset-sdk']
subprocess.run([str(tc/'bin/clang++'),'--target=aarch64-linux-ohosmusl','--sysroot=/root/nabu-build/ohos-sysroot',
 '-O2','-fno-rtti','-fno-emulated-tls','-std=c++17','-nostdinc++','-isystem',str(tc/'include/libcxx-ohos/include/c++/v1'),
 *[f'-I{p}' for p in inc],str(w/'port/storage-probe.cpp'),*[f'-L{p}' for p in dirs],
 *[f'-Wl,-rpath-link,{p}' for p in dirs],'-l:libutils.z.so','-l:libipc_single.z.so','-l:libhdf_ipc_adapter.z.so','-l:libhdf_utils.z.so',
 '-Wl,--allow-shlib-undefined','-o',str(w/'port/out/storage-probe')],check=True)
