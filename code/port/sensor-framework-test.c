#include <sensors/oh_sensor.h>
#include <stdio.h>
#include <stdatomic.h>
#include <unistd.h>
static atomic_int samples;
static void event(Sensor_Event *event)
{
 float *v=NULL;uint32_t n=0;int ret=OH_SensorEvent_GetData(event,&v,&n);
 int count=atomic_fetch_add(&samples,1);
 if(count<3&&ret==0&&n>=3){printf("gravity %.5f %.5f %.5f\n",v[0],v[1],v[2]);fflush(stdout);}
}
int main(void)
{
 Sensor_SubscriptionId *id=OH_Sensor_CreateSubscriptionId();
 Sensor_SubscriptionAttribute *attribute=OH_Sensor_CreateSubscriptionAttribute();
 Sensor_Subscriber *subscriber=OH_Sensor_CreateSubscriber();
 OH_SensorSubscriptionId_SetType(id,SENSOR_TYPE_GRAVITY);
 OH_SensorSubscriptionAttribute_SetSamplingInterval(attribute,40000000);
 OH_SensorSubscriber_SetCallback(subscriber,event);
 int ret=OH_Sensor_Subscribe(id,attribute,subscriber);printf("subscribe=%d\n",ret);fflush(stdout);
 sleep(3);printf("samples=%d unsubscribe=%d\n",atomic_load(&samples),OH_Sensor_Unsubscribe(id,subscriber));
 OH_Sensor_DestroySubscriber(subscriber);OH_Sensor_DestroySubscriptionAttribute(attribute);OH_Sensor_DestroySubscriptionId(id);
 return ret||atomic_load(&samples)==0;
}
