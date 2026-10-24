/* nabu Motion HDI: real OHOS gravity events -> standard rotation motion events. */
#include "v1_1/motion_interface_stub.h"
#include <hdf_remote_service.h>
#include <hdf_sbuf_ipc.h>
#include <ipc_skeleton.h>
#include <iservmgr_hdi.h>
#include <sensors/oh_sensor.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <unistd.h>

using namespace OHOS;
using namespace OHOS::HDI::Motion;
using V1_0::IMotionCallback;
using V1_0::HdfMotionEvent;

class NabuMotion final : public V1_1::IMotionInterface {
    std::mutex mutex_;
    std::vector<sptr<IMotionCallback>> callbacks_;
    bool enabled_ = false;
    int candidate_ = -1, stable_ = -1, count_ = 0;
    int displayOffset_ = 0;
    std::chrono::steady_clock::time_point candidateSince_;
public:
    explicit NabuMotion(int offset):displayOffset_(offset){}
    bool Enabled(){std::lock_guard<std::mutex> lock(mutex_);return enabled_;}
    int32_t EnableMotion(int32_t type) override {
        fprintf(stderr,"motion enable type=%d\n",type);
        if(type != V1_0::HDF_MOTION_TYPE_ROTATION) return HDF_ERR_NOT_SUPPORT;
        std::lock_guard<std::mutex> lock(mutex_);
        enabled_ = true; stable_ = -1; candidate_ = -1; count_ = 0;
        return HDF_SUCCESS;
    }
    int32_t DisableMotion(int32_t type) override {
        if(type != V1_0::HDF_MOTION_TYPE_ROTATION) return HDF_ERR_NOT_SUPPORT;
        std::lock_guard<std::mutex> lock(mutex_);enabled_=false;return HDF_SUCCESS;
    }
    int32_t Register(const sptr<IMotionCallback>& cb) override {
        if(cb==nullptr)return HDF_ERR_INVALID_PARAM;
        std::lock_guard<std::mutex> lock(mutex_);
        callbacks_.push_back(cb);stable_=-1;
        fprintf(stderr,"motion callback registered count=%zu\n",callbacks_.size());
        return HDF_SUCCESS;
    }
    int32_t Unregister(const sptr<IMotionCallback>& cb) override {
        /* This HDI has one consumer, the system MotionService. */
        std::lock_guard<std::mutex> lock(mutex_);callbacks_.clear();return HDF_SUCCESS;
    }
    int32_t SetMotionConfig(int32_t type,const std::vector<uint8_t>& data) override {
        return type==V1_0::HDF_MOTION_TYPE_ROTATION ? HDF_SUCCESS : HDF_ERR_NOT_SUPPORT;
    }
    void Gravity(const float *v) {
        float x=v[0],y=v[1],z=v[2];
        /* Ignore lying flat and movement with no reliable gravity direction. */
        if(x*x+y*y<16.0f || x*x+y*y < z*z*0.25f)return;
        double angle=atan2(-x,y)*180.0/M_PI;
        if(angle<0)angle+=360.0;
        int quarter=static_cast<int>(lround(angle/90.0))%4;
        double distance=fabs(angle-quarter*90.0);distance=std::min(distance,360.0-distance);
        if(distance>30.0)return;
        /* Android sensor axes: +Y portrait, -X left landscape. */
        int status=(quarter+displayOffset_)%4;
        std::vector<sptr<IMotionCallback>> targets;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if(!enabled_)return;
            auto now=std::chrono::steady_clock::now();
            if(candidate_!=status){candidate_=status;candidateSince_=now;count_=1;return;}
            count_++;
            if(stable_==status || count_<3 || now-candidateSince_<std::chrono::milliseconds(200))return;
            stable_=status;targets=callbacks_;
        }
        HdfMotionEvent event={V1_0::HDF_MOTION_TYPE_ROTATION,1,status,1,{status}};
        for(const auto& cb:targets){int ret=cb->OnDataEvent(event);fprintf(stderr,"motion rotation status=%d callback=%d xyz=%.3f,%.3f,%.3f\n",status,ret,x,y,z);}
    }
};

constexpr uint32_t NABU_TEST_GRAVITY=0x4e4142;
class NabuMotionStub final : public V1_1::MotionInterfaceStub {
    sptr<NabuMotion> impl_;
public:
    explicit NabuMotionStub(const sptr<NabuMotion>& impl):MotionInterfaceStub(impl),impl_(impl){}
    int32_t OnRemoteRequest(uint32_t code,MessageParcel& data,MessageParcel& reply,MessageOption& option) override {
        if(code!=NABU_TEST_GRAVITY)return MotionInterfaceStub::OnRemoteRequest(code,data,reply,option);
        if(IPCSkeleton::GetCallingUid()!=0)return HDF_ERR_NOPERM;
        int status=data.ReadInt32();if(status<0||status>3)return HDF_ERR_INVALID_PARAM;
        float xyz[4][3]={{0,9.80665f,0},{-9.80665f,0,0},{0,-9.80665f,0},{9.80665f,0,0}};
        for(int i=0;i<5;i++){impl_->Gravity(xyz[status]);usleep(100000);}
        fprintf(stderr,"test gravity status=%d completed\n",status);return HDF_SUCCESS;
    }
};

static NabuMotion *active;
static void SensorEvent(Sensor_Event *event) {
    float *values=nullptr;uint32_t count=0;
    if(OH_SensorEvent_GetData(event,&values,&count)==0 && count>=3 && active)active->Gravity(values);
}

static int RegisterService(const sptr<IRemoteObject>& object) {
    HdfRemoteService *manager=HdfRemoteServiceGet(5100);
    if(!manager||!manager->dispatcher)return HDF_FAILURE;
    HdfRemoteServiceSetInterfaceDesc(manager,"HDI.IServiceManager.V1_0");
    MessageParcel data,reply;
    data.WriteInterfaceToken(u"HDI.IServiceManager.V1_0");
    data.WriteCString("motion_interface_service");
    data.WriteUint16(4); /* DEVICE_CLASS_SENSOR */
    data.WriteUint32(0x4e414255); /* Standalone nabu device. */
    data.WriteRemoteObject(object);
    data.WriteCString("nabu gravity rotation");
    data.WriteCString("ohos.hdi.motion.v1_1.IMotionInterface");
    HdfSBuf *in=ParcelToSbuf(&data),*out=ParcelToSbuf(&reply);
    int ret=manager->dispatcher->Dispatch(manager,1,in,out);
    HdfSbufRecycle(in);HdfSbufRecycle(out);HdfRemoteServiceRecycle(manager);
    return ret;
}

static int SubscribeDisplay(bool subscribe=true) {
    auto *remote=HdfRemoteServiceGet(4607);
    if(!remote||!remote->dispatcher)return HDF_FAILURE;
    MessageParcel data,reply;
    data.WriteInterfaceToken(u"OHOS.IScreenSessionManager");data.WriteInt32(700);
    HdfSBuf *in=ParcelToSbuf(&data),*out=ParcelToSbuf(&reply);
    int ret=remote->dispatcher->Dispatch(remote,subscribe?2581:2582,in,out);
    HdfSbufRecycle(in);HdfSbufRecycle(out);HdfRemoteServiceRecycle(remote);
    fprintf(stderr,"DMS %ssubscribe rotation=%d\n",subscribe?"":"un",ret);return ret;
}

int main(int argc,char **argv) {
    setvbuf(stderr,nullptr,_IOLBF,0);
    if(argc==2&&!strcmp(argv[1],"--subscribe"))return SubscribeDisplay()!=0;
    if(argc==2&&!strcmp(argv[1],"--refresh")){SubscribeDisplay(false);return SubscribeDisplay()!=0;}
    if(argc==3&&!strcmp(argv[1],"--inject")){
        auto manager=HDI::ServiceManager::V1_0::IServiceManager::Get();
        auto remote=manager?manager->GetService("motion_interface_service"):nullptr;
        if(!remote)return 1;
        MessageParcel data,reply;MessageOption option;data.WriteInt32(atoi(argv[2]));
        int ret=remote->SendRequest(NABU_TEST_GRAVITY,data,reply,option);
        fprintf(stderr,"test gravity inject=%d\n",ret);return ret!=0;
    }
    int offset=0;
    FILE *parameter=popen("/system/bin/param get const.window.phyrotation.offset","r");
    if(parameter){int degrees=0;if(fscanf(parameter,"%d",&degrees)==1)offset=((degrees/90)%4+4)%4;pclose(parameter);}
    fprintf(stderr,"motion physical rotation offset=%d\n",offset*90);
    sptr<NabuMotion> impl=new NabuMotion(offset);active=impl.GetRefPtr();
    sptr<IRemoteObject> stub=new NabuMotionStub(impl);
    IPCSkeleton::SetMaxWorkThreadNum(4);
    int ret=RegisterService(stub);fprintf(stderr,"motion HDI register=%d\n",ret);
    if(ret)return 1;
    std::thread([impl]{
        /* DMS may have cached a successful subscription before real hardware was ready. */
        sleep(2);
        if(!impl->Enabled()){
            /* MSDP caches an absent Motion HDI during early boot. Register the
             * real provider first, then let MSDP recreate that connection. */
            fprintf(stderr,"motion reconnecting MSDP after HDI registration\n");
            system("/system/bin/param set ohos.ctl.stop msdp");
            usleep(200000);
            system("/system/bin/param set ohos.ctl.start msdp");
        }
        for(int attempt=0;attempt<6;attempt++){
            sleep(5);if(impl->Enabled())break;SubscribeDisplay(false);SubscribeDisplay();
        }
    }).detach();
    std::thread([]{
        auto *id=OH_Sensor_CreateSubscriptionId();
        auto *attribute=OH_Sensor_CreateSubscriptionAttribute();
        auto *subscriber=OH_Sensor_CreateSubscriber();
        OH_SensorSubscriptionId_SetType(id,SENSOR_TYPE_GRAVITY);
        OH_SensorSubscriptionAttribute_SetSamplingInterval(attribute,100000000);
        OH_SensorSubscriber_SetCallback(subscriber,SensorEvent);
        int ret;
        while((ret=OH_Sensor_Subscribe(id,attribute,subscriber))!=0){
            fprintf(stderr,"motion gravity subscribe=%d; retrying\n",ret);sleep(3);
        }
        fprintf(stderr,"motion gravity subscribe=0\n");
    }).detach();
    IPCSkeleton::JoinWorkThread();return 0;
}
