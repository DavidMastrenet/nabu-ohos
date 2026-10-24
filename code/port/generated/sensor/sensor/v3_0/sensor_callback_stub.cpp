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

#include "v3_0/sensor_callback_stub.h"
#include <hdf_base.h>
#include <hdf_core_log.h>

#define HDF_LOG_TAG    sensor_callback_stub

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {



OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStub(const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> &impl)
    : IPCObjectStub(OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor()), impl_(impl)
{
}

OHOS::HDI::Sensor::V3_0::SensorCallbackStub::~SensorCallbackStub()
{
    HDF_LOGI("%{public}s enter", __func__);
    ObjectCollector::GetInstance().RemoveObject(impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case CMD_SENSOR_CALLBACK_GET_VERSION:
            return SensorCallbackStubGetVersion(data, reply, option);
        case CMD_SENSOR_CALLBACK_ON_DATA_EVENT:
            return SensorCallbackStubOnDataEvent(data, reply, option);
        case CMD_SENSOR_CALLBACK_ON_DATA_EVENT_ASYNC:
            return SensorCallbackStubOnDataEventAsync(data, reply, option);
        default: {
            HDF_LOGE("%{public}s: cmd %{public}d is not supported", __func__, code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStubOnDataEvent(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStubOnDataEvent_(sensorCallbackData, sensorCallbackReply, sensorCallbackOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStubOnDataEventAsync(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStubOnDataEventAsync_(sensorCallbackData, sensorCallbackReply, sensorCallbackOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStubGetVersion(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStubGetVersion_(sensorCallbackData, sensorCallbackReply, sensorCallbackOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStubOnDataEvent_(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> impl)
{
    if (sensorCallbackData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::HDI::Sensor::V3_0::HdfSensorEvents event;
    if (!HdfSensorEventsBlockUnmarshalling(sensorCallbackData, event)) {
        HDF_LOGE("%{public}s: read event failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorCallbackRet = impl->OnDataEvent(event);
    if (sensorCallbackRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorCallbackRet);
        return sensorCallbackRet;
    }

    return sensorCallbackRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStubOnDataEventAsync_(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> impl)
{
    if (sensorCallbackData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents> events;
    uint32_t eventsSize = 0;
    if (!sensorCallbackData.ReadUint32(eventsSize)) {
        HDF_LOGE("%{public}s: failed to read size", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (eventsSize > HDI_VECTOR_MAX_SIZE) {
        HDF_LOGE("%{public}s: events size exceeds limit", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDI_CHECK_VALUE_RETURN(eventsSize, >, HDI_BUFF_MAX_SIZE / sizeof(OHOS::HDI::Sensor::V3_0::HdfSensorEvents), HDF_ERR_INVALID_PARAM);
    events.clear();
    events.reserve(eventsSize);
    for (uint32_t i0 = 0; i0 < eventsSize; ++i0) {
        OHOS::HDI::Sensor::V3_0::HdfSensorEvents value0;
        if (!HdfSensorEventsBlockUnmarshalling(sensorCallbackData, value0)) {
            HDF_LOGE("%{public}s: read value0 failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        events.push_back(value0);
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorCallbackRet = impl->OnDataEventAsync(events);
    if (sensorCallbackRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorCallbackRet);
        return sensorCallbackRet;
    }

    return sensorCallbackRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackStub::SensorCallbackStubGetVersion_(MessageParcel& sensorCallbackData, MessageParcel& sensorCallbackReply, MessageOption& sensorCallbackOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> impl)
{
    if (sensorCallbackData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t majorVer = 0;

    uint32_t minorVer = 0;

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorCallbackRet = impl->GetVersion(majorVer, minorVer);
    if (sensorCallbackRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorCallbackRet);
        return sensorCallbackRet;
    }

    if (!sensorCallbackReply.WriteUint32(majorVer)) {
        HDF_LOGE("%{public}s: write majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorCallbackReply.WriteUint32(minorVer)) {
        HDF_LOGE("%{public}s: write minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return sensorCallbackRet;
}
} // V3_0
} // Sensor
} // HDI
} // OHOS
