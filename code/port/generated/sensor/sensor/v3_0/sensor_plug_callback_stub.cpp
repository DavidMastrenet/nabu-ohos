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

#include "v3_0/sensor_plug_callback_stub.h"
#include <hdf_base.h>
#include <hdf_core_log.h>

#define HDF_LOG_TAG    sensor_plug_callback_stub

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {



OHOS::HDI::Sensor::V3_0::SensorPlugCallbackStub::SensorPlugCallbackStub(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback> &impl)
    : IPCObjectStub(OHOS::HDI::Sensor::V3_0::ISensorPlugCallback::GetDescriptor()), impl_(impl)
{
}

OHOS::HDI::Sensor::V3_0::SensorPlugCallbackStub::~SensorPlugCallbackStub()
{
    HDF_LOGI("%{public}s enter", __func__);
    ObjectCollector::GetInstance().RemoveObject(impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorPlugCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case CMD_SENSOR_PLUG_CALLBACK_GET_VERSION:
            return SensorPlugCallbackStubGetVersion(data, reply, option);
        case CMD_SENSOR_PLUG_CALLBACK_ON_SENSOR_PLUG_EVENT:
            return SensorPlugCallbackStubOnSensorPlugEvent(data, reply, option);
        default: {
            HDF_LOGE("%{public}s: cmd %{public}d is not supported", __func__, code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t OHOS::HDI::Sensor::V3_0::SensorPlugCallbackStub::SensorPlugCallbackStubOnSensorPlugEvent(MessageParcel& sensorPlugCallbackData, MessageParcel& sensorPlugCallbackReply, MessageOption& sensorPlugCallbackOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorPlugCallbackStub::SensorPlugCallbackStubOnSensorPlugEvent_(sensorPlugCallbackData, sensorPlugCallbackReply, sensorPlugCallbackOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorPlugCallbackStub::SensorPlugCallbackStubGetVersion(MessageParcel& sensorPlugCallbackData, MessageParcel& sensorPlugCallbackReply, MessageOption& sensorPlugCallbackOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorPlugCallbackStub::SensorPlugCallbackStubGetVersion_(sensorPlugCallbackData, sensorPlugCallbackReply, sensorPlugCallbackOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorPlugCallbackStub::SensorPlugCallbackStubOnSensorPlugEvent_(MessageParcel& sensorPlugCallbackData, MessageParcel& sensorPlugCallbackReply, MessageOption& sensorPlugCallbackOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback> impl)
{
    if (sensorPlugCallbackData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorPlugCallback::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::HDI::Sensor::V3_0::SensorPlugInfo sensorPlugInfo;
    if (!SensorPlugInfoBlockUnmarshalling(sensorPlugCallbackData, sensorPlugInfo)) {
        HDF_LOGE("%{public}s: read sensorPlugInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorPlugCallbackRet = impl->OnSensorPlugEvent(sensorPlugInfo);
    if (sensorPlugCallbackRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorPlugCallbackRet);
        return sensorPlugCallbackRet;
    }

    return sensorPlugCallbackRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorPlugCallbackStub::SensorPlugCallbackStubGetVersion_(MessageParcel& sensorPlugCallbackData, MessageParcel& sensorPlugCallbackReply, MessageOption& sensorPlugCallbackOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback> impl)
{
    if (sensorPlugCallbackData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorPlugCallback::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t majorVer = 0;

    uint32_t minorVer = 0;

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorPlugCallbackRet = impl->GetVersion(majorVer, minorVer);
    if (sensorPlugCallbackRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorPlugCallbackRet);
        return sensorPlugCallbackRet;
    }

    if (!sensorPlugCallbackReply.WriteUint32(majorVer)) {
        HDF_LOGE("%{public}s: write majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorPlugCallbackReply.WriteUint32(minorVer)) {
        HDF_LOGE("%{public}s: write minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return sensorPlugCallbackRet;
}
} // V3_0
} // Sensor
} // HDI
} // OHOS
