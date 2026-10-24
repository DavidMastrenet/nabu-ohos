/* SSC protobuf/QMI transport. Message schemas: libssc and libqmi SSC service. */
#define _GNU_SOURCE
#include "ssc-client.h"
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

/* Linux QRTR UAPI, kept local to avoid mixed libc/kernel socket headers. */
#ifndef AF_QIPCRTR
#define AF_QIPCRTR 42
#endif
#define QRTR_PORT_CTRL UINT32_C(0xfffffffe)
#define QRTR_TYPE_NEW_SERVER 4
#define QRTR_TYPE_NEW_LOOKUP 10
struct sockaddr_qrtr { sa_family_t sq_family; uint32_t sq_node,sq_port; };
struct qrtr_ctrl_pkt { uint32_t cmd;struct { uint32_t service,instance,node,port; } server; };

typedef struct { unsigned char b[4096]; size_t n; } Buffer;
typedef struct { const unsigned char *p; size_t n; } Slice;
typedef struct { unsigned field, wire; Slice value; uint64_t number; } Field;
static void u16(unsigned char *p,unsigned n) { p[0]=n;p[1]=n>>8; }
static unsigned r16(const unsigned char *p) { return p[0]|p[1]<<8; }
static void append(Buffer *b,const void *p,size_t n) { if(b->n+n<=sizeof(b->b)){memcpy(b->b+b->n,p,n);b->n+=n;} }
static void varint(Buffer *b,uint64_t n) { do { unsigned char v=(n&127)|(n>127?128:0);append(b,&v,1);n>>=7; }while(n); }
static void bytes(Buffer *b,unsigned f,const void *p,size_t n) {varint(b,f*8+2);varint(b,n);append(b,p,n);}
static void fixed(Buffer *b,unsigned f,uint64_t n,int size) {varint(b,f*8+(size==8?1:5));append(b,&n,size);}
static int readvar(Slice *s,uint64_t *v) { *v=0;for(int shift=0;s->n&&shift<64;shift+=7){unsigned c=*s->p++;s->n--;*v|=(uint64_t)(c&127)<<shift;if(!(c&128))return 1;}return 0; }
static int next(Slice *s,Field *f)
{
 uint64_t tag,n;memset(f,0,sizeof(*f));if(!readvar(s,&tag))return 0;
 f->field=tag>>3;f->wire=tag&7;
 if(f->wire==0)return readvar(s,&f->number);
 if(f->wire==1)n=8;else if(f->wire==5)n=4;else if(f->wire==2){if(!readvar(s,&n))return 0;}else return 0;
 if(n>s->n)return 0;f->value=(Slice){s->p,n};if(f->wire==1||f->wire==5)memcpy(&f->number,s->p,n);
 s->p+=n;s->n-=n;return 1;
}
typedef struct { int id;const char *type;uint64_t low,high;int enabled; } Sensor;
typedef struct {
 int fd;struct sockaddr_qrtr server;unsigned tx;
 Sensor sensors[2];NabuSensorCallback callback;void *user;
} Client;
static void send_request(Client *c,uint64_t low,uint64_t high,unsigned id,const void *data,size_t length)
{
 Buffer uid={0},body={0},request={0};unsigned char config[]={8,1,16,0};
 fixed(&uid,1,low,8);fixed(&uid,2,high,8);bytes(&body,2,data,length);
 bytes(&request,1,uid.b,uid.n);fixed(&request,2,id,4);bytes(&request,3,config,sizeof(config));bytes(&request,4,body.b,body.n);
 unsigned char packet[4200]={0};u16(packet+1,++c->tx);u16(packet+3,0x20);u16(packet+5,request.n+9);
 packet[7]=1;u16(packet+8,request.n+2);u16(packet+10,request.n);memcpy(packet+12,request.b,request.n);
 size_t n=12+request.n;packet[n++]=0x10;u16(packet+n,1);n+=2;packet[n++]=1;
 if(sendto(c->fd,packet,n,0,(struct sockaddr*)&c->server,sizeof(c->server))<0)perror("ssc send");
}
static void discover(Client *c)
{
 for(int i=0;i<2;i++) {
  Buffer b={0};bytes(&b,1,c->sensors[i].type,strlen(c->sensors[i].type));
  varint(&b,16);varint(&b,1);varint(&b,24);varint(&b,1);
  send_request(c,UINT64_C(0xabababababababab),UINT64_C(0xabababababababab),512,b.b,b.n);
 }
}
static void process_body(Client *c,unsigned msg,uint64_t ticks,Slice payload)
{
 if(msg==768) {
  Slice type={0};uint64_t low=0,high=0;Field f;
  while(next(&payload,&f)) {
   if(f.field==1)type=f.value;
   if(f.field==2){Slice uid=f.value;Field u;while(next(&uid,&u)){if(u.field==1)low=u.number;if(u.field==2)high=u.number;}}
  }
  for(int i=0;i<2;i++) {
   Sensor *s=&c->sensors[i];if(type.n!=strlen(s->type)||memcmp(type.p,s->type,type.n)||!low||s->enabled)continue;
   s->low=low;s->high=high;s->enabled=1;
   float rate=50;Buffer b={0};uint32_t bits;memcpy(&bits,&rate,4);fixed(&b,1,bits,4);
   fprintf(stderr,"SSC %s uid=%016llx:%016llx\n",s->type,(unsigned long long)high,(unsigned long long)low);
   send_request(c,low,high,513,b.b,b.n);
  }
 }
}
static void protobuf(Client *c,Slice p)
{
 uint64_t low=0,high=0;Slice scan=p;Field f;
 while(next(&scan,&f))if(f.field==1){Slice uid=f.value;Field u;while(next(&uid,&u)){if(u.field==1)low=u.number;if(u.field==2)high=u.number;}}
 while(next(&p,&f))if(f.field==2) {
  Slice body=f.value,payload={0};unsigned msg=0;uint64_t ticks=0;Field b;
  while(next(&body,&b)){if(b.field==1)msg=b.number;if(b.field==2)ticks=b.number;if(b.field==3)payload=b.value;}
  if(low==UINT64_C(0xabababababababab)){process_body(c,msg,ticks,payload);continue;}
  if(msg!=1025)continue;
  float v[3]={0};int count=0;Field a;
  while(next(&payload,&a))if(a.field==1){
   if(a.wire==5&&count<3)memcpy(v+count++,&a.number,4);
   else if(a.wire==2)for(size_t i=0;i+4<=a.value.n&&count<3;i+=4)memcpy(v+count++,a.value.p+i,4);
  }
  if(count!=3)continue;
  for(int i=0;i<2;i++)if(c->sensors[i].low==low&&c->sensors[i].high==high)
   c->callback(c->sensors[i].id,ticks,v,c->user);
 }
}
int NabuSscRun(NabuSensorCallback callback,void *user,atomic_int *stop)
{
 Client c={.fd=-1,.sensors={{1,"accel",0,0,0},{257,"gravity",0,0,0}},.callback=callback,.user=user};
 c.fd=socket(AF_QIPCRTR,SOCK_DGRAM|SOCK_CLOEXEC,0);if(c.fd<0){perror("ssc socket");return -1;}
 struct sockaddr_qrtr local={.sq_family=AF_QIPCRTR};socklen_t sz=sizeof(local);
 if(getsockname(c.fd,(struct sockaddr*)&local,&sz)){perror("ssc getname");close(c.fd);return -1;}
 struct sockaddr_qrtr ctrl={.sq_family=AF_QIPCRTR,.sq_node=local.sq_node,.sq_port=QRTR_PORT_CTRL};
 struct qrtr_ctrl_pkt lookup={.cmd=QRTR_TYPE_NEW_LOOKUP};lookup.server.service=400;
 if(sendto(c.fd,&lookup,sizeof(lookup),0,(struct sockaddr*)&ctrl,sizeof(ctrl))<0)perror("ssc lookup");
 while(!atomic_load(stop)) {
  struct pollfd pfd={c.fd,POLLIN,0};if(poll(&pfd,1,1000)<=0)continue;
  unsigned char packet[65536];struct sockaddr_qrtr from;sz=sizeof(from);
  ssize_t n=recvfrom(c.fd,packet,sizeof(packet),0,(struct sockaddr*)&from,&sz);if(n<0)continue;
  if(from.sq_port==QRTR_PORT_CTRL) {
   if(n<(ssize_t)sizeof(lookup))continue;struct qrtr_ctrl_pkt event;memcpy(&event,packet,sizeof(event));
   if(event.cmd==QRTR_TYPE_NEW_SERVER&&event.server.service==400&&event.server.port) {
    c.server=(struct sockaddr_qrtr){.sq_family=AF_QIPCRTR,.sq_node=event.server.node,.sq_port=event.server.port};
    fprintf(stderr,"SSC server=%u:%u\n",c.server.sq_node,c.server.sq_port);discover(&c);
   }
   continue;
  }
  if(n<7)continue;unsigned id=r16(packet+3);if(packet[0]!=4||(id!=0x21&&id!=0x22))continue;
  for(size_t pos=7;pos+3<=(size_t)n;) {
   unsigned tag=packet[pos],len=r16(packet+pos+1);pos+=3;if(pos+len>(size_t)n)break;
   if(tag==2&&len>=2){unsigned size=r16(packet+pos);if(size<=len-2)protobuf(&c,(Slice){packet+pos+2,size});}
   pos+=len;
  }
 }
 close(c.fd);return 0;
}
#ifdef NABU_SSC_PROBE
static atomic_int stopped;
static void alarm_stop(int n){atomic_store(&stopped,1);}
static void measurement(int id,uint64_t ticks,const float v[3],void *user)
{printf("sensor=%d ticks=%llu xyz=%.5f,%.5f,%.5f\n",id,(unsigned long long)ticks,v[0],v[1],v[2]);fflush(stdout);}
int main(void){signal(SIGALRM,alarm_stop);alarm(5);return NabuSscRun(measurement,NULL,&stopped);}
#endif
