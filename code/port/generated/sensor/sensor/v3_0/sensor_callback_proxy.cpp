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

#include "v3_0/sensor_callback_proxy.h"
#include <hdf_base.h>
#include <hdf_core_log.h>
#include <hdi_support.h>
#include <message_option.h>
#include <message_parcel.h>
#include <string_ex.h>

#define HDF_LOG_TAG    sensor_callback_proxy

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {


int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackProxy::OnDataEvent(const OHOS::HDI::Sensor::V3_0::HdfSensorEvents& event)
{
    return OHOS::HDI::Sensor::V3_0::SensorCallbackProxy::OnDataEvent_(event, Remote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackProxy::OnDataEventAsync(const std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& events)
{
    return OHOS::HDI::Sensor::V3_0::SensorCallbackProxy::OnDataEventAsync_(events, Remote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackProxy::GetVersion(uint32_t& majorVer, uint32_t& minorVer)
{
    return OHOS::HDI::Sensor::V3_0::SensorCallbackProxy::GetVersion_(majorVer, minorVer, Remote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackProxy::OnDataEvent_(const OHOS::HDI::Sensor::V3_0::HdfSensorEvents& event, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorCallbackData;
    MessageParcel sensorCallbackReply;
    MessageOption sensorCallbackOption(MessageOption::TF_SYNC);

    if (!sensorCallbackData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Sensor::V3_0::HdfSensorEventsBlockMarshalling(sensorCallbackData, event)) {
        HDF_LOGE("%{public}s: write event failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorCallbackRet = remote->SendRequest(CMD_SENSOR_CALLBACK_ON_DATA_EVENT, sensorCallbackData, sensorCallbackReply, sensorCallbackOption);
    if (sensorCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorCallbackRet);
        return sensorCallbackRet;
    }

    return sensorCallbackRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackProxy::OnDataEventAsync_(const std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& events, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorCallbackData;
    MessageParcel sensorCallbackReply;
    MessageOption sensorCallbackOption(MessageOption::TF_ASYNC);

    if (!sensorCallbackData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (events.size() > HDI_VECTOR_MAX_SIZE) {
        HDF_LOGE("%{public}s: events size exceeds limit", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!sensorCallbackData.WriteUint32(events.size())) {
        HDF_LOGE("%{public}s: write events size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    for (const auto& it0 : events) {
        if (!OHOS::HDI::Sensor::V3_0::HdfSensorEventsBlockMarshalling(sensorCallbackData, it0)) {
            HDF_LOGE("%{public}s: write it0 failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorCallbackRet = remote->SendRequest(CMD_SENSOR_CALLBACK_ON_DATA_EVENT_ASYNC, sensorCallbackData, sensorCallbackReply, sensorCallbackOption);
    if (sensorCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorCallbackRet);
        return sensorCallbackRet;
    }
    return sensorCallbackRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorCallbackProxy::GetVersion_(uint32_t& majorVer, uint32_t& minorVer,
     const sptr<IRemoteObject> remote)
{
    MessageParcel sensorCallbackData;
    MessageParcel sensorCallbackReply;
    MessageOption sensorCallbackOption(MessageOption::TF_SYNC);

    if (!sensorCallbackData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorCallbackRet = remote->SendRequest(CMD_SENSOR_CALLBACK_GET_VERSION, sensorCallbackData, sensorCallbackReply, sensorCallbackOption);
    if (sensorCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorCallbackRet);
        return sensorCallbackRet;
    }

    if (!sensorCallbackReply.ReadUint32(majorVer)) {
        HDF_LOGE("%{public}s: read majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorCallbackReply.ReadUint32(minorVer)) {
        HDF_LOGE("%{public}s: read minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return sensorCallbackRet;
}

} // V3_0
} // Sensor
} // HDI
} // OHOS
