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

#ifndef OHOS_HDI_SENSOR_V3_0_SENSORPLUGCALLBACKSERVICE_H
#define OHOS_HDI_SENSOR_V3_0_SENSORPLUGCALLBACKSERVICE_H

#include "v3_0/isensor_plug_callback.h"

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {
class SensorPlugCallbackService : public OHOS::HDI::Sensor::V3_0::ISensorPlugCallback {
public:
    SensorPlugCallbackService() = default;
    virtual ~SensorPlugCallbackService() = default;

    int32_t OnSensorPlugEvent(const OHOS::HDI::Sensor::V3_0::SensorPlugInfo& sensorPlugInfo) override;

};
} // V3_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V3_0_SENSORPLUGCALLBACKSERVICE_H

