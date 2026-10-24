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

#ifndef OHOS_HDI_SENSOR_V3_0_SENSORPLUGCALLBACKPROXY_H
#define OHOS_HDI_SENSOR_V3_0_SENSORPLUGCALLBACKPROXY_H

#include "v3_0/isensor_plug_callback.h"
#include <iproxy_broker.h>

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {

class SensorPlugCallbackProxy : public IProxyBroker<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback> {
public:
    explicit SensorPlugCallbackProxy(const sptr<IRemoteObject>& remote) : IProxyBroker<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>(remote) {
    }
    virtual ~SensorPlugCallbackProxy() = default;

    inline bool IsProxy() override
    {
        return true;
    }

    int32_t OnSensorPlugEvent(const OHOS::HDI::Sensor::V3_0::SensorPlugInfo& sensorPlugInfo) override;

    int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer) override;

    static int32_t OnSensorPlugEvent_(const OHOS::HDI::Sensor::V3_0::SensorPlugInfo& sensorPlugInfo,
         const sptr<IRemoteObject> remote);

    static int32_t GetVersion_(uint32_t& majorVer, uint32_t& minorVer, const sptr<IRemoteObject> remote);

private:
    static inline BrokerDelegator<OHOS::HDI::Sensor::V3_0::SensorPlugCallbackProxy> delegator_;
};

} // V3_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V3_0_SENSORPLUGCALLBACKPROXY_H

