#include "nabu_buffer_metadata.h"
#include <cassert>
#include <cstdio>
#include <sys/wait.h>
#include <thread>
using namespace NabuBufferMetadata;
int main(){
    auto *h=static_cast<BufferHandle*>(calloc(1,sizeof(BufferHandle)));h->fd=-1;
    assert(Attach(h)==0);assert(Register(*h)==0);
    std::vector<uint8_t> out;
    assert(Set(*h,1,{1,2,3,4})==0);assert(Get(*h,1,out)==0 && out==std::vector<uint8_t>({1,2,3,4}));
    assert(Set(*h,2,std::vector<uint8_t>(VALUE_BYTES,7))==0);
    assert(Set(*h,2,std::vector<uint8_t>(VALUE_BYTES+1,7))==INVALID);
    pid_t child=fork();assert(child>=0);
    if(!child){
        int fd=dup(h->reserve[0]);assert(fd>=0);close(h->reserve[0]);h->reserve[0]=fd;
        assert(Get(*h,1,out)==0 && out[0]==1);assert(Set(*h,1,{9,8,7})==0);
        for(int i=0;i<100;i++)assert(Set(*h,3,{static_cast<uint8_t>(i)})==0);
        _exit(0);
    }
    std::thread a([&]{for(int i=0;i<100;i++)assert(Set(*h,4,{5,6})==0);});
    std::thread b([&]{for(int i=0;i<100;i++)assert(Get(*h,2,out)==0 && out.size()==VALUE_BYTES);});
    a.join();b.join();int status;assert(waitpid(child,&status,0)==child && WIFEXITED(status) && WEXITSTATUS(status)==0);
    assert(Get(*h,1,out)==0 && out==std::vector<uint8_t>({9,8,7}));
    std::vector<uint32_t> keys;assert(List(*h,keys)==0 && keys.size()==4);
    assert(Erase(*h,1)==0 && Get(*h,1,out)==INVALID);
    assert(Set(*h,1,{})==0 && Get(*h,1,out)==0 && out.empty());
    for(int i=5;i<17;i++)assert(Set(*h,i,{1})==0);
    assert(Set(*h,17,{1})==NO_MEMORY);
    close(h->reserve[0]);free(h);
    puts("PASS cross-process fd transfer, concurrent readers/writers, bounds, erase and capacity");
}
