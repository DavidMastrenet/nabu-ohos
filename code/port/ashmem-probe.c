#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ashmem/uapi/ashmem.h"

#define CHECK(c) do { if (!(c)) { perror(#c); return 1; } } while (0)
int main(void)
{
    int fd = open("/dev/ashmem", O_RDWR);
    CHECK(fd >= 0);
    CHECK(ioctl(fd, ASHMEM_SET_NAME, "nabu-smq-test") == 0);
    CHECK(ioctl(fd, ASHMEM_SET_SIZE, 4096) == 0);
    CHECK(ioctl(fd, ASHMEM_GET_SIZE, 0) == 4096);
    char *a = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    char *b = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    CHECK(a != MAP_FAILED && b != MAP_FAILED);
    strcpy(a, "shared-parent");
    CHECK(strcmp(b, "shared-parent") == 0);
    pid_t child = fork();
    CHECK(child >= 0);
    if (child == 0) {
        if (strcmp(b, "shared-parent")) _exit(1);
        strcpy(b, "shared-child");
        _exit(0);
    }
    int status;
    CHECK(waitpid(child, &status, 0) == child && status == 0);
    CHECK(strcmp(a, "shared-child") == 0);
    struct ashmem_pin range = {0, 4096};
    CHECK(ioctl(fd, ASHMEM_UNPIN, &range) == 0);
    CHECK(ioctl(fd, ASHMEM_PIN, &range) == ASHMEM_NOT_PURGED);
    CHECK(ioctl(fd, ASHMEM_UNPIN, &range) == 0);
    CHECK(ioctl(fd, ASHMEM_PURGE_ALL_CACHES, 0) >= 1);
    CHECK(ioctl(fd, ASHMEM_PIN, &range) == ASHMEM_WAS_PURGED);
    CHECK(a[0] == 0 && b[0] == 0);
    close(fd);
    strcpy(a, "mapping-after-close");
    CHECK(strcmp(b, "mapping-after-close") == 0);
    munmap(a, 4096);
    munmap(b, 4096);
    puts("NABU_ASHMEM_SHARED_FORK_PURGE_PASS");
    return 0;
}
