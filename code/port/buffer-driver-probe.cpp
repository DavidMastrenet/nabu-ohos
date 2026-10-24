#include "idisplay_buffer_vdi.h"
#include "v1_0/display_composer_type.h"
#include <cassert>
#include <cstdio>
#include <sys/wait.h>
#include <unistd.h>
using namespace OHOS::HDI::Display::Buffer::V1_0;
using namespace OHOS::HDI::Display::Composer::V1_0;
int main(){
    auto *b=CreateDisplayBufferVdi();assert(b);
    AllocInfo a={64,64,HBM_USE_CPU_READ|HBM_USE_CPU_WRITE,PIXEL_FMT_RGBA_8888,0};
    BufferHandle *h=nullptr;assert(b->AllocMem(a,h)==0 && h);
    assert(h->reserveFds==1 && h->reserveInts==2 && b->RegisterBuffer(*h)==0);
    assert(b->SetMetadata(*h,1,{1,2,3,4})==0);
    pid_t child=fork();assert(child>=0);
    if(!child){std::vector<uint8_t> v;assert(b->GetMetadata(*h,1,v)==0 && v[0]==1);assert(b->SetMetadata(*h,1,{9,8})==0);_exit(0);}
    int status;assert(waitpid(child,&status,0)==child && WIFEXITED(status) && WEXITSTATUS(status)==0);
    std::vector<uint8_t> v;assert(b->GetMetadata(*h,1,v)==0 && v==std::vector<uint8_t>({9,8}));
    auto *pixels=b->Mmap(*h);assert(pixels);assert(b->Unmap(*h)==0);
    b->FreeMem(*h);DestroyDisplayBufferVdi(b);
    puts("PASS native VDI DRM allocation, metadata descriptor, cross-process update, mapping and release");
}
