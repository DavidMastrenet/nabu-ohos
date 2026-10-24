from pathlib import Path
base=Path('/root/nabu-build')
tc=base/'ohos-toolchain/clang_linux-x86_64-02fbe8-20260908'
sysroot=base/'ohos-sysroot'
prefix=base/'graphics/install'
prefix.mkdir(exist_ok=True)
args=['--target=aarch64-linux-ohosmusl',f'--sysroot={sysroot}','-fPIC','-fno-emulated-tls']
cppargs=args+['-nostdinc++','-isystem',str(tc/'include/libcxx-ohos/include/c++/v1')]
linkargs=args+['-L/root/nabu-rootfs/system-lib/lib64','-L/root/nabu-rootfs/system-lib/lib64/platformsdk','-Wl,-rpath-link,/root/nabu-rootfs/system-lib/lib64','-Wl,-rpath-link,/root/nabu-rootfs/system-lib/lib64/platformsdk']
cross=f'''[binaries]
c = '{tc}/bin/clang'
cpp = '{tc}/bin/clang++'
ar = '{tc}/bin/llvm-ar'
strip = '{tc}/bin/llvm-strip'
pkg-config = '/usr/bin/pkg-config'
[properties]
needs_exe_wrapper = true
pkg_config_libdir = ['{prefix}/lib/pkgconfig','{prefix}/share/pkgconfig']
[built-in options]
c_args = {args!r}
cpp_args = {cppargs!r}
c_link_args = {linkargs!r}
cpp_link_args = {linkargs!r}
[host_machine]
system = 'linux'
cpu_family = 'aarch64'
cpu = 'armv8'
endian = 'little'
'''
(base/'graphics/ohos.cross').write_text(cross)
print('Created',base/'graphics/ohos.cross')
