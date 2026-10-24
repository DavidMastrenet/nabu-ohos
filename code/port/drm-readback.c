#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <drm.h>
#include <drm_mode.h>
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    int fd = open("/dev/dri/card0", O_RDWR);
    struct drm_mode_fb_cmd2 fb = { .fb_id = strtoul(argv[1], NULL, 0) };
    if (fd < 0 || ioctl(fd, DRM_IOCTL_MODE_GETFB2, &fb)) { perror("GETFB2"); return 1; }
    printf("fb=%u %ux%u fmt=%08x handle=%u pitch=%u modifier=%llx\n", fb.fb_id,
        fb.width, fb.height, fb.pixel_format, fb.handles[0], fb.pitches[0],
        (unsigned long long)fb.modifier[0]);
    struct drm_mode_map_dumb map = { .handle = fb.handles[0] };
    if (ioctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map)) { perror("MAP_DUMB"); return 1; }
    uint8_t *data = mmap(NULL, (size_t)fb.pitches[0] * fb.height, PROT_READ, MAP_SHARED, fd, map.offset);
    if (data == MAP_FAILED) { perror("mmap"); return 1; }
    FILE *out = fopen(argv[2], "wb");
    if (!out) { perror("output"); return 1; }
    fprintf(out, "P6\n%u %u\n255\n", fb.width, fb.height);
    size_t nonzero=0;
    for (unsigned y=0; y<fb.height; y++) for (unsigned x=0; x<fb.width; x++) {
        uint8_t *p=data+y*fb.pitches[0]+x*4;
        if (p[0] || p[1] || p[2]) nonzero++;
        fwrite(p,1,3,out);
    }
    fclose(out);
    printf("nonblack=%zu / %u\n", nonzero, fb.width*fb.height);
    return 0;
}
