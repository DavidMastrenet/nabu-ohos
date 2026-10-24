"""Keep release fences owned by the VDI for the non-community HDI service."""
from pathlib import Path
import difflib

w = Path('/root/nabu-package')
root = Path('/root/nabu-build/graphics/nabu-display')
patches = []

def update(name, transform):
    p = root / name
    original = (w / 'sources/device_soc_oniro/x86_general/hardware/display' / name).read_text()
    s = p.read_text()
    t = transform(s)
    if t != s:
        p.write_text(t)
    patches.extend(difflib.unified_diff(original.splitlines(True), t.splitlines(True),
                                      fromfile='a/' + name, tofile='b/' + name))

def layer_header(s):
    comment = ('    // The HDI service duplicates this borrowed descriptor for IPC and calls\n'
               '    // HdifdParcelable::Move(); the VDI must retain and close its original.\n')
    s = s.replace(comment, '')
    start = s.index('    int GetReleaseFenceFd()')
    end = s.index('    bool IsVisible()', start)
    s = s[:start] + '''    // The HDI service duplicates this borrowed descriptor for IPC and calls
    // HdifdParcelable::Move(); the VDI must retain and close its original.
    int GetReleaseFenceFd()
    {
        return mReleaseFence.GetFd();
    }

''' + s[end:]
    s = s.replace('releaseFences_.push(fd);', 'mReleaseFence = fd;')
    return s.replace('std::queue<int> releaseFences_;', 'HdiFd mReleaseFence;')

def layer_source(s):
    start = s.find('    while (!releaseFences_.empty())')
    if start >= 0:
        end = s.index('    }', start) + len('    }\n')
        s = s[:start] + s[end:]
    s = s.replace('    if (fence > -1) {\n        mAcquireFence = dup(fence);',
                  '    mAcquireFence = fence;\n    if (fence > -1) {')
    # The non-community service transfers (Move), rather than lends, input FDs.
    return s.replace('mAcquireFence = fence >= 0 ? dup(fence) : -1;',
                     'mAcquireFence = fence;')

update('src/display_device/core/hdi_layer.h', layer_header)
update('src/display_device/core/hdi_layer.cpp', layer_source)
update('src/display_device/drm/drm_display.cpp', lambda s: s.replace(
    '    mConnector->SetDpmsState(drmPowerState);\n    return DISPLAY_SUCCESS;',
    '    return mConnector->SetDpmsState(drmPowerState);', 1))
(w / 'port/patches/nabu-display-runtime.patch').write_text(''.join(patches))
print('DISPLAY_RUNTIME_PATCHED')
