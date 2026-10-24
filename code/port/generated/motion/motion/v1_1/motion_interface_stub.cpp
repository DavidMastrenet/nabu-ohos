/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "v1_1/motion_interface_stub.h"
#include <hdf_base.h>
#include <hdf_core_log.h>
#include <hdi_support.h>
#include <securec.h>
#include <string_ex.h>

#define HDF_LOG_TAG    motion_interface_stub

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_1 {
template<typename ElementType>
static bool ReadPodArray(MessageParcel &parcel, std::vector<ElementType> &data);


template<typename ElementType>
static bool ReadPodArray(MessageParcel &parcel, std::vector<ElementType> &data)
{
    data.clear();
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        HDF_LOGE("%{public}s: failed to read size", __func__);
        return false;
    }

    if (size == 0) {
        return true;
    }
    size_t sz;
    bool ok = __builtin_mul_overflow(sizeof(ElementType), size, &sz);
    if (ok) { return false; }
    const ElementType *dataPtr = reinterpret_cast<const ElementType*>(parcel.ReadUnpadBuffer(sz));
    if (dataPtr == nullptr) {
        HDF_LOGI("%{public}s: failed to read data", __func__);
        return false;
    }
    data.assign(dataPtr, dataPtr + size);
    return true;
}

sptr<OHOS::HDI::Motion::V1_1::IMotionInterface> OHOS::HDI::Motion::V1_1::IMotionInterface::Get(bool isStub)
{
    return OHOS::HDI::Motion::V1_1::IMotionInterface::Get("motion_interface_service", isStub);
}

sptr<OHOS::HDI::Motion::V1_1::IMotionInterface> OHOS::HDI::Motion::V1_1::IMotionInterface::Get(const std::string& serviceName, bool isStub)
{
    if (!isStub) {
        return nullptr;
    }
    std::string desc = Str16ToStr8(OHOS::HDI::Motion::V1_1::IMotionInterface::GetDescriptor());
    void *impl = LoadHdiImpl(desc.c_str(), serviceName == "motion_interface_service" ? "service" : serviceName.c_str());
    if (impl == nullptr) {
        HDF_LOGE("failed to load hdi impl %{public}s", desc.c_str());
        return nullptr;
    }
    return reinterpret_cast<OHOS::HDI::Motion::V1_1::IMotionInterface *>(impl);
}


OHOS::HDI::Motion::V1_1::MotionInterfaceStub::MotionInterfaceStub(const sptr<OHOS::HDI::Motion::V1_1::IMotionInterface> &impl)
    : IPCObjectStub(OHOS::HDI::Motion::V1_1::IMotionInterface::GetDescriptor()), impl_(impl)
{
}

OHOS::HDI::Motion::V1_1::MotionInterfaceStub::~MotionInterfaceStub()
{
    HDF_LOGI("%{public}s enter", __func__);
    ObjectCollector::GetInstance().RemoveObject(impl_);
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case CMD_MOTION_INTERFACE_GET_VERSION:
            return MotionInterfaceStubGetVersion(data, reply, option);
        case CMD_MOTION_INTERFACE_SET_MOTION_CONFIG:
            return MotionInterfaceStubSetMotionConfig(data, reply, option);
        case CMD_MOTION_INTERFACE_ENABLE_MOTION:
            return MotionInterfaceStubEnableMotion(data, reply, option);
        case CMD_MOTION_INTERFACE_DISABLE_MOTION:
            return MotionInterfaceStubDisableMotion(data, reply, option);
        case CMD_MOTION_INTERFACE_REGISTER:
            return MotionInterfaceStubRegister(data, reply, option);
        case CMD_MOTION_INTERFACE_UNREGISTER:
            return MotionInterfaceStubUnregister(data, reply, option);
        default: {
            HDF_LOGE("%{public}s: cmd %{public}d is not supported", __func__, code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceStub::MotionInterfaceStubSetMotionConfig(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption)
{
    return OHOS::HDI::Motion::V1_1::MotionInterfaceStub::MotionInterfaceStubSetMotionConfig_(motionInterfaceData, motionInterfaceReply, motionInterfaceOption, impl_);
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceStub::MotionInterfaceStubEnableMotion(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceStub::MotionInterfaceStubEnableMotion_(motionInterfaceData, motionInterfaceReply, motionInterfaceOption, impl_);
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceStub::MotionInterfaceStubDisableMotion(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceStub::MotionInterfaceStubDisableMotion_(motionInterfaceData, motionInterfaceReply, motionInterfaceOption, impl_);
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceStub::MotionInterfaceStubRegister(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceStub::MotionInterfaceStubRegister_(motionInterfaceData, motionInterfaceReply, motionInterfaceOption, impl_);
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceStub::MotionInterfaceStubUnregister(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceStub::MotionInterfaceStubUnregister_(motionInterfaceData, motionInterfaceReply, motionInterfaceOption, impl_);
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceStub::MotionInterfaceStubGetVersion(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceStub::MotionInterfaceStubGetVersion_(motionInterfaceData, motionInterfaceReply, motionInterfaceOption, impl_);
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceStub::MotionInterfaceStubSetMotionConfig_(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption, sptr<OHOS::HDI::Motion::V1_1::IMotionInterface> impl)
{
    if (motionInterfaceData.ReadInterfaceToken() != OHOS::HDI::Motion::V1_1::IMotionInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t motionType = 0;
    if (!motionInterfaceData.ReadInt32(motionType)) {
        HDF_LOGE("%{public}s: read motionType failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    std::vector<uint8_t> data;
    if (!ReadPodArray(motionInterfaceData, data)) {
        HDF_LOGE("%{public}s: failed to read data", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t motionInterfaceRet = impl->SetMotionConfig(motionType, data);
    if (motionInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, motionInterfaceRet);
        return motionInterfaceRet;
    }

    return motionInterfaceRet;
}
} // V1_1
} // Motion
} // HDI
} // OHOS
