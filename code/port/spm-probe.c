#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint16_t u16;
struct data {
 u32 version, uid, tokenid, tokenid_attr, index, name_size;
 u64 name_offset;
 u16 apl, distribution_type;
 u32 id_type;
 u64 ownerid;
 u32 perm_size, extend_perm_size;
 u64 perms_offset, extend_perms_offset, content_ptr;
 u32 reserved[32];
};
struct update { uint8_t count, error; struct data entries[]; };
struct query { u32 tokenid; struct data entry; };
struct ref_update { uint8_t op; u32 id, spawnid; };
struct ref_query { u32 id; u64 count; };
static void check(int ok,const char *name) { if(!ok) {fprintf(stderr,"%s: errno=%d %s\n",name,errno,strerror(errno));exit(1);} }
int main(void) {
 _Static_assert(sizeof(struct data)==208,"API26 entry size");
 _Static_assert(sizeof(struct update)==8,"API26 batch header");
 int fd=open("/dev/access_token_id",O_RDWR);
 check(fd>=0,"open");
 u32 version=0;
 check(!ioctl(fd,_IOR('A',25,u32),&version)&&version==1,"SPM version");
 struct update *batch=calloc(1,8+sizeof(struct data));
 unsigned char content[40]="nabu-port-test";
 content[16]=0x55;content[24]=0x77;
 batch->count=1;
 struct data *d=&batch->entries[0];
 *d=(struct data){.version=1,.uid=123,.tokenid=0xfedcba98,.index=17,.name_size=16,.apl=2,.perm_size=8,.extend_perm_size=8,.perms_offset=16,.extend_perms_offset=24,.content_ptr=(uintptr_t)content};
 u32 token=d->tokenid;
 ioctl(fd,_IOW('A',19,u32),&token);
 check(!ioctl(fd,_IOWR('A',16,struct update),batch),"add");
 check(ioctl(fd,_IOWR('A',16,struct update),batch)<0&&errno==EEXIST,"duplicate");
 unsigned char output[40]={0};
 struct query query={.tokenid=token,.entry=*d};
 query.entry.content_ptr=(uintptr_t)output;
 check(!ioctl(fd,_IOR('A',18,struct query),&query),"get");
 check(query.entry.uid==123&&query.entry.index==17&&!memcmp(content,output,32),"entry roundtrip");
 d->uid=999;d->index=18;content[16]=0xaa;
 check(!ioctl(fd,_IOWR('A',17,struct update),batch),"update");
 check(!ioctl(fd,_IOR('A',18,struct query),&query),"get updated");
 check(query.entry.uid==123&&query.entry.index==18&&output[16]==0xaa,"update preserves uid");
 struct ref_update ref={.op=1,.id=token,.spawnid=75};
 struct ref_query count={.id=token};
 check(!ioctl(fd,_IOW('A',22,struct ref_update),&ref),"increment");
 check(!ioctl(fd,_IOR('A',23,struct ref_query),&count)&&count.count==1,"query count");
 check(!ioctl(fd,_IOW('A',24,u32),&ref.spawnid),"clear spawn");
 check(!ioctl(fd,_IOR('A',23,struct ref_query),&count)&&count.count==0,"cleared count");
 check(!ioctl(fd,_IOW('A',19,u32),&token),"remove");
 check(ioctl(fd,_IOR('A',18,struct query),&query)<0&&errno==ENOENT,"removed entry");
 u32 original,userid=42,got;
 check(!ioctl(fd,_IOR('A',13,u32),&original),"get userid");
 check(!ioctl(fd,_IOW('A',12,u32),&userid),"set userid");
 check(!ioctl(fd,_IOR('A',13,u32),&got)&&got==42,"read userid");
 check(!ioctl(fd,_IOW('A',12,u32),&original),"restore userid");
 puts("API26 SPM: add/get/update/remove, blob roundtrip, duplicate errors, refcounts, userid PASS");
 close(fd);free(batch);
}
