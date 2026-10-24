#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
int main(int argc,char **argv) {
 void *lib=dlopen("libpermsetproc_shared.z.so",RTLD_NOW);
 if(!lib){puts(dlerror());return 1;}
 bool (*code)(const char*,uint32_t*)=dlsym(lib,"TransferPermissionToOpcode");
 int (*check)(uint32_t,int32_t,bool*)=dlsym(lib,"GetPermissionFromKernel");
 if(!code||!check){puts(dlerror());return 1;}
 for(int i=2;i<argc;i++){
  uint32_t op=0;bool granted=false;
  bool mapped=code(argv[i],&op);
  int ret=mapped?check(strtoul(argv[1],0,0),op,&granted):-999;
  printf("%s opcode=%u mapped=%d ret=%d granted=%d\n",argv[i],op,mapped,ret,granted);
 }
 return 0;
}
