#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
struct hci_address {sa_family_t family;uint16_t device,channel;};
int main(void){
 int fd=socket(31,SOCK_RAW|SOCK_CLOEXEC,1);if(fd<0){perror("HCI socket");return 1;}
 ioctl(fd,_IOW('H',202,int),0);
 struct hci_address address={31,0,1};
 if(bind(fd,(struct sockaddr*)&address,sizeof(address))){perror("HCI user bind");close(fd);return 1;}
 uint8_t command[]={1,3,12,0};
 if(write(fd,command,sizeof(command))!=sizeof(command)){perror("HCI reset");return 1;}
 struct pollfd p={fd,POLLIN,0};int ret=poll(&p,1,5000);
 if(ret>0){uint8_t reply[1024];ssize_t size=read(fd,reply,sizeof(reply));printf("HCI reset response:");for(int i=0;i<size;i++)printf(" %02x",reply[i]);puts("");ret=size>=7&&reply[0]==4&&reply[1]==14&&reply[6]==0?0:1;}
 else{printf("HCI reset timeout=%d errno=%d\n",ret,errno);ret=1;}
 close(fd);return ret;
}
