/* Read-only observation of PCM progress and the buffer actually consumed by DSP. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec/1e9;}
int main(int argc,char **argv){
 int pid=argc>1?atoi(argv[1]):1018;char path[128],line[512];
 snprintf(path,sizeof(path),"/proc/%d/maps",pid);FILE *maps=fopen(path,"r");if(!maps){perror("maps");return 1;}
 unsigned long start=0,end=0;
 while(fgets(line,sizeof(line),maps))if(strstr(line,"/dev/snd/pcmC0D0p")){sscanf(line,"%lx-%lx",&start,&end);break;}
 fclose(maps);if(!start){fprintf(stderr,"No PCM mapping\n");return 1;}
 snprintf(path,sizeof(path),"/proc/%d/mem",pid);int mem=open(path,O_RDONLY|O_CLOEXEC);if(mem<0){perror("mem");return 1;}
 int32_t *data=malloc(end-start);if(pread(mem,data,end-start,start)<0){perror("read PCM");return 1;}
 unsigned bufferFrames=0;
 FILE *params=fopen("/proc/asound/card0/pcm0p/sub0/hw_params","r");
 if(params){while(fgets(line,sizeof(line),params))sscanf(line,"buffer_size: %u",&bufferFrames);fclose(params);}
 if(!bufferFrames){fprintf(stderr,"No PCM buffer size\n");return 1;}
 double began=now();unsigned long long previous=0;
 printf("seconds,state,hw_ptr,appl_ptr,delta,rms,peak\n");
 for(int i=0;i<400;i++){
  FILE *status=fopen("/proc/asound/card0/pcm0p/sub0/status","r");if(!status)break;
  char state[24]="?";unsigned long long hw=0,app=0;
  while(fgets(line,sizeof(line),status)){sscanf(line,"state: %23s",state);sscanf(line,"hw_ptr : %llu",&hw);sscanf(line,"appl_ptr : %llu",&app);}
  fclose(status);unsigned long long delta=hw>=previous?hw-previous:0;
  double sum=0;int peak=0;unsigned count=0;
  if(delta && delta<bufferFrames && pread(mem,data,end-start,start)>0){
   for(unsigned long long frame=previous;frame<hw;frame++){
    int32_t v=data[(frame%bufferFrames)*2];int magnitude=v<0?-v:v;
    if(magnitude>peak)peak=magnitude;sum+=(double)v*v;count++;
   }
  }
  printf("%.5f,%s,%llu,%llu,%llu,%.8f,%.8f\n",now()-began,state,hw,app,delta,count?sqrt(sum/count)/8388608.0:0,peak/8388608.0);
  fflush(stdout);previous=hw;usleep(20000);
 }
 close(mem);free(data);return 0;
}
