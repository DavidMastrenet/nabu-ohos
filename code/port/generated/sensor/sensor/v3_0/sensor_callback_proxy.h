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

#ifndef OHOS_HDI_SENSOR_V3_0_SENSORCALLBACKPROXY_H
#define OHOS_HDI_SENSOR_V3_0_SENSORCALLBACKPROXY_H

#include "v3_0/isensor_callback.h"
#include <iproxy_broker.h>

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {

class SensorCallbackProxy : public IProxyBroker<OHOS::HDI::Sensor::V3_0::ISensorCallback> {
public:
    explicit SensorCallbackProxy(const sptr<IRemoteObject>& remote) : IProxyBroker<OHOS::HDI::Sensor::V3_0::ISensorCallback>(remote) {
    }
    virtual ~SensorCallbackProxy() = default;

    inline bool IsProxy() override
    {
        return true;
    }

    int32_t OnDataEvent(const OHOS::HDI::Sensor::V3_0::HdfSensorEvents& event) override;

    int32_t OnDataEventAsync(const std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& events) override;

    int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer) override;

    static int32_t OnDataEvent_(const OHOS::HDI::Sensor::V3_0::HdfSensorEvents& event,
         const sptr<IRemoteObject> remote);

    static int32_t OnDataEventAsync_(const std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& events,
         const sptr<IRemoteObject> remote);

    static int32_t GetVersion_(uint32_t& majorVer, uint32_t& minorVer, const sptr<IRemoteObject> remote);

private:
    static inline BrokerDelegator<OHOS::HDI::Sensor::V3_0::SensorCallbackProxy> delegator_;
};

} // V3_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V3_0_SENSORCALLBACKPROXY_H

