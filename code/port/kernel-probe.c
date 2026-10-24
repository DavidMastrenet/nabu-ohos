#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

struct binder_feature_set { uint64_t feature_set; };
struct access_token { uint64_t sender_tokenid, first_tokenid, reserved[2]; };
struct binder_sender_info { struct access_token tokens; uint64_t sender_pid_nr; };
#define GET_TOKEN _IOR('A', 1, unsigned long long)
#define SET_TOKEN _IOW('A', 2, unsigned long long)
#define GET_FTOKEN _IOR('A', 3, unsigned long long)
#define SET_FTOKEN _IOW('A', 4, unsigned long long)
#define FEATURES _IOWR('b', 30, struct binder_feature_set)
#define SENDER _IOWR('b', 32, struct binder_sender_info)
static void check(int result, const char *what) {
    if (result < 0) { perror(what); exit(1); }
}
int main(void) {
    int fd = open("/dev/access_token_id", O_RDWR);
    check(fd, "open access_token_id");
    uint64_t original, first, got, token = 0x11223344, ftoken = 0x55667788;
    check(ioctl(fd, GET_TOKEN, &original), "get token");
    check(ioctl(fd, GET_FTOKEN, &first), "get first token");
    check(ioctl(fd, SET_TOKEN, &token), "set token");
    check(ioctl(fd, SET_FTOKEN, &ftoken), "set first token");
    check(ioctl(fd, GET_TOKEN, &got), "read token");
    if (got != token) return 2;
    check(ioctl(fd, GET_FTOKEN, &got), "read first token");
    if (got != ftoken) return 3;
    pid_t pid=fork();
    check(pid, "fork");
    if (!pid) {
        check(ioctl(fd, GET_TOKEN, &got), "child inherited token");
        _exit(got==token ? 0 : 4);
    }
    int status;
    waitpid(pid, &status, 0);
    check(ioctl(fd, SET_TOKEN, &original), "restore token");
    check(ioctl(fd, SET_FTOKEN, &first), "restore first token");
    close(fd);
    if (!WIFEXITED(status) || WEXITSTATUS(status)) return 4;
    puts("AccessToken: get/set/restore and fork inheritance PASS");
    fd = open("/dev/binder", O_RDWR);
    check(fd, "open binder");
    struct binder_feature_set features={0};
    check(ioctl(fd, FEATURES, &features), "Binder features");
    printf("Binder features: %llu (expected 5)\n", (unsigned long long)features.feature_set);
    if(features.feature_set != 5) return 5;
    struct binder_sender_info sender={0};
    check(ioctl(fd, SENDER, &sender), "Binder sender info");
    puts("Binder feature and sender info ioctls PASS; transaction propagation test pending");
    close(fd);
    return 0;
}
