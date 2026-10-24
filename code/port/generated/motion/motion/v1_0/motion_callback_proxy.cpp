/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "v1_0/motion_callback_proxy.h"
#include <hdf_base.h>
#include <hdf_core_log.h>
#include <hdi_support.h>
#include <message_option.h>
#include <message_parcel.h>
#include <string_ex.h>

#define HDF_LOG_TAG    motion_callback_proxy

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_0 {


int32_t OHOS::HDI::Motion::V1_0::MotionCallbackProxy::OnDataEvent(const OHOS::HDI::Motion::V1_0::HdfMotionEvent& event)
{
    return OHOS::HDI::Motion::V1_0::MotionCallbackProxy::OnDataEvent_(event, Remote());
}

int32_t OHOS::HDI::Motion::V1_0::MotionCallbackProxy::GetVersion(uint32_t& majorVer, uint32_t& minorVer)
{
    return OHOS::HDI::Motion::V1_0::MotionCallbackProxy::GetVersion_(majorVer, minorVer, Remote());
}

int32_t OHOS::HDI::Motion::V1_0::MotionCallbackProxy::OnDataEvent_(const OHOS::HDI::Motion::V1_0::HdfMotionEvent& event,
     const sptr<IRemoteObject> remote)
{
    MessageParcel motionCallbackData;
    MessageParcel motionCallbackReply;
    MessageOption motionCallbackOption(MessageOption::TF_SYNC);

    if (!motionCallbackData.WriteInterfaceToken(OHOS::HDI::Motion::V1_0::IMotionCallback::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Motion::V1_0::HdfMotionEventBlockMarshalling(motionCallbackData, event)) {
        HDF_LOGE("%{public}s: write event failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t motionCallbackRet = remote->SendRequest(CMD_MOTION_CALLBACK_ON_DATA_EVENT, motionCallbackData, motionCallbackReply, motionCallbackOption);
    if (motionCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, motionCallbackRet);
        return motionCallbackRet;
    }

    return motionCallbackRet;
}

int32_t OHOS::HDI::Motion::V1_0::MotionCallbackProxy::GetVersion_(uint32_t& majorVer, uint32_t& minorVer,
     const sptr<IRemoteObject> remote)
{
    MessageParcel motionCallbackData;
    MessageParcel motionCallbackReply;
    MessageOption motionCallbackOption(MessageOption::TF_SYNC);

    if (!motionCallbackData.WriteInterfaceToken(OHOS::HDI::Motion::V1_0::IMotionCallback::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t motionCallbackRet = remote->SendRequest(CMD_MOTION_CALLBACK_GET_VERSION, motionCallbackData, motionCallbackReply, motionCallbackOption);
    if (motionCallbackRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, motionCallbackRet);
        return motionCallbackRet;
    }

    if (!motionCallbackReply.ReadUint32(majorVer)) {
        HDF_LOGE("%{public}s: read majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!motionCallbackReply.ReadUint32(minorVer)) {
        HDF_LOGE("%{public}s: read minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return motionCallbackRet;
}

} // V1_0
} // Motion
} // HDI
} // OHOS
