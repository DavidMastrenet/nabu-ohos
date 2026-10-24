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

#include "v3_0/sensor_plug_callback_proxy.h"
#include <hdf_base.h>
#include <hdf_core_log.h>
#include <hdi_support.h>
#include <message_option.h>
#include <message_parcel.h>
#include <string_ex.h>

#define HDF_LOG_TAG    sensor_plug_callback_proxy

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {


int32_t OHOS::HDI::Sensor::V3_0::SensorPlugCallbackProxy::OnSensorPlugEvent(const OHOS::HDI::Sensor::V3_0::SensorPlugInfo& sensorPlugInfo)
{
    return OHOS::HDI::Sensor::V3_0::SensorPlugCallbackProxy::OnSensorPlugEvent_(sensorPlugInfo, Remote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorPlugCallbackProxy::GetVersion(uint32_t& majorVer, uint32_t& minorVer)
{
    return OHOS::HDI::Sensor::V3_0::SensorPlugCallbackProxy::GetVersion_(majorVer, minorVer, Remote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorPlugCallbackProxy::OnSensorPlugEvent_(const OHOS::HDI::Sensor::V3_0::SensorPlugInfo& sensorPlugInfo, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorPlugCallbackData;
    MessageParcel sensorPlugCallbackReply;
    MessageOption sensorPlugCallbackOption(MessageOption::TF_SYNC);

    if (!sensorPlugCallbackData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorPlugCallback::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Sensor::V3_0::SensorPlugInfoBlockMarshalling(sensorPlugCallbackData, sensorPlugInfo)) {
        HDF_LOGE("%{public}s: write sensorPlugInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorPlugCallbackRet = remote->SendRequest(CMD_SENSOR_PLUG_CALLBACK_ON_SENSOR_PLUG_EVENT, sensorPlugCallbackData, sensorPlugCallbackReply, sensorPlugCallbackOption);
    if (sensorPlugCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorPlugCallbackRet);
        return sensorPlugCallbackRet;
    }

    return sensorPlugCallbackRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorPlugCallbackProxy::GetVersion_(uint32_t& majorVer, uint32_t& minorVer,
     const sptr<IRemoteObject> remote)
{
    MessageParcel sensorPlugCallbackData;
    MessageParcel sensorPlugCallbackReply;
    MessageOption sensorPlugCallbackOption(MessageOption::TF_SYNC);

    if (!sensorPlugCallbackData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorPlugCallback::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorPlugCallbackRet = remote->SendRequest(CMD_SENSOR_PLUG_CALLBACK_GET_VERSION, sensorPlugCallbackData, sensorPlugCallbackReply, sensorPlugCallbackOption);
    if (sensorPlugCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorPlugCallbackRet);
        return sensorPlugCallbackRet;
    }

    if (!sensorPlugCallbackReply.ReadUint32(majorVer)) {
        HDF_LOGE("%{public}s: read majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorPlugCallbackReply.ReadUint32(minorVer)) {
        HDF_LOGE("%{public}s: read minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return sensorPlugCallbackRet;
}

} // V3_0
} // Sensor
} // HDI
} // OHOS
