#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <grp.h>

int main(int argc, char **argv)
{
    if (argc < 4) return 2;
    uint64_t token = strtoull(argv[1], NULL, 0);
    uid_t uid = strtoul(argv[2], NULL, 0);
    int fd = open("/dev/access_token_id", O_RDWR);
    if (fd < 0 || ioctl(fd, _IOW('A', 2, unsigned long long), &token)) {
        perror("set native token"); return 1;
    }
    close(fd);
    if (setgroups(0, NULL) || setgid(uid) || setuid(uid)) {
        perror("set native uid"); return 1;
    }
    execvp(argv[3], argv + 3);
    perror("exec native command");
    return 1;
}
