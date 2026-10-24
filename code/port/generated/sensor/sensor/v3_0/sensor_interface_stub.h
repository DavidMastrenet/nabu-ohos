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

#ifndef OHOS_HDI_SENSOR_V3_0_SENSORINTERFACESTUB_H
#define OHOS_HDI_SENSOR_V3_0_SENSORINTERFACESTUB_H

#include <ipc_object_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include <object_collector.h>
#include <refbase.h>
#include "v3_0/isensor_interface.h"

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {

using namespace OHOS;
class SensorInterfaceStub : public IPCObjectStub {
public:
    explicit SensorInterfaceStub(const sptr<ISensorInterface> &impl);
    virtual ~SensorInterfaceStub();

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    static int32_t SensorInterfaceStubGetAllSensorInfo_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubGetDeviceSensorInfo_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubEnable_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubDisable_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubSetBatch_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubSetMode_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubSetOption_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubRegister_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubUnregister_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubReadData_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubSetSdcSensor_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubGetSdcSensorInfo_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubRegisterAsync_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubUnregisterAsync_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubRegSensorPlugCallBack_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubUnRegSensorPlugCallBack_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

    static int32_t SensorInterfaceStubGetVersion_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl);

private:
    int32_t SensorInterfaceStubGetAllSensorInfo(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubGetDeviceSensorInfo(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubEnable(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubDisable(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubSetBatch(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubSetMode(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubSetOption(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubRegister(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubUnregister(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubReadData(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubSetSdcSensor(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubGetSdcSensorInfo(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubRegisterAsync(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubUnregisterAsync(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubRegSensorPlugCallBack(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubUnRegSensorPlugCallBack(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);

    int32_t SensorInterfaceStubGetVersion(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption);


    static inline ObjectDelegator<OHOS::HDI::Sensor::V3_0::SensorInterfaceStub, OHOS::HDI::Sensor::V3_0::ISensorInterface> objDelegator_;
    sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl_;
};
} // V3_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V3_0_SENSORINTERFACESTUB_H

