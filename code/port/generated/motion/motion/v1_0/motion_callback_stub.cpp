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

#include "v1_0/motion_callback_stub.h"
#include <hdf_base.h>
#include <hdf_core_log.h>

#define HDF_LOG_TAG    motion_callback_stub

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_0 {



OHOS::HDI::Motion::V1_0::MotionCallbackStub::MotionCallbackStub(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback> &impl)
    : IPCObjectStub(OHOS::HDI::Motion::V1_0::IMotionCallback::GetDescriptor()), impl_(impl)
{
}

OHOS::HDI::Motion::V1_0::MotionCallbackStub::~MotionCallbackStub()
{
    HDF_LOGI("%{public}s enter", __func__);
    ObjectCollector::GetInstance().RemoveObject(impl_);
}

int32_t OHOS::HDI::Motion::V1_0::MotionCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case CMD_MOTION_CALLBACK_GET_VERSION:
            return MotionCallbackStubGetVersion(data, reply, option);
        case CMD_MOTION_CALLBACK_ON_DATA_EVENT:
            return MotionCallbackStubOnDataEvent(data, reply, option);
        default: {
            HDF_LOGE("%{public}s: cmd %{public}d is not supported", __func__, code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t OHOS::HDI::Motion::V1_0::MotionCallbackStub::MotionCallbackStubOnDataEvent(MessageParcel& motionCallbackData, MessageParcel& motionCallbackReply, MessageOption& motionCallbackOption)
{
    return OHOS::HDI::Motion::V1_0::MotionCallbackStub::MotionCallbackStubOnDataEvent_(motionCallbackData, motionCallbackReply, motionCallbackOption, impl_);
}

int32_t OHOS::HDI::Motion::V1_0::MotionCallbackStub::MotionCallbackStubGetVersion(MessageParcel& motionCallbackData, MessageParcel& motionCallbackReply, MessageOption& motionCallbackOption)
{
    return OHOS::HDI::Motion::V1_0::MotionCallbackStub::MotionCallbackStubGetVersion_(motionCallbackData, motionCallbackReply, motionCallbackOption, impl_);
}

int32_t OHOS::HDI::Motion::V1_0::MotionCallbackStub::MotionCallbackStubOnDataEvent_(MessageParcel& motionCallbackData, MessageParcel& motionCallbackReply, MessageOption& motionCallbackOption, sptr<OHOS::HDI::Motion::V1_0::IMotionCallback> impl)
{
    if (motionCallbackData.ReadInterfaceToken() != OHOS::HDI::Motion::V1_0::IMotionCallback::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::HDI::Motion::V1_0::HdfMotionEvent event;
    if (!HdfMotionEventBlockUnmarshalling(motionCallbackData, event)) {
        HDF_LOGE("%{public}s: read event failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t motionCallbackRet = impl->OnDataEvent(event);
    if (motionCallbackRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, motionCallbackRet);
        return motionCallbackRet;
    }

    return motionCallbackRet;
}

int32_t OHOS::HDI::Motion::V1_0::MotionCallbackStub::MotionCallbackStubGetVersion_(MessageParcel& motionCallbackData, MessageParcel& motionCallbackReply, MessageOption& motionCallbackOption, sptr<OHOS::HDI::Motion::V1_0::IMotionCallback> impl)
{
    if (motionCallbackData.ReadInterfaceToken() != OHOS::HDI::Motion::V1_0::IMotionCallback::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t majorVer = 0;

    uint32_t minorVer = 0;

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t motionCallbackRet = impl->GetVersion(majorVer, minorVer);
    if (motionCallbackRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, motionCallbackRet);
        return motionCallbackRet;
    }

    if (!motionCallbackReply.WriteUint32(majorVer)) {
        HDF_LOGE("%{public}s: write majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!motionCallbackReply.WriteUint32(minorVer)) {
        HDF_LOGE("%{public}s: write minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return motionCallbackRet;
}
} // V1_0
} // Motion
} // HDI
} // OHOS
