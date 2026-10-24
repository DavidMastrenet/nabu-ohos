#!/debug/busybox sh
set -e
for position in BR TR BL TL; do
 /debug/tinymix -D 0 set "$position Digital PCM Volume" 745
 /debug/tinymix -D 0 set "$position PCM Soft Ramp" 4ms
 /debug/tinymix -D 0 set "$position Analog PCM Volume" 8
done
/debug/tinymix -D 0 set 'QUAT_TDM_RX_0 Audio Mixer MultiMedia1' 1
