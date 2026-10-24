/* WCN3991 transport: Linux HCI user channel <-> the image's H4 vendor ABI. */
#include <cstddef>
#include "ohos_bt_vendor_lib.h"
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>
#include <cstdio>

struct HciAddress {sa_family_t family;uint16_t device,channel;};
static BtVendorCallbacksT callbacks;
static int controller=-1,host=-1,bridge=-1,wake=-1,logfd=-1;
static std::thread worker;
static std::atomic<bool> stopped{true},notifyInit{false};

static bool WriteAll(int fd,const uint8_t *data,size_t size){
 while(size){ssize_t n=send(fd,data,size,MSG_NOSIGNAL);if(n<0&&errno==EINTR)continue;if(n<=0)return false;data+=n;size-=n;}return true;
}
static size_t PacketLength(const std::vector<uint8_t>& p){
 if(p.empty())return 0;
 switch(p[0]){
  case 1:return p.size()<4?0:4+p[3];
  case 2:return p.size()<5?0:5+p[3]+(p[4]<<8);
  case 3:return p.size()<4?0:4+p[3];
  case 4:return p.size()<3?0:3+p[2];
  case 5:return p.size()<5?0:5+((p[3]+(p[4]<<8))&0x3fff);
  default:return 1;
 }
}
static void Run(){
 uint8_t packet[65540];std::vector<uint8_t> queued;unsigned tx=0,rx=0;
 while(!stopped.load()){
  if(notifyInit.exchange(false)&&callbacks.initCb)callbacks.initCb(BTC_OP_RESULT_SUCCESS);
  pollfd fds[]={{controller,POLLIN,0},{bridge,POLLIN,0},{wake,POLLIN,0}};
  if(poll(fds,3,50)<0){if(errno==EINTR)continue;break;}
  if(fds[2].revents)break;
  if(fds[0].revents&POLLIN){
   ssize_t n=recv(controller,packet,sizeof(packet),0);if(n<=0)break;
   if(++rx<=24&&logfd>=0)dprintf(logfd,"HCI receive type=%u bytes=%zd\n",packet[0],n);
   if(!WriteAll(bridge,packet,n))break;
  }
  if(fds[1].revents&POLLIN){
   ssize_t n=recv(bridge,packet,sizeof(packet),0);if(n<=0)break;
   queued.insert(queued.end(),packet,packet+n);
   for(size_t length;(length=PacketLength(queued))&&queued.size()>=length;){
    if(queued[0]<1||queued[0]>5){queued.erase(queued.begin());continue;}
    if(++tx<=24&&logfd>=0)dprintf(logfd,"HCI send type=%u bytes=%zu opcode=%02x%02x\n",queued[0],length,length>2?queued[2]:0,length>1?queued[1]:0);
    ssize_t sent;do{sent=send(controller,queued.data(),length,MSG_NOSIGNAL);}while(sent<0&&errno==EINTR);
    if(sent!=(ssize_t)length){if(logfd>=0)dprintf(logfd,"HCI send failed errno=%d\n",errno);stopped.store(true);break;}
    queued.erase(queued.begin(),queued.begin()+length);
   }
  }
  if((fds[0].revents|fds[1].revents)&(POLLERR|POLLHUP|POLLNVAL))break;
 }
 if(logfd>=0)dprintf(logfd,"HCI bridge stopped tx=%u rx=%u\n",tx,rx);
}
static void CloseChannel(){
 stopped.store(true);notifyInit.store(false);
 if(wake>=0){uint64_t one=1;write(wake,&one,sizeof(one));}
 if(host>=0)shutdown(host,SHUT_RDWR);
 if(bridge>=0)shutdown(bridge,SHUT_RDWR);
 if(worker.joinable())worker.join();
 for(int *fd:{&controller,&host,&bridge,&wake}){if(*fd>=0)close(*fd);*fd=-1;}
}
static int OpenChannel(int *channels){
 CloseChannel();
 controller=socket(31,SOCK_RAW|SOCK_CLOEXEC,1);
 if(controller<0){if(logfd>=0)dprintf(logfd,"HCI socket errno=%d\n",errno);return -1;}
 ioctl(controller,_IOW('H',202,int),0);
 HciAddress address={31,0,1};
 if(bind(controller,reinterpret_cast<sockaddr*>(&address),sizeof(address))){
  if(logfd>=0)dprintf(logfd,"HCI user bind errno=%d\n",errno);CloseChannel();return -1;
 }
 int pair[2];
 if(socketpair(AF_UNIX,SOCK_STREAM|SOCK_CLOEXEC,0,pair)){CloseChannel();return -1;}
 host=pair[0];bridge=pair[1];wake=eventfd(0,EFD_CLOEXEC|EFD_NONBLOCK);
 if(wake<0){CloseChannel();return -1;}
 channels[0]=host;stopped.store(false);worker=std::thread(Run);
 if(logfd>=0)dprintf(logfd,"HCI user channel ready fd=%d stream=%d\n",controller,host);
 return 1;
}
static int Initialize(const BtVendorCallbacksT *value,unsigned char *address){
 if(!value||value->size<sizeof(BtVendorCallbacksT))return -1;
 callbacks=*value;
 logfd=open("/data/vendor/bluetooth/nabu-hci.log",O_WRONLY|O_CREAT|O_APPEND|O_CLOEXEC,0640);
 if(logfd>=0)dprintf(logfd,"nabu WCN3991 vendor initialized\n");return 0;
}
static int Operation(BtOpcodeT code,void *parameter){
 switch(code){
  case BT_OP_HCI_CHANNEL_OPEN:return parameter?OpenChannel(static_cast<int*>(parameter)):-1;
  case BT_OP_HCI_CHANNEL_CLOSE:CloseChannel();return 0;
  case BT_OP_INIT:notifyInit.store(true);return 0;
  case BT_OP_GET_LPM_TIMER:if(parameter)*static_cast<uint32_t*>(parameter)=5000;return 0;
  default:return 0;
 }
}
static void Cleanup(){CloseChannel();memset(&callbacks,0,sizeof(callbacks));if(logfd>=0)close(logfd);logfd=-1;}
extern "C" __attribute__((visibility("default")))
const BtVendorInterfaceT BLUETOOTH_VENDOR_LIB_INTERFACE={sizeof(BtVendorInterfaceT),Initialize,Operation,Cleanup};
