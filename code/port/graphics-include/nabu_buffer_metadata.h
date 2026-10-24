#ifndef NABU_BUFFER_METADATA_H
#define NABU_BUFFER_METADATA_H

#include "buffer_handle.h"
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <mutex>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <vector>

/* A reserved descriptor travels with the dma-buf through NativeBuffer IPC.
 * Metadata must follow the buffer across processes, not live in a local map. */
namespace NabuBufferMetadata {
constexpr uint32_t MAGIC=0x4e4d4431;
constexpr uint32_t VERSION=1;
constexpr size_t SLOT_COUNT=16;
constexpr size_t VALUE_BYTES=4096;
constexpr int OK=0, INVALID=-3, UNSUPPORTED=-5, NO_MEMORY=-6;
struct Entry { uint32_t used,key,length; uint8_t value[VALUE_BYTES]; };
struct Store { uint32_t magic,version; Entry entries[SLOT_COUNT]; };

inline int Attach(BufferHandle *&handle)
{
    if(!handle || handle->reserveFds || handle->reserveInts)return INVALID;
    int fd=static_cast<int>(syscall(SYS_memfd_create,"nabu-buffer-metadata",1U));
    if(fd<0)return NO_MEMORY;
    if(ftruncate(fd,sizeof(Store))!=0){close(fd);return NO_MEMORY;}
    const uint32_t header[]={MAGIC,VERSION};
    if(pwrite(fd,header,sizeof(header),0)!=sizeof(header)){close(fd);return NO_MEMORY;}
    auto *extended=static_cast<BufferHandle*>(realloc(handle,sizeof(BufferHandle)+3*sizeof(int32_t)));
    if(!extended){close(fd);return NO_MEMORY;}
    handle=extended;
    handle->reserveFds=1;handle->reserveInts=2;
    handle->reserve[0]=fd;handle->reserve[1]=MAGIC;handle->reserve[2]=VERSION;
    return OK;
}

inline int Descriptor(const BufferHandle& handle)
{
    if(handle.reserveFds!=1 || handle.reserveInts<2 ||
       static_cast<uint32_t>(handle.reserve[1])!=MAGIC || handle.reserve[2]!=VERSION)return -1;
    struct stat st{};
    if(fstat(handle.reserve[0],&st)!=0 || st.st_size!=sizeof(Store))return -1;
    return handle.reserve[0];
}

/* POSIX locks recover automatically if a client dies. A local mutex also
 * serializes threads because fcntl record locks belong to the process. */
class Access {
    static std::mutex& Mutex(){static std::mutex mutex;return mutex;}
    std::unique_lock<std::mutex> guard_{Mutex()};
    int fd_=-1;
    Store *store_=nullptr;
public:
    explicit Access(const BufferHandle& handle,bool write){
        int fd=Descriptor(handle);if(fd<0)return;
        struct flock lock{};lock.l_type=write?F_WRLCK:F_RDLCK;lock.l_whence=SEEK_SET;
        int ret;do{ret=fcntl(fd,F_SETLKW,&lock);}while(ret<0 && errno==EINTR);
        if(ret<0)return;
        fd_=fd;
        void *p=mmap(nullptr,sizeof(Store),PROT_READ|(write?PROT_WRITE:0),MAP_SHARED,fd,0);
        if(p==MAP_FAILED)return;
        store_=static_cast<Store*>(p);
        if(store_->magic!=MAGIC || store_->version!=VERSION){munmap(store_,sizeof(Store));store_=nullptr;}
    }
    ~Access(){
        if(store_)munmap(store_,sizeof(Store));
        if(fd_>=0){struct flock lock{};lock.l_type=F_UNLCK;lock.l_whence=SEEK_SET;fcntl(fd_,F_SETLK,&lock);}
    }
    Store* get()const{return store_;}
};

inline int Register(const BufferHandle& handle){Access access(handle,false);return access.get()?OK:UNSUPPORTED;}
inline int Set(const BufferHandle& handle,uint32_t key,const std::vector<uint8_t>& value)
{
    if(value.size()>VALUE_BYTES)return INVALID;
    Access access(handle,true);auto *store=access.get();if(!store)return UNSUPPORTED;
    Entry *empty=nullptr,*entry=nullptr;
    for(auto& e:store->entries){if(e.used && e.key==key){entry=&e;break;}if(!e.used && !empty)empty=&e;}
    if(!entry)entry=empty;if(!entry)return NO_MEMORY;
    if(!value.empty())memcpy(entry->value,value.data(),value.size());
    entry->key=key;entry->length=value.size();entry->used=1;return OK;
}
inline int Get(const BufferHandle& handle,uint32_t key,std::vector<uint8_t>& value)
{
    Access access(handle,false);auto *store=access.get();if(!store)return UNSUPPORTED;
    value.clear();
    for(const auto& e:store->entries)if(e.used && e.key==key){
        if(e.length>VALUE_BYTES)return INVALID;
        value.assign(e.value,e.value+e.length);return OK;
    }
    return INVALID;
}
inline int List(const BufferHandle& handle,std::vector<uint32_t>& keys)
{
    Access access(handle,false);auto *store=access.get();if(!store)return UNSUPPORTED;
    keys.clear();for(const auto& e:store->entries)if(e.used)keys.push_back(e.key);return OK;
}
inline int Erase(const BufferHandle& handle,uint32_t key)
{
    Access access(handle,true);auto *store=access.get();if(!store)return UNSUPPORTED;
    for(auto& e:store->entries)if(e.used && e.key==key){e.used=0;e.length=0;}
    return OK;
}
}
#endif
