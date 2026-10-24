#include "idisplay_composer_vdi.h"
#include "idisplay_buffer_vdi.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <ucontext.h>
using namespace OHOS::HDI::Display::Composer::V1_0;
using namespace OHOS::HDI::Display::Buffer::V1_0;
static uint32_t screen=UINT32_MAX;
static void plug(uint32_t id,bool connected,void*) {if(connected) screen=id; printf("Screen %u connected=%d\n",id,connected);}
static void check(int r,const char *what) {printf("%s=%d\n",what,r);fflush(stdout);if(r)exit(1);}
int main() {
 struct sigaction sa={};sa.sa_flags=SA_SIGINFO;
 sa.sa_sigaction=[](int, siginfo_t*info,void*context) {
  auto *u=(ucontext_t*)context;
  fprintf(stderr,"fault=%p PC=%llx LR=%llx\n",info->si_addr,(unsigned long long)u->uc_mcontext.pc,(unsigned long long)u->uc_mcontext.regs[30]);
  FILE *f=fopen("/proc/self/maps","r");char line[512];while(f&&fgets(line,sizeof(line),f))fputs(line,stderr);_exit(99);
 };
 sigaction(SIGSEGV,&sa,nullptr);
 auto *c=CreateComposerVdi();auto *b=CreateDisplayBufferVdi();
 if(!c||!b)return 1;
 check(c->RegHotPlugCallback(plug,nullptr),"hotplug");
 if(screen==UINT32_MAX){puts("No screen");return 2;}
 std::vector<DisplayModeInfo> modes;
 check(c->GetDisplaySupportedModes(screen,modes),"modes");
 for(auto &m:modes)printf("Mode %d: %dx%d @%u\n",m.id,m.width,m.height,m.freshRate);
 if(modes.empty())return 3;
 auto m=modes[0];
 check(c->SetDisplayMode(screen,m.id),"set mode");
 AllocInfo a={(uint32_t)m.width,(uint32_t)m.height,HBM_USE_CPU_READ|HBM_USE_CPU_WRITE|HBM_USE_MEM_FB,PIXEL_FMT_BGRA_8888,0};
 BufferHandle *h=nullptr;check(b->AllocMem(a,h),"allocate");
 auto *pixels=(unsigned char*)b->Mmap(*h);if(!pixels||pixels==MAP_FAILED)return 4;
 for(int y=0;y<m.height;y++)for(int x=0;x<m.width;x++) {
  auto *p=(uint32_t*)(pixels+y*h->stride+x*4);
  const uint32_t colors[]={0xffeeeeee,0xffffff00,0xff00ffff,0xff00ff00,0xffff00ff,0xffff0000,0xff0000ff,0xff181818};
  *p=colors[x*8/m.width];
 }
 b->FlushCache(*h);
 bool flush=true;check(c->PrepareDisplayLayers(screen,flush),"prepare");
 check(c->SetDisplayClientBuffer(screen,*h,-1),"client buffer");
 int fence=-1;check(c->Commit(screen,fence),"commit");if(fence>=0)close(fence);
 puts("NABU OHOS DRM COLOR BARS COMMITTED");
 sleep(8);
 b->Unmap(*h);b->FreeMem(*h);
}
