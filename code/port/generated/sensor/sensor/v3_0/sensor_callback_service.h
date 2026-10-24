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

#ifndef OHOS_HDI_SENSOR_V3_0_SENSORCALLBACKSERVICE_H
#define OHOS_HDI_SENSOR_V3_0_SENSORCALLBACKSERVICE_H

#include "v3_0/isensor_callback.h"

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {
class SensorCallbackService : public OHOS::HDI::Sensor::V3_0::ISensorCallback {
public:
    SensorCallbackService() = default;
    virtual ~SensorCallbackService() = default;

    int32_t OnDataEvent(const OHOS::HDI::Sensor::V3_0::HdfSensorEvents& event) override;

    int32_t OnDataEventAsync(const std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& events) override;

};
} // V3_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V3_0_SENSORCALLBACKSERVICE_H

