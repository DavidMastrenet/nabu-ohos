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

#ifndef OHOS_HDI_SENSOR_V3_0_SENSORCALLBACKSTUB_H
#define OHOS_HDI_SENSOR_V3_0_SENSORCALLBACKSTUB_H

#include <ipc_object_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include <object_collector.h>
#include <refbase.h>
#include "v3_0/isensor_callback.h"

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {

using namespace OHOS;
class SensorCallbackStub : public IPCObjectStub {
public:
    explicit SensorCallbackStub(const sptr<ISensorCallback> &impl);
    virtual ~SensorCallbackStub();

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    static int32_t SensorCallbackStubOnDataEvent_(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> impl);

    static int32_t SensorCallbackStubOnDataEventAsync_(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> impl);

    static int32_t SensorCallbackStubGetVersion_(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> impl);

private:
    int32_t SensorCallbackStubOnDataEvent(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption);

    int32_t SensorCallbackStubOnDataEventAsync(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption);

    int32_t SensorCallbackStubGetVersion(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption);


    static inline ObjectDelegator<OHOS::HDI::Sensor::V3_0::SensorCallbackStub, OHOS::HDI::Sensor::V3_0::ISensorCallback> objDelegator_;
    sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> impl_;
};
} // V3_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V3_0_SENSORCALLBACKSTUB_H

