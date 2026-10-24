/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_HDI_SENSOR_V3_0_SENSORINTERFACEPROXY_H
#define OHOS_HDI_SENSOR_V3_0_SENSORINTERFACEPROXY_H

#include "v3_0/isensor_interface.h"
#include <unistd.h>
#include <iproxy_broker.h>

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {

class SensorInterfaceProxy : public IProxyBroker<OHOS::HDI::Sensor::V3_0::ISensorInterface> {
public:
    class IServiceManagerDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        IServiceManagerDeathRecipient(wptr<OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy> proxy) : proxy_(proxy) {} 
        ~IServiceManagerDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            int32_t result = HDF_FAILURE;
            const int sleepInterval = 500000;
            const int waitTimes = 10;
            int currentTime = waitTimes;
            do {
                usleep(sleepInterval);
                auto proxy = proxy_.promote();
                if (proxy != nullptr) {
                    result = OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Reconnect(proxy);
                }
                --currentTime;
            } while (result != HDF_SUCCESS && currentTime >0);
        }
    private:
        wptr<OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy> proxy_;
    };

    explicit SensorInterfaceProxy(const sptr<IRemoteObject>& remote) : IProxyBroker<OHOS::HDI::Sensor::V3_0::ISensorInterface>(remote) {
        reconnectRemote_ = nullptr;
        servMgr_ = nullptr;
        deathRecipient_ = nullptr;
        isReconnected_ = false;
    }
    virtual ~SensorInterfaceProxy() {
        if (servMgr_ != nullptr && deathRecipient_ != nullptr) {
            servMgr_->RemoveDeathRecipient(deathRecipient_);
        }
    }

    inline bool IsProxy() override
    {
        return true;
    }

    int32_t GetAllSensorInfo(std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info) override;

    int32_t GetDeviceSensorInfo(int32_t deviceId,
         std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info) override;

    int32_t Enable(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo) override;

    int32_t Disable(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo) override;

    int32_t SetBatch(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, int64_t samplingInterval,
         int64_t reportInterval) override;

    int32_t SetMode(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, int32_t mode) override;

    int32_t SetOption(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, uint32_t option) override;

    int32_t Register(int32_t groupId, const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj) override;

    int32_t Unregister(int32_t groupId, const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj) override;

    int32_t ReadData(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
         std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& event) override;

    int32_t SetSdcSensor(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, bool enabled,
         int32_t rateLevel) override;

    int32_t GetSdcSensorInfo(std::vector<OHOS::HDI::Sensor::V3_0::SdcSensorInfo>& sdcSensorInfo) override;

    int32_t RegisterAsync(int32_t groupId, const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj) override;

    int32_t UnregisterAsync(int32_t groupId,
         const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj) override;

    int32_t RegSensorPlugCallBack(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj) override;

    int32_t UnRegSensorPlugCallBack(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj) override;

    int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer) override;

    static int32_t GetAllSensorInfo_(std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info,
         const sptr<IRemoteObject> remote);

    static int32_t GetDeviceSensorInfo_(int32_t deviceId,
         std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info, const sptr<IRemoteObject> remote);

    static int32_t Enable_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
         const sptr<IRemoteObject> remote);

    static int32_t Disable_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
         const sptr<IRemoteObject> remote);

    static int32_t SetBatch_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
         int64_t samplingInterval, int64_t reportInterval, const sptr<IRemoteObject> remote);

    static int32_t SetMode_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, int32_t mode,
         const sptr<IRemoteObject> remote);

    static int32_t SetOption_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, uint32_t option,
         const sptr<IRemoteObject> remote);

    static int32_t Register_(int32_t groupId, const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj,
         const sptr<IRemoteObject> remote);

    static int32_t Unregister_(int32_t groupId, const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj,
         const sptr<IRemoteObject> remote);

    static int32_t ReadData_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
         std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& event, const sptr<IRemoteObject> remote);

    static int32_t SetSdcSensor_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, bool enabled,
         int32_t rateLevel, const sptr<IRemoteObject> remote);

    static int32_t GetSdcSensorInfo_(std::vector<OHOS::HDI::Sensor::V3_0::SdcSensorInfo>& sdcSensorInfo,
         const sptr<IRemoteObject> remote);

    static int32_t RegisterAsync_(int32_t groupId, const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj,
         const sptr<IRemoteObject> remote);

    static int32_t UnregisterAsync_(int32_t groupId, const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj,
         const sptr<IRemoteObject> remote);

    static int32_t RegSensorPlugCallBack_(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj,
         const sptr<IRemoteObject> remote);

    static int32_t UnRegSensorPlugCallBack_(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj,
         const sptr<IRemoteObject> remote);

    static int32_t GetVersion_(uint32_t& majorVer, uint32_t& minorVer, const sptr<IRemoteObject> remote);

    static int32_t Reconnect(sptr<OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy> proxy);

    sptr<IRemoteObject> GetCurrentRemote() {
        return isReconnected_ ? reconnectRemote_ : Remote();
    }

    bool isReconnected_;
    std::string serviceName_;
    sptr<IRemoteObject> servMgr_;
    sptr<OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::IServiceManagerDeathRecipient> deathRecipient_;
    sptr<IRemoteObject> reconnectRemote_;
private:
    static inline BrokerDelegator<OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy> delegator_;
};

} // V3_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V3_0_SENSORINTERFACEPROXY_H

