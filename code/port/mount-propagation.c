#include <sys/mount.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    unsigned long flags;
    if (!strcmp(argv[1], "shared")) flags = MS_SHARED;
    else if (!strcmp(argv[1], "slave")) flags = MS_SLAVE;
    else return 2;
    if (mount(NULL, argv[2], NULL, flags, NULL)) {
        perror("mount propagation");
        return 1;
    }
    return 0;
}
