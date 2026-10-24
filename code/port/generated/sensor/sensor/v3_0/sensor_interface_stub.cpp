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

#include "v3_0/sensor_interface_stub.h"
#include <hdf_base.h>
#include <hdf_core_log.h>
#include <hdi_support.h>
#include <iproxy_broker.h>
#include <securec.h>
#include <string_ex.h>
#include "sensor\v3_0\sensor_callback_proxy.h"
#include "sensor\v3_0\sensor_plug_callback_proxy.h"

#define HDF_LOG_TAG    sensor_interface_stub

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {
template<typename ElementType>
static bool WritePodArray(MessageParcel &parcel, const std::vector<ElementType> &data);


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

sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> OHOS::HDI::Sensor::V3_0::ISensorInterface::Get(bool isStub)
{
    return OHOS::HDI::Sensor::V3_0::ISensorInterface::Get("sensor_interface_service", isStub);
}

sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> OHOS::HDI::Sensor::V3_0::ISensorInterface::Get(const std::string& serviceName, bool isStub)
{
    if (!isStub) {
        return nullptr;
    }
    std::string desc = Str16ToStr8(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor());
    void *impl = LoadHdiImpl(desc.c_str(), serviceName == "sensor_interface_service" ? "service" : serviceName.c_str());
    if (impl == nullptr) {
        HDF_LOGE("failed to load hdi impl %{public}s", desc.c_str());
        return nullptr;
    }
    return reinterpret_cast<OHOS::HDI::Sensor::V3_0::ISensorInterface *>(impl);
}


OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStub(const sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> &impl)
    : IPCObjectStub(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()), impl_(impl)
{
}

OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::~SensorInterfaceStub()
{
    HDF_LOGI("%{public}s enter", __func__);
    ObjectCollector::GetInstance().RemoveObject(impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case CMD_SENSOR_INTERFACE_GET_VERSION:
            return SensorInterfaceStubGetVersion(data, reply, option);
        case CMD_SENSOR_INTERFACE_GET_ALL_SENSOR_INFO:
            return SensorInterfaceStubGetAllSensorInfo(data, reply, option);
        case CMD_SENSOR_INTERFACE_GET_DEVICE_SENSOR_INFO:
            return SensorInterfaceStubGetDeviceSensorInfo(data, reply, option);
        case CMD_SENSOR_INTERFACE_ENABLE:
            return SensorInterfaceStubEnable(data, reply, option);
        case CMD_SENSOR_INTERFACE_DISABLE:
            return SensorInterfaceStubDisable(data, reply, option);
        case CMD_SENSOR_INTERFACE_SET_BATCH:
            return SensorInterfaceStubSetBatch(data, reply, option);
        case CMD_SENSOR_INTERFACE_SET_MODE:
            return SensorInterfaceStubSetMode(data, reply, option);
        case CMD_SENSOR_INTERFACE_SET_OPTION:
            return SensorInterfaceStubSetOption(data, reply, option);
        case CMD_SENSOR_INTERFACE_REGISTER:
            return SensorInterfaceStubRegister(data, reply, option);
        case CMD_SENSOR_INTERFACE_UNREGISTER:
            return SensorInterfaceStubUnregister(data, reply, option);
        case CMD_SENSOR_INTERFACE_READ_DATA:
            return SensorInterfaceStubReadData(data, reply, option);
        case CMD_SENSOR_INTERFACE_SET_SDC_SENSOR:
            return SensorInterfaceStubSetSdcSensor(data, reply, option);
        case CMD_SENSOR_INTERFACE_GET_SDC_SENSOR_INFO:
            return SensorInterfaceStubGetSdcSensorInfo(data, reply, option);
        case CMD_SENSOR_INTERFACE_REGISTER_ASYNC:
            return SensorInterfaceStubRegisterAsync(data, reply, option);
        case CMD_SENSOR_INTERFACE_UNREGISTER_ASYNC:
            return SensorInterfaceStubUnregisterAsync(data, reply, option);
        case CMD_SENSOR_INTERFACE_REG_SENSOR_PLUG_CALL_BACK:
            return SensorInterfaceStubRegSensorPlugCallBack(data, reply, option);
        case CMD_SENSOR_INTERFACE_UN_REG_SENSOR_PLUG_CALL_BACK:
            return SensorInterfaceStubUnRegSensorPlugCallBack(data, reply, option);
        default: {
            HDF_LOGE("%{public}s: cmd %{public}d is not supported", __func__, code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetAllSensorInfo(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetAllSensorInfo_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetDeviceSensorInfo(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetDeviceSensorInfo_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubEnable(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubEnable_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubDisable(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubDisable_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetBatch(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetBatch_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetMode(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetMode_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetOption(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetOption_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubRegister(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubRegister_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubUnregister(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubUnregister_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubReadData(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubReadData_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetSdcSensor(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetSdcSensor_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetSdcSensorInfo(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetSdcSensorInfo_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubRegisterAsync(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubRegisterAsync_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubUnregisterAsync(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubUnregisterAsync_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubRegSensorPlugCallBack(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubRegSensorPlugCallBack_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubUnRegSensorPlugCallBack(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubUnRegSensorPlugCallBack_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetVersion(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetVersion_(sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption, impl_);
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetAllSensorInfo_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    bool sensorInterfaceMemSet = false;
    if (!sensorInterfaceData.ReadBool(sensorInterfaceMemSet)) {
        HDF_LOGE("%{public}s: failed to read sensorInterfaceMemSet", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation> info;
    if (sensorInterfaceMemSet) {
        uint32_t capacity = 0;
        if (!sensorInterfaceData.ReadUint32(capacity)) {
            HDF_LOGE("%{public}s: failed to read capacity", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        HDI_CHECK_VALUE_RETURN(capacity, >, HDI_BUFF_MAX_SIZE / sizeof(OHOS::HDI::Sensor::V3_0::HdfSensorInformation), HDF_ERR_INVALID_PARAM);
        info.reserve(capacity);
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->GetAllSensorInfo(info);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    if (info.size() > HDI_VECTOR_MAX_SIZE) {
        HDF_LOGE("%{public}s: info size exceeds limit", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!sensorInterfaceReply.WriteUint32(info.size())) {
        HDF_LOGE("%{public}s: write info size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    for (const auto& it0 : info) {
        if (!OHOS::HDI::Sensor::V3_0::HdfSensorInformationBlockMarshalling(sensorInterfaceReply, it0)) {
            HDF_LOGE("%{public}s: write it0 failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetDeviceSensorInfo_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    bool sensorInterfaceMemSet = false;
    if (!sensorInterfaceData.ReadBool(sensorInterfaceMemSet)) {
        HDF_LOGE("%{public}s: failed to read sensorInterfaceMemSet", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t deviceId = 0;
    if (!sensorInterfaceData.ReadInt32(deviceId)) {
        HDF_LOGE("%{public}s: read deviceId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation> info;
    if (sensorInterfaceMemSet) {
        uint32_t capacity = 0;
        if (!sensorInterfaceData.ReadUint32(capacity)) {
            HDF_LOGE("%{public}s: failed to read capacity", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        HDI_CHECK_VALUE_RETURN(capacity, >, HDI_BUFF_MAX_SIZE / sizeof(OHOS::HDI::Sensor::V3_0::HdfSensorInformation), HDF_ERR_INVALID_PARAM);
        info.reserve(capacity);
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->GetDeviceSensorInfo(deviceId, info);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    if (info.size() > HDI_VECTOR_MAX_SIZE) {
        HDF_LOGE("%{public}s: info size exceeds limit", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!sensorInterfaceReply.WriteUint32(info.size())) {
        HDF_LOGE("%{public}s: write info size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    for (const auto& it0 : info) {
        if (!OHOS::HDI::Sensor::V3_0::HdfSensorInformationBlockMarshalling(sensorInterfaceReply, it0)) {
            HDF_LOGE("%{public}s: write it0 failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubEnable_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    if (!DeviceSensorInfoBlockUnmarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->Enable(deviceSensorInfo);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubDisable_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    if (!DeviceSensorInfoBlockUnmarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->Disable(deviceSensorInfo);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetBatch_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    if (!DeviceSensorInfoBlockUnmarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int64_t samplingInterval = 0;
    if (!sensorInterfaceData.ReadInt64(samplingInterval)) {
        HDF_LOGE("%{public}s: read samplingInterval failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int64_t reportInterval = 0;
    if (!sensorInterfaceData.ReadInt64(reportInterval)) {
        HDF_LOGE("%{public}s: read reportInterval failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->SetBatch(deviceSensorInfo, samplingInterval, reportInterval);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetMode_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    if (!DeviceSensorInfoBlockUnmarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t mode = 0;
    if (!sensorInterfaceData.ReadInt32(mode)) {
        HDF_LOGE("%{public}s: read mode failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->SetMode(deviceSensorInfo, mode);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetOption_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    if (!DeviceSensorInfoBlockUnmarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t option = 0;
    if (!sensorInterfaceData.ReadUint32(option)) {
        HDF_LOGE("%{public}s: read option failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->SetOption(deviceSensorInfo, option);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubRegister_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t groupId = 0;
    if (!sensorInterfaceData.ReadInt32(groupId)) {
        HDF_LOGE("%{public}s: read groupId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> callbackObj;
    sptr<IRemoteObject> callbackObjRemote = sensorInterfaceData.ReadRemoteObject();
    if (callbackObjRemote == nullptr) {
        HDF_LOGE("%{public}s: read an invalid remote object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    callbackObj = new OHOS::HDI::Sensor::V3_0::SensorCallbackProxy(callbackObjRemote);
    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: failed to create interface object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->Register(groupId, callbackObj);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubUnregister_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t groupId = 0;
    if (!sensorInterfaceData.ReadInt32(groupId)) {
        HDF_LOGE("%{public}s: read groupId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> callbackObj;
    sptr<IRemoteObject> callbackObjRemote = sensorInterfaceData.ReadRemoteObject();
    if (callbackObjRemote == nullptr) {
        HDF_LOGE("%{public}s: read an invalid remote object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    callbackObj = new OHOS::HDI::Sensor::V3_0::SensorCallbackProxy(callbackObjRemote);
    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: failed to create interface object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->Unregister(groupId, callbackObj);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubReadData_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    bool sensorInterfaceMemSet = false;
    if (!sensorInterfaceData.ReadBool(sensorInterfaceMemSet)) {
        HDF_LOGE("%{public}s: failed to read sensorInterfaceMemSet", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    if (!DeviceSensorInfoBlockUnmarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents> event;
    if (sensorInterfaceMemSet) {
        uint32_t capacity = 0;
        if (!sensorInterfaceData.ReadUint32(capacity)) {
            HDF_LOGE("%{public}s: failed to read capacity", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        HDI_CHECK_VALUE_RETURN(capacity, >, HDI_BUFF_MAX_SIZE / sizeof(OHOS::HDI::Sensor::V3_0::HdfSensorEvents), HDF_ERR_INVALID_PARAM);
        event.reserve(capacity);
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->ReadData(deviceSensorInfo, event);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    if (event.size() > HDI_VECTOR_MAX_SIZE) {
        HDF_LOGE("%{public}s: event size exceeds limit", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!sensorInterfaceReply.WriteUint32(event.size())) {
        HDF_LOGE("%{public}s: write event size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    for (const auto& it0 : event) {
        if (!OHOS::HDI::Sensor::V3_0::HdfSensorEventsBlockMarshalling(sensorInterfaceReply, it0)) {
            HDF_LOGE("%{public}s: write it0 failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubSetSdcSensor_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    if (!DeviceSensorInfoBlockUnmarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: read deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    bool enabled = false;
    if (!sensorInterfaceData.ReadBool(enabled)) {
        HDF_LOGE("%{public}s: read enabled failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t rateLevel = 0;
    if (!sensorInterfaceData.ReadInt32(rateLevel)) {
        HDF_LOGE("%{public}s: read rateLevel failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->SetSdcSensor(deviceSensorInfo, enabled, rateLevel);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetSdcSensorInfo_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    bool sensorInterfaceMemSet = false;
    if (!sensorInterfaceData.ReadBool(sensorInterfaceMemSet)) {
        HDF_LOGE("%{public}s: failed to read sensorInterfaceMemSet", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    std::vector<OHOS::HDI::Sensor::V3_0::SdcSensorInfo> sdcSensorInfo;
    if (sensorInterfaceMemSet) {
        uint32_t capacity = 0;
        if (!sensorInterfaceData.ReadUint32(capacity)) {
            HDF_LOGE("%{public}s: failed to read capacity", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        HDI_CHECK_VALUE_RETURN(capacity, >, HDI_BUFF_MAX_SIZE / sizeof(OHOS::HDI::Sensor::V3_0::SdcSensorInfo), HDF_ERR_INVALID_PARAM);
        sdcSensorInfo.reserve(capacity);
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->GetSdcSensorInfo(sdcSensorInfo);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    if (!WritePodArray(sensorInterfaceReply, sdcSensorInfo)) {
        HDF_LOGE("%{public}s: failed to write sdcSensorInfo", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubRegisterAsync_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t groupId = 0;
    if (!sensorInterfaceData.ReadInt32(groupId)) {
        HDF_LOGE("%{public}s: read groupId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> callbackObj;
    sptr<IRemoteObject> callbackObjRemote = sensorInterfaceData.ReadRemoteObject();
    if (callbackObjRemote == nullptr) {
        HDF_LOGE("%{public}s: read an invalid remote object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    callbackObj = new OHOS::HDI::Sensor::V3_0::SensorCallbackProxy(callbackObjRemote);
    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: failed to create interface object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->RegisterAsync(groupId, callbackObj);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubUnregisterAsync_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t groupId = 0;
    if (!sensorInterfaceData.ReadInt32(groupId)) {
        HDF_LOGE("%{public}s: read groupId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback> callbackObj;
    sptr<IRemoteObject> callbackObjRemote = sensorInterfaceData.ReadRemoteObject();
    if (callbackObjRemote == nullptr) {
        HDF_LOGE("%{public}s: read an invalid remote object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    callbackObj = new OHOS::HDI::Sensor::V3_0::SensorCallbackProxy(callbackObjRemote);
    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: failed to create interface object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->UnregisterAsync(groupId, callbackObj);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubRegSensorPlugCallBack_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback> callbackObj;
    sptr<IRemoteObject> callbackObjRemote = sensorInterfaceData.ReadRemoteObject();
    if (callbackObjRemote == nullptr) {
        HDF_LOGE("%{public}s: read an invalid remote object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    callbackObj = new OHOS::HDI::Sensor::V3_0::SensorPlugCallbackProxy(callbackObjRemote);
    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: failed to create interface object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->RegSensorPlugCallBack(callbackObj);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubUnRegSensorPlugCallBack_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback> callbackObj;
    sptr<IRemoteObject> callbackObjRemote = sensorInterfaceData.ReadRemoteObject();
    if (callbackObjRemote == nullptr) {
        HDF_LOGE("%{public}s: read an invalid remote object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    callbackObj = new OHOS::HDI::Sensor::V3_0::SensorPlugCallbackProxy(callbackObjRemote);
    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: failed to create interface object", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->UnRegSensorPlugCallBack(callbackObj);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceStub::SensorInterfaceStubGetVersion_(MessageParcel& sensorInterfaceData, MessageParcel& sensorInterfaceReply, MessageOption& sensorInterfaceOption, sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl)
{
    if (sensorInterfaceData.ReadInterfaceToken() != OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t majorVer = 0;

    uint32_t minorVer = 0;

    if (impl == nullptr) {
        HDF_LOGE("%{public}s: impl is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t sensorInterfaceRet = impl->GetVersion(majorVer, minorVer);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGD("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    if (!sensorInterfaceReply.WriteUint32(majorVer)) {
        HDF_LOGE("%{public}s: write majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceReply.WriteUint32(minorVer)) {
        HDF_LOGE("%{public}s: write minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return sensorInterfaceRet;
}
} // V3_0
} // Sensor
} // HDI
} // OHOS
