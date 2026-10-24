#include "isensor_interface_vdi.h"
#include "sensor_if.h"
#include <algorithm>
#include <mutex>

namespace OHOS::HDI::Sensor::V1_1 {
class NabuSensorVdi;
static NabuSensorVdi *active;
class NabuSensorVdi : public ISensorInterfaceVdi {
    const SensorInterface *hal_ = nullptr;
    std::mutex mutex_;
    std::vector<sptr<ISensorCallbackVdi>> callbacks_;
public:
    ~NabuSensorVdi() override { FreeSensorInterfaceInstance(); active=nullptr; }
    int32_t Init() override {
        active=this;hal_=NewSensorInterfaceInstance();
        return hal_?hal_->Register(0,Report):HDF_FAILURE;
    }
    int32_t GetAllSensorInfo(std::vector<HdfSensorInformationVdi> &out) override {
        if(!hal_&&Init())return HDF_FAILURE;
        SensorInformation *list=nullptr;int32_t count=0;
        int ret=hal_->GetAllSensors(&list,&count);if(ret)return ret;
        out.clear();
        for(int i=0;i<count;i++) {
            const auto &s=list[i];HdfSensorInformationVdi v={};
            v.sensorName=s.sensorName;v.vendorName=s.vendorName;
            v.firmwareVersion=s.firmwareVersion;v.hardwareVersion=s.hardwareVersion;
            v.sensorTypeId=s.sensorTypeId;v.sensorId=s.sensorId;
            v.sensorHandle={-1,s.sensorTypeId,0,1};
            v.maxRange=s.maxRange;v.accuracy=s.accuracy;v.power=s.power;
            v.minDelay=s.minDelay;v.maxDelay=s.maxDelay;out.push_back(v);
        }
        return HDF_SUCCESS;
    }
    int32_t GetDeviceSensorInfo(int32_t device,std::vector<HdfSensorInformationVdi>& out) override { return GetAllSensorInfo(out); }
    int32_t Enable(int32_t id) override {return hal_?hal_->Enable(id):HDF_FAILURE;}
    int32_t Disable(int32_t id) override {return hal_?hal_->Disable(id):HDF_FAILURE;}
    int32_t SetBatch(int32_t id,int64_t sampling,int64_t report) override {return hal_?hal_->SetBatch(id,sampling,report):HDF_FAILURE;}
    int32_t SetSaBatch(int32_t id,int64_t sampling,int64_t report) override {return SetBatch(id,sampling,report);}
    int32_t SetMode(int32_t id,int32_t mode) override {return hal_?hal_->SetMode(id,mode):HDF_FAILURE;}
    int32_t SetOption(int32_t id,uint32_t option) override {return hal_?hal_->SetOption(id,option):HDF_FAILURE;}
    int32_t Register(int32_t group,const sptr<ISensorCallbackVdi>& cb) override {
        if(group!=0||cb==nullptr)return HDF_ERR_INVALID_PARAM;
        std::lock_guard<std::mutex> lock(mutex_);
        if(std::find(callbacks_.begin(),callbacks_.end(),cb)==callbacks_.end())callbacks_.push_back(cb);
        return HDF_SUCCESS;
    }
    int32_t Unregister(int32_t group,const sptr<ISensorCallbackVdi>& cb) override {
        std::lock_guard<std::mutex> lock(mutex_);
        callbacks_.erase(std::remove(callbacks_.begin(),callbacks_.end(),cb),callbacks_.end());return HDF_SUCCESS;
    }
    static int32_t Report(const SensorEvents *e) {
        auto *self=active;if(!self)return HDF_FAILURE;
        HdfSensorEventsVdi v={};v.sensorId=e->sensorId;
        v.deviceSensorInfo={-1,e->sensorId,0,1};v.version=e->version;
        v.timestamp=e->timestamp;v.option=e->option;v.mode=e->mode;v.dataLen=e->dataLen;
        v.data.assign(e->data,e->data+e->dataLen);
        std::vector<sptr<ISensorCallbackVdi>> callbacks;
        {std::lock_guard<std::mutex> lock(self->mutex_);callbacks=self->callbacks_;}
        for(const auto& cb:callbacks)if(cb!=nullptr)cb->OnDataEventVdi(v);
        return HDF_SUCCESS;
    }
};
static int32_t Create(HdfVdiBase *base) {
    if(!base)return HDF_FAILURE;
    reinterpret_cast<WrapperSensorVdi*>(base)->sensorModule=new NabuSensorVdi();return HDF_SUCCESS;
}
static int32_t Destroy(HdfVdiBase *base) {
    if(!base)return HDF_FAILURE;
    auto *v=reinterpret_cast<WrapperSensorVdi*>(base);delete v->sensorModule;v->sensorModule=nullptr;return HDF_SUCCESS;
}
static WrapperSensorVdi descriptor={{1,"sensor_Service",Create,Destroy},nullptr};
extern "C" HDF_VDI_INIT(descriptor);
}
