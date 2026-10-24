#include <hdf_remote_service.h>
#include <hdf_sbuf_ipc.h>
#include <cstdio>
int main(){
 auto *remote=HdfRemoteServiceGet(5003);if(!remote||!remote->dispatcher)return 1;
 HdfRemoteServiceSetInterfaceDesc(remote,"OHOS.StorageManager.IStorageManager");
 const char *names[]={"system","total","free"};
 for(int code=9;code<=11;code++){
  OHOS::MessageParcel data,reply;
  data.WriteInterfaceToken(u"OHOS.StorageManager.IStorageManager");
  auto *in=ParcelToSbuf(&data),*out=ParcelToSbuf(&reply);
  int transport=remote->dispatcher->Dispatch(remote,code,in,out);
  int result=reply.ReadInt32();long long bytes=reply.ReadInt64();
  printf("%s transport=%d result=%d bytes=%lld GiB=%.3f\n",names[code-9],transport,result,bytes,bytes/1073741824.0);
  HdfSbufRecycle(in);HdfSbufRecycle(out);
 }
 HdfRemoteServiceRecycle(remote);return 0;
}
