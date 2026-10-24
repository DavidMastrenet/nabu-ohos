/* nabu physical sensors exposed through the existing Harmony sensor C ABI. */
#define _GNU_SOURCE
#include "sensor_if.h"
#include "ssc-client.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
static pthread_t worker;
static int started;
static atomic_int stopped,enabled;
static RecordDataCallback callback;
static int64_t intervals[2]={20000000,20000000},last_report[2];
static struct SensorInformation sensors[2];
static int index_for(int id){return id==1?0:id==257?1:-1;}
static void receive(int id,uint64_t ticks,const float v[3],void *unused)
{
 int index=index_for(id);if(index<0||!(atomic_load(&enabled)&(1<<index)))return;
 struct timespec now;clock_gettime(CLOCK_BOOTTIME,&now);
 int64_t ns=(int64_t)now.tv_sec*1000000000+now.tv_nsec;
 pthread_mutex_lock(&lock);
 RecordDataCallback cb=callback;
 if(ns-last_report[index]<intervals[index])cb=NULL;else last_report[index]=ns;
 pthread_mutex_unlock(&lock);
 if(cb){
  /* Verified nabu mount matrix from 81-nabu-ssc.rules: diag(-1,-1,1). */
  float mounted[3]={-v[0],-v[1],v[2]};
  struct SensorEvents e={.sensorId=id,.version=1,.timestamp=ns,.option=3,.mode=0,.data=(uint8_t*)mounted,.dataLen=12};cb(&e);
 }
}
static void *run(void *unused){NabuSscRun(receive,NULL,&stopped);return NULL;}
static int32_t all(struct SensorInformation **info,int32_t *count)
{if(!info||!count)return -22;*info=sensors;*count=2;return 0;}
static int32_t enable(int32_t id)
{int i=index_for(id);if(i<0)return -22;atomic_fetch_or(&enabled,1<<i);fprintf(stderr,"nabu sensor enable %d\n",id);return 0;}
static int32_t disable(int32_t id)
{int i=index_for(id);if(i<0)return -22;atomic_fetch_and(&enabled,~(1<<i));return 0;}
static int32_t batch(int32_t id,int64_t sampling,int64_t report)
{int i=index_for(id);if(i<0||sampling<=0)return -22;pthread_mutex_lock(&lock);intervals[i]=sampling<20000000?20000000:sampling;pthread_mutex_unlock(&lock);return 0;}
static int32_t mode(int32_t id,int32_t m){return index_for(id)<0||(m!=0&&m!=1)?-22:0;}
static int32_t option(int32_t id,uint32_t o){return index_for(id)<0?-22:0;}
static int32_t reg(int32_t group,RecordDataCallback cb)
{if(group!=0||!cb)return -22;pthread_mutex_lock(&lock);callback=cb;pthread_mutex_unlock(&lock);return 0;}
static int32_t unreg(int32_t group,RecordDataCallback cb)
{pthread_mutex_lock(&lock);if(callback==cb)callback=NULL;pthread_mutex_unlock(&lock);return 0;}
static int32_t read_data(int32_t id,struct SensorEvents *e){return -95;}
static int32_t sdc(struct SdcSensorInfo info[]){return -95;}
static const struct SensorInterface interface={all,enable,disable,batch,mode,option,reg,unreg,read_data,sdc};
const struct SensorInterface *NewSensorInterfaceInstance(void)
{
 pthread_mutex_lock(&lock);
 if(!started){
  for(int i=0;i<2;i++){
   struct SensorInformation *s=&sensors[i];s->sensorTypeId=s->sensorId=i?257:1;
   snprintf(s->sensorName,sizeof(s->sensorName),"nabu SSC %s",i?"Gravity":"Accelerometer");
   strcpy(s->vendorName,"Qualcomm/ST");strcpy(s->firmwareVersion,"SSC");strcpy(s->hardwareVersion,"nabu");
   s->maxRange=i?19.6133f:156.9064f;s->accuracy=0.0023942f;s->power=0.5f;
   s->minDelay=20000000;s->maxDelay=1000000000;
  }
  atomic_store(&stopped,0);started=pthread_create(&worker,NULL,run,NULL)==0;
 }
 pthread_mutex_unlock(&lock);return started?&interface:NULL;
}
int32_t FreeSensorInterfaceInstance(void)
{
 if(started){atomic_store(&stopped,1);pthread_join(worker,NULL);started=0;}
 return 0;
}
void SensorDevRegisterDump(void){}
