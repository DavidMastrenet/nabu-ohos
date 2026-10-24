#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
struct data {
 uint32_t version, uid, tokenid, tokenid_attr, index, name_size;
 uint64_t name_offset;
 uint16_t apl, distribution_type;
 uint32_t id_type;
 uint64_t ownerid;
 uint32_t perm_size, extend_perm_size;
 uint64_t perms_offset, extend_perms_offset, content_ptr;
 uint32_t reserved[32];
};
struct query { uint32_t tokenid; struct data entry; };
struct permission { uint32_t token, opcode; uint8_t granted; };
int main(int argc, char **argv) {
 if (argc != 2) return 2;
 int fd = open("/dev/access_token_id", O_RDWR);
 unsigned char content[32768] = {0};
 struct query q = {.tokenid = strtoul(argv[1], NULL, 0), .entry = {
  .name_size=1024, .perm_size=4096, .extend_perm_size=16384,
  .perms_offset=1024, .extend_perms_offset=5120,
  .content_ptr=(uintptr_t)content}};
 if (ioctl(fd, _IOR('A',18,struct query), &q)) { perror("SPM get"); return 1; }
 printf("token=%u uid=%u index=%u name=%s permissions=%u extended=%u\n",
  q.tokenid,q.entry.uid,q.entry.index,content,q.entry.perm_size,q.entry.extend_perm_size);
 unsigned granted=0, mismatches=0, errors=0;
 for (unsigned i=0;i<q.entry.perm_size*8;i++) {
  unsigned expected=(content[1024+i/8]>>(i%8))&1;
  struct permission p={.token=q.tokenid,.opcode=i};
  errno=0;
  int r=ioctl(fd,_IOW('A',7,struct permission),&p);
  if (expected) {granted++;printf(" granted opcode=%u legacy=%d errno=%d\n",i,r,errno);}
  if (r<0) errors++; else if ((unsigned)r!=expected) mismatches++;
 }
 printf("SPM grants=%u legacy_errors=%u mismatches=%u\n",granted,errors,mismatches);
 return 0;
}
