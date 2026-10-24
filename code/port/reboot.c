#include <linux/reboot.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv) {
    const char *target = argc > 1 ? argv[1] : "bootloader";
    if (argc > 2 && strcmp(argv[1], "--after") == 0) {
        sleep((unsigned int)atoi(argv[2]));
        target = argc > 3 ? argv[3] : "bootloader";
    }
    sync();
    syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
        LINUX_REBOOT_CMD_RESTART2, target);
    perror("reboot");
    return 1;
}
