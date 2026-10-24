/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 1.1 (the "License");
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

#include "v3_0/sensor_types.h"
#include <hdf_core_log.h>
#include <message_parcel.h>
#include <securec.h>

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {

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

bool DeviceSensorInfoBlockMarshalling(OHOS::MessageParcel& data, const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& dataBlock)
{
    if (!data.WriteUnpadBuffer((const void*)&dataBlock, sizeof(OHOS::HDI::Sensor::V3_0::DeviceSensorInfo))) {
        return false;
    }
    return true;
}

bool DeviceSensorInfoBlockUnmarshalling(OHOS::MessageParcel& data, OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& dataBlock)
{
    const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo *dataBlockPtr = reinterpret_cast<const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo*>(data.ReadUnpadBuffer(sizeof(OHOS::HDI::Sensor::V3_0::DeviceSensorInfo)));
    if (dataBlockPtr == NULL) {
        return false;
    }

    if (memcpy_s(&dataBlock, sizeof(OHOS::HDI::Sensor::V3_0::DeviceSensorInfo), dataBlockPtr, sizeof(OHOS::HDI::Sensor::V3_0::DeviceSensorInfo)) != EOK) {
        return false;
    }
    return true;
}

bool HdfSensorInformationBlockMarshalling(OHOS::MessageParcel& data, const OHOS::HDI::Sensor::V3_0::HdfSensorInformation& dataBlock)
{
    if (!data.WriteCString(dataBlock.sensorName.c_str())) {
        HDF_LOGE("%{public}s: write dataBlock.sensorName failed!", __func__);
        return false;
    }

    if (!data.WriteCString(dataBlock.vendorName.c_str())) {
        HDF_LOGE("%{public}s: write dataBlock.vendorName failed!", __func__);
        return false;
    }

    if (!data.WriteCString(dataBlock.firmwareVersion.c_str())) {
        HDF_LOGE("%{public}s: write dataBlock.firmwareVersion failed!", __func__);
        return false;
    }

    if (!data.WriteCString(dataBlock.hardwareVersion.c_str())) {
        HDF_LOGE("%{public}s: write dataBlock.hardwareVersion failed!", __func__);
        return false;
    }

    if (!data.WriteFloat(dataBlock.maxRange)) {
        HDF_LOGE("%{public}s: write dataBlock.maxRange failed!", __func__);
        return false;
    }

    if (!data.WriteFloat(dataBlock.accuracy)) {
        HDF_LOGE("%{public}s: write dataBlock.accuracy failed!", __func__);
        return false;
    }

    if (!data.WriteFloat(dataBlock.power)) {
        HDF_LOGE("%{public}s: write dataBlock.power failed!", __func__);
        return false;
    }

    if (!data.WriteInt64(dataBlock.minDelay)) {
        HDF_LOGE("%{public}s: write dataBlock.minDelay failed!", __func__);
        return false;
    }

    if (!data.WriteInt64(dataBlock.maxDelay)) {
        HDF_LOGE("%{public}s: write dataBlock.maxDelay failed!", __func__);
        return false;
    }

    if (!data.WriteUint32(dataBlock.fifoMaxEventCount)) {
        HDF_LOGE("%{public}s: write dataBlock.fifoMaxEventCount failed!", __func__);
        return false;
    }

    if (!DeviceSensorInfoBlockMarshalling(data, dataBlock.deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write dataBlock.deviceSensorInfo failed!", __func__);
        return false;
    }

    if (!data.WriteUint32(dataBlock.reserved)) {
        HDF_LOGE("%{public}s: write dataBlock.reserved failed!", __func__);
        return false;
    }
    return true;
}

bool HdfSensorInformationBlockUnmarshalling(OHOS::MessageParcel& data, OHOS::HDI::Sensor::V3_0::HdfSensorInformation& dataBlock)
{
    const char* sensorNameCp = data.ReadCString();
    if (sensorNameCp == nullptr) {
        HDF_LOGE("%{public}s: read sensorNameCp failed", __func__);
        return false;
    }
    dataBlock.sensorName = sensorNameCp;

    const char* vendorNameCp = data.ReadCString();
    if (vendorNameCp == nullptr) {
        HDF_LOGE("%{public}s: read vendorNameCp failed", __func__);
        return false;
    }
    dataBlock.vendorName = vendorNameCp;

    const char* firmwareVersionCp = data.ReadCString();
    if (firmwareVersionCp == nullptr) {
        HDF_LOGE("%{public}s: read firmwareVersionCp failed", __func__);
        return false;
    }
    dataBlock.firmwareVersion = firmwareVersionCp;

    const char* hardwareVersionCp = data.ReadCString();
    if (hardwareVersionCp == nullptr) {
        HDF_LOGE("%{public}s: read hardwareVersionCp failed", __func__);
        return false;
    }
    dataBlock.hardwareVersion = hardwareVersionCp;

    if (!data.ReadFloat(dataBlock.maxRange)) {
        HDF_LOGE("%{public}s: read dataBlock.maxRange failed!", __func__);
        return false;
    }

    if (!data.ReadFloat(dataBlock.accuracy)) {
        HDF_LOGE("%{public}s: read dataBlock.accuracy failed!", __func__);
        return false;
    }

    if (!data.ReadFloat(dataBlock.power)) {
        HDF_LOGE("%{public}s: read dataBlock.power failed!", __func__);
        return false;
    }

    if (!data.ReadInt64(dataBlock.minDelay)) {
        HDF_LOGE("%{public}s: read dataBlock.minDelay failed!", __func__);
        return false;
    }

    if (!data.ReadInt64(dataBlock.maxDelay)) {
        HDF_LOGE("%{public}s: read dataBlock.maxDelay failed!", __func__);
        return false;
    }

    if (!data.ReadUint32(dataBlock.fifoMaxEventCount)) {
        HDF_LOGE("%{public}s: read dataBlock.fifoMaxEventCount failed!", __func__);
        return false;
    }

    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockUnmarshalling(data, dataBlock.deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read dataBlock.deviceSensorInfo failed!", __func__);
        return false;
    }

    if (!data.ReadUint32(dataBlock.reserved)) {
        HDF_LOGE("%{public}s: read dataBlock.reserved failed!", __func__);
        return false;
    }
    return true;
}

bool HdfSensorEventsBlockMarshalling(OHOS::MessageParcel& data, const OHOS::HDI::Sensor::V3_0::HdfSensorEvents& dataBlock)
{
    if (!DeviceSensorInfoBlockMarshalling(data, dataBlock.deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write dataBlock.deviceSensorInfo failed!", __func__);
        return false;
    }

    if (!data.WriteInt32(dataBlock.version)) {
        HDF_LOGE("%{public}s: write dataBlock.version failed!", __func__);
        return false;
    }

    if (!data.WriteInt64(dataBlock.timestamp)) {
        HDF_LOGE("%{public}s: write dataBlock.timestamp failed!", __func__);
        return false;
    }

    if (!data.WriteUint32(dataBlock.option)) {
        HDF_LOGE("%{public}s: write dataBlock.option failed!", __func__);
        return false;
    }

    if (!data.WriteInt32(dataBlock.mode)) {
        HDF_LOGE("%{public}s: write dataBlock.mode failed!", __func__);
        return false;
    }

    if (!WritePodArray(data, dataBlock.data)) {
        HDF_LOGE("%{public}s: failed to write dataBlock.data", __func__);
        return false;
    }

    if (!data.WriteUint32(dataBlock.dataLen)) {
        HDF_LOGE("%{public}s: write dataBlock.dataLen failed!", __func__);
        return false;
    }
    return true;
}

bool HdfSensorEventsBlockUnmarshalling(OHOS::MessageParcel& data, OHOS::HDI::Sensor::V3_0::HdfSensorEvents& dataBlock)
{
    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockUnmarshalling(data, dataBlock.deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read dataBlock.deviceSensorInfo failed!", __func__);
        return false;
    }

    if (!data.ReadInt32(dataBlock.version)) {
        HDF_LOGE("%{public}s: read dataBlock.version failed!", __func__);
        return false;
    }

    if (!data.ReadInt64(dataBlock.timestamp)) {
        HDF_LOGE("%{public}s: read dataBlock.timestamp failed!", __func__);
        return false;
    }

    if (!data.ReadUint32(dataBlock.option)) {
        HDF_LOGE("%{public}s: read dataBlock.option failed!", __func__);
        return false;
    }

    if (!data.ReadInt32(dataBlock.mode)) {
        HDF_LOGE("%{public}s: read dataBlock.mode failed!", __func__);
        return false;
    }

    if (!ReadPodArray(data, dataBlock.data)) {
        HDF_LOGE("%{public}s: failed to read dataBlock.data", __func__);
        return false;
    }

    if (!data.ReadUint32(dataBlock.dataLen)) {
        HDF_LOGE("%{public}s: read dataBlock.dataLen failed!", __func__);
        return false;
    }
    return true;
}

bool SdcSensorInfoBlockMarshalling(OHOS::MessageParcel& data, const OHOS::HDI::Sensor::V3_0::SdcSensorInfo& dataBlock)
{
    if (!data.WriteUnpadBuffer((const void*)&dataBlock, sizeof(OHOS::HDI::Sensor::V3_0::SdcSensorInfo))) {
        return false;
    }
    return true;
}

bool SdcSensorInfoBlockUnmarshalling(OHOS::MessageParcel& data, OHOS::HDI::Sensor::V3_0::SdcSensorInfo& dataBlock)
{
    const OHOS::HDI::Sensor::V3_0::SdcSensorInfo *dataBlockPtr = reinterpret_cast<const OHOS::HDI::Sensor::V3_0::SdcSensorInfo*>(data.ReadUnpadBuffer(sizeof(OHOS::HDI::Sensor::V3_0::SdcSensorInfo)));
    if (dataBlockPtr == NULL) {
        return false;
    }

    if (memcpy_s(&dataBlock, sizeof(OHOS::HDI::Sensor::V3_0::SdcSensorInfo), dataBlockPtr, sizeof(OHOS::HDI::Sensor::V3_0::SdcSensorInfo)) != EOK) {
        return false;
    }
    return true;
}

bool SensorPlugInfoBlockMarshalling(OHOS::MessageParcel& data, const OHOS::HDI::Sensor::V3_0::SensorPlugInfo& dataBlock)
{
    if (!DeviceSensorInfoBlockMarshalling(data, dataBlock.deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write dataBlock.deviceSensorInfo failed!", __func__);
        return false;
    }

    if (!data.WriteCString(dataBlock.deviceName.c_str())) {
        HDF_LOGE("%{public}s: write dataBlock.deviceName failed!", __func__);
        return false;
    }

    if (!data.WriteInt32(dataBlock.status)) {
        HDF_LOGE("%{public}s: write dataBlock.status failed!", __func__);
        return false;
    }

    if (!data.WriteInt32(dataBlock.reserved)) {
        HDF_LOGE("%{public}s: write dataBlock.reserved failed!", __func__);
        return false;
    }
    return true;
}

bool SensorPlugInfoBlockUnmarshalling(OHOS::MessageParcel& data, OHOS::HDI::Sensor::V3_0::SensorPlugInfo& dataBlock)
{
    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockUnmarshalling(data, dataBlock.deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read dataBlock.deviceSensorInfo failed!", __func__);
        return false;
    }

    const char* deviceNameCp = data.ReadCString();
    if (deviceNameCp == nullptr) {
        HDF_LOGE("%{public}s: read deviceNameCp failed", __func__);
        return false;
    }
    dataBlock.deviceName = deviceNameCp;

    if (!data.ReadInt32(dataBlock.status)) {
        HDF_LOGE("%{public}s: read dataBlock.status failed!", __func__);
        return false;
    }

    if (!data.ReadInt32(dataBlock.reserved)) {
        HDF_LOGE("%{public}s: read dataBlock.reserved failed!", __func__);
        return false;
    }
    return true;
}

} // V3_0
} // Sensor
} // HDI
} // OHOS
