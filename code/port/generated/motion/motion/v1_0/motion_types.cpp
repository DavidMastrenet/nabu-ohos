/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "v1_0/motion_types.h"
#include <hdf_core_log.h>
#include <message_parcel.h>
#include <securec.h>

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_0 {

template<typename ElementType>
static bool ReadPodArray(MessageParcel &parcel, std::vector<ElementType> &data);
template<typename ElementType>
static bool WritePodArray(MessageParcel &parcel, const std::vector<ElementType> &data);


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

template<typename ElementType>
static bool WritePodArray(MessageParcel &parcel, const std::vector<ElementType> &data)
{
    if (!parcel.WriteUint32(data.size())) {
        HDF_LOGE("%{public}s: failed to write data size", __func__);
        return false;
    }
    if (data.empty()) {
        return true;
    }
    size_t sz;
    bool ok = __builtin_mul_overflow(sizeof(ElementType), data.size(), &sz);
    if (ok) { return false; }
    if (!parcel.WriteUnpadBuffer((const void*)data.data(), sz)) {
        HDF_LOGE("%{public}s: failed to write array", __func__);
        return false;
    }
    return true;
}

bool HdfMotionEventBlockMarshalling(OHOS::MessageParcel& data, const OHOS::HDI::Motion::V1_0::HdfMotionEvent& dataBlock)
{
    if (!data.WriteInt32(dataBlock.motion)) {
        HDF_LOGE("%{public}s: write dataBlock.motion failed!", __func__);
        return false;
    }

    if (!data.WriteInt32(dataBlock.result)) {
        HDF_LOGE("%{public}s: write dataBlock.result failed!", __func__);
        return false;
    }

    if (!data.WriteInt32(dataBlock.status)) {
        HDF_LOGE("%{public}s: write dataBlock.status failed!", __func__);
        return false;
    }

    if (!data.WriteInt32(dataBlock.datalen)) {
        HDF_LOGE("%{public}s: write dataBlock.datalen failed!", __func__);
        return false;
    }

    if (!WritePodArray(data, dataBlock.data)) {
        HDF_LOGE("%{public}s: failed to write dataBlock.data", __func__);
        return false;
    }
    return true;
}

bool HdfMotionEventBlockUnmarshalling(OHOS::MessageParcel& data, OHOS::HDI::Motion::V1_0::HdfMotionEvent& dataBlock)
{
    if (!data.ReadInt32(dataBlock.motion)) {
        HDF_LOGE("%{public}s: read dataBlock.motion failed!", __func__);
        return false;
    }

    if (!data.ReadInt32(dataBlock.result)) {
        HDF_LOGE("%{public}s: read dataBlock.result failed!", __func__);
        return false;
    }

    if (!data.ReadInt32(dataBlock.status)) {
        HDF_LOGE("%{public}s: read dataBlock.status failed!", __func__);
        return false;
    }

    if (!data.ReadInt32(dataBlock.datalen)) {
        HDF_LOGE("%{public}s: read dataBlock.datalen failed!", __func__);
        return false;
    }

    if (!ReadPodArray(data, dataBlock.data)) {
        HDF_LOGE("%{public}s: failed to read dataBlock.data", __func__);
        return false;
    }
    return true;
}

} // V1_0
} // Motion
} // HDI
} // OHOS
