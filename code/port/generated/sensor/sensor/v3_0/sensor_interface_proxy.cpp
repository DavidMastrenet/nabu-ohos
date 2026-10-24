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

#include "v3_0/sensor_interface_proxy.h"
#include <hdf_base.h>
#include <hdf_core_log.h>
#include <hdi_support.h>
#include <iservmgr_hdi.h>
#include <message_option.h>
#include <message_parcel.h>
#include <object_collector.h>
#include <string_ex.h>

#define HDF_LOG_TAG    sensor_interface_proxy

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {

template<typename ElementType>
static bool ReadPodArray(MessageParcel &parcel, std::vector<ElementType> &data);

sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> OHOS::HDI::Sensor::V3_0::ISensorInterface::Get(bool isStub)
{
    return ISensorInterface::Get("sensor_interface_service", isStub);
}

sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> OHOS::HDI::Sensor::V3_0::ISensorInterface::Get(const std::string& serviceName, bool isStub)
{
    if (isStub) {
        std::string desc = Str16ToStr8(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor());
        std::string svrName = (serviceName == "sensor_interface_service") ? "service" : serviceName;
        sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> impl = \
            reinterpret_cast<OHOS::HDI::Sensor::V3_0::ISensorInterface *>(LoadHdiImpl(desc.c_str(), svrName.c_str()));
        if (impl == nullptr) {
            HDF_LOGE("failed to load hdi impl %{public}s", desc.data());
            return nullptr;
        }
        uint32_t serMajorVer = 0;
        uint32_t serMinorVer = 0;
        int32_t sensorInterfaceRet = impl->GetVersion(serMajorVer, serMinorVer);
        if (sensorInterfaceRet != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: get version failed!", __func__);
            return nullptr;
        }

        if (serMajorVer != 3) {
            HDF_LOGE("%{public}s:check version failed! version of service:%{public}u.%{public}u, version of client:3.0", __func__, serMajorVer, serMinorVer);
            return nullptr;
        }
        if (serMinorVer < 0) {
            HDF_LOGE("%{public}s:check Minor version failed! "
                "client minor version(0) should be less or equal to implementation minor version(%{public}u).", __func__, serMinorVer);
            return nullptr;
        }

        return impl;
    }

    using namespace OHOS::HDI::ServiceManager::V1_0;
    auto servMgr = IServiceManager::Get();
    if (servMgr == nullptr) {
        HDF_LOGE("%{public}s:get IServiceManager failed!", __func__);
        return nullptr;
    }

    sptr<IRemoteObject> remote = servMgr->GetService(serviceName.c_str());
    if (remote == nullptr) {
        HDF_LOGE("%{public}s:get remote object failed!", __func__);
        return nullptr;
    }

    sptr<OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy> proxy = new OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy(remote);
    if (proxy == nullptr) {
        HDF_LOGE("%{public}s:iface_cast failed!", __func__);
        return nullptr;
    }

    proxy->servMgr_ = OHOS::HDI::hdi_objcast<IServiceManager>(servMgr);
    proxy->deathRecipient_ = new OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::IServiceManagerDeathRecipient(proxy);
    proxy->servMgr_->AddDeathRecipient(proxy->deathRecipient_);
    proxy->isReconnected_ = false;
    proxy->serviceName_ = serviceName;
    uint32_t serMajorVer = 0;
    uint32_t serMinorVer = 0;
    int32_t sensorInterfaceRet = proxy->GetVersion(serMajorVer, serMinorVer);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:get version failed!", __func__);
        return nullptr;
    }

    if (serMajorVer != 3) {
        HDF_LOGE("%{public}s:check version failed! version of service:%{public}u.%{public}u, version of client:3.0", __func__, serMajorVer, serMinorVer);
        return nullptr;
    }

    if (serMinorVer < 0) {
        HDF_LOGE("%{public}s:check Minor version failed! "
            "client minor version(0) should be less or equal to server minor version(%{public}u).", __func__, serMinorVer);
        return nullptr;
    }

    return proxy;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Reconnect(
    sptr<OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy> proxy) 
{
    if (proxy == nullptr) {
        HDF_LOGW("Reconnect failed : input proxy is null");
        return HDF_FAILURE;
    }
    using namespace OHOS::HDI::ServiceManager::V1_0;
    proxy->isReconnected_ = false;
    auto iServMgr = IServiceManager::Get();
    if (iServMgr == nullptr) {
        HDF_LOGW("Reconnect failed : iServMgr is null");
        return HDF_FAILURE;
    };
    proxy->reconnectRemote_ = iServMgr->GetService(proxy->serviceName_.c_str());
    if (proxy->reconnectRemote_ == nullptr) {
        HDF_LOGW("Reconnect failed : reconnectRemote_ is null");
        return HDF_FAILURE;
    }
    proxy->servMgr_ = OHOS::HDI::hdi_objcast<IServiceManager>(iServMgr);
    if (proxy->servMgr_ == nullptr) {
        HDF_LOGE("%{public}s:get IServiceManager failed!", __func__);
        return HDF_FAILURE;
    }
    proxy->servMgr_->AddDeathRecipient(
        new OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::IServiceManagerDeathRecipient(proxy));
    proxy->isReconnected_ = true;
    return HDF_SUCCESS;
}


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
int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetAllSensorInfo(std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetAllSensorInfo_(info, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetDeviceSensorInfo(int32_t deviceId,
     std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetDeviceSensorInfo_(deviceId, info, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Enable(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Enable_(deviceSensorInfo, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Disable(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Disable_(deviceSensorInfo, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetBatch(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, int64_t samplingInterval, int64_t reportInterval)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetBatch_(deviceSensorInfo, samplingInterval, reportInterval, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetMode(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, int32_t mode)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetMode_(deviceSensorInfo, mode, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetOption(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, uint32_t option)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetOption_(deviceSensorInfo, option, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Register(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Register_(groupId, callbackObj, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Unregister(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Unregister_(groupId, callbackObj, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::ReadData(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& event)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::ReadData_(deviceSensorInfo, event, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetSdcSensor(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, bool enabled, int32_t rateLevel)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetSdcSensor_(deviceSensorInfo, enabled, rateLevel, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetSdcSensorInfo(std::vector<OHOS::HDI::Sensor::V3_0::SdcSensorInfo>& sdcSensorInfo)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetSdcSensorInfo_(sdcSensorInfo, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::RegisterAsync(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::RegisterAsync_(groupId, callbackObj, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::UnregisterAsync(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::UnregisterAsync_(groupId, callbackObj, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::RegSensorPlugCallBack(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::RegSensorPlugCallBack_(callbackObj, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::UnRegSensorPlugCallBack(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::UnRegSensorPlugCallBack_(callbackObj, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetVersion(uint32_t& majorVer, uint32_t& minorVer)
{
    return OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetVersion_(majorVer, minorVer, GetCurrentRemote());
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetAllSensorInfo_(std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteBool(false)) {
        HDF_LOGE("%{public}s:failed to write flag of memory setting!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_GET_ALL_SENSOR_INFO, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    uint32_t infoSize = 0;
    if (!sensorInterfaceReply.ReadUint32(infoSize)) {
        HDF_LOGE("%{public}s: failed to read size", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (infoSize > HDI_VECTOR_MAX_SIZE) {
        HDF_LOGE("%{public}s: info size exceeds limit", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDI_CHECK_VALUE_RETURN(infoSize, >, HDI_BUFF_MAX_SIZE / sizeof(OHOS::HDI::Sensor::V3_0::HdfSensorInformation), HDF_ERR_INVALID_PARAM);
    info.clear();
    info.reserve(infoSize);
    for (uint32_t i0 = 0; i0 < infoSize; ++i0) {
        OHOS::HDI::Sensor::V3_0::HdfSensorInformation value0;
        if (!HdfSensorInformationBlockUnmarshalling(sensorInterfaceReply, value0)) {
            HDF_LOGE("%{public}s: read value0 failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        info.push_back(value0);
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetDeviceSensorInfo_(int32_t deviceId,
     std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteBool(false)) {
        HDF_LOGE("%{public}s:failed to write flag of memory setting!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteInt32(deviceId)) {
        HDF_LOGE("%{public}s: write deviceId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_GET_DEVICE_SENSOR_INFO, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    uint32_t infoSize = 0;
    if (!sensorInterfaceReply.ReadUint32(infoSize)) {
        HDF_LOGE("%{public}s: failed to read size", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (infoSize > HDI_VECTOR_MAX_SIZE) {
        HDF_LOGE("%{public}s: info size exceeds limit", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDI_CHECK_VALUE_RETURN(infoSize, >, HDI_BUFF_MAX_SIZE / sizeof(OHOS::HDI::Sensor::V3_0::HdfSensorInformation), HDF_ERR_INVALID_PARAM);
    info.clear();
    info.reserve(infoSize);
    for (uint32_t i0 = 0; i0 < infoSize; ++i0) {
        OHOS::HDI::Sensor::V3_0::HdfSensorInformation value0;
        if (!HdfSensorInformationBlockUnmarshalling(sensorInterfaceReply, value0)) {
            HDF_LOGE("%{public}s: read value0 failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        info.push_back(value0);
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Enable_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockMarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_ENABLE, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Disable_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockMarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_DISABLE, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetBatch_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, int64_t samplingInterval, int64_t reportInterval, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockMarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteInt64(samplingInterval)) {
        HDF_LOGE("%{public}s: write samplingInterval failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteInt64(reportInterval)) {
        HDF_LOGE("%{public}s: write reportInterval failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_SET_BATCH, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetMode_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, int32_t mode, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockMarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteInt32(mode)) {
        HDF_LOGE("%{public}s: write mode failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_SET_MODE, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetOption_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, uint32_t option, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockMarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteUint32(option)) {
        HDF_LOGE("%{public}s: write option failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_SET_OPTION, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Register_(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteInt32(groupId)) {
        HDF_LOGE("%{public}s: write groupId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: parameter callbackObj is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteRemoteObject(OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(callbackObj, OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor()))) {
        HDF_LOGE("%{public}s: write callbackObj failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_REGISTER, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::Unregister_(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteInt32(groupId)) {
        HDF_LOGE("%{public}s: write groupId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: parameter callbackObj is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteRemoteObject(OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(callbackObj, OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor()))) {
        HDF_LOGE("%{public}s: write callbackObj failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_UNREGISTER, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::ReadData_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& event, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteBool(false)) {
        HDF_LOGE("%{public}s:failed to write flag of memory setting!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockMarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_READ_DATA, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    uint32_t eventSize = 0;
    if (!sensorInterfaceReply.ReadUint32(eventSize)) {
        HDF_LOGE("%{public}s: failed to read size", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (eventSize > HDI_VECTOR_MAX_SIZE) {
        HDF_LOGE("%{public}s: event size exceeds limit", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDI_CHECK_VALUE_RETURN(eventSize, >, HDI_BUFF_MAX_SIZE / sizeof(OHOS::HDI::Sensor::V3_0::HdfSensorEvents), HDF_ERR_INVALID_PARAM);
    event.clear();
    event.reserve(eventSize);
    for (uint32_t i0 = 0; i0 < eventSize; ++i0) {
        OHOS::HDI::Sensor::V3_0::HdfSensorEvents value0;
        if (!HdfSensorEventsBlockUnmarshalling(sensorInterfaceReply, value0)) {
            HDF_LOGE("%{public}s: read value0 failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
        event.push_back(value0);
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::SetSdcSensor_(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, bool enabled, int32_t rateLevel, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!OHOS::HDI::Sensor::V3_0::DeviceSensorInfoBlockMarshalling(sensorInterfaceData, deviceSensorInfo)) {
        HDF_LOGE("%{public}s: write deviceSensorInfo failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteBool(enabled)) {
        HDF_LOGE("%{public}s: write enabled failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteInt32(rateLevel)) {
        HDF_LOGE("%{public}s: write rateLevel failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_SET_SDC_SENSOR, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetSdcSensorInfo_(std::vector<OHOS::HDI::Sensor::V3_0::SdcSensorInfo>& sdcSensorInfo, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteBool(false)) {
        HDF_LOGE("%{public}s:failed to write flag of memory setting!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_GET_SDC_SENSOR_INFO, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    if (!ReadPodArray(sensorInterfaceReply, sdcSensorInfo)) {
        HDF_LOGE("%{public}s: failed to read sdcSensorInfo", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::RegisterAsync_(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteInt32(groupId)) {
        HDF_LOGE("%{public}s: write groupId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: parameter callbackObj is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteRemoteObject(OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(callbackObj, OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor()))) {
        HDF_LOGE("%{public}s: write callbackObj failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_REGISTER_ASYNC, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::UnregisterAsync_(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteInt32(groupId)) {
        HDF_LOGE("%{public}s: write groupId failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: parameter callbackObj is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteRemoteObject(OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(callbackObj, OHOS::HDI::Sensor::V3_0::ISensorCallback::GetDescriptor()))) {
        HDF_LOGE("%{public}s: write callbackObj failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_UNREGISTER_ASYNC, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::RegSensorPlugCallBack_(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: parameter callbackObj is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteRemoteObject(OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(callbackObj, OHOS::HDI::Sensor::V3_0::ISensorPlugCallback::GetDescriptor()))) {
        HDF_LOGE("%{public}s: write callbackObj failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_REG_SENSOR_PLUG_CALL_BACK, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::UnRegSensorPlugCallBack_(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj, const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (callbackObj == nullptr) {
        HDF_LOGE("%{public}s: parameter callbackObj is nullptr!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceData.WriteRemoteObject(OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(callbackObj, OHOS::HDI::Sensor::V3_0::ISensorPlugCallback::GetDescriptor()))) {
        HDF_LOGE("%{public}s: write callbackObj failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_UN_REG_SENSOR_PLUG_CALL_BACK, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    return sensorInterfaceRet;
}

int32_t OHOS::HDI::Sensor::V3_0::SensorInterfaceProxy::GetVersion_(uint32_t& majorVer, uint32_t& minorVer,
     const sptr<IRemoteObject> remote)
{
    MessageParcel sensorInterfaceData;
    MessageParcel sensorInterfaceReply;
    MessageOption sensorInterfaceOption(MessageOption::TF_SYNC);

    if (!sensorInterfaceData.WriteInterfaceToken(OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t sensorInterfaceRet = remote->SendRequest(CMD_SENSOR_INTERFACE_GET_VERSION, sensorInterfaceData, sensorInterfaceReply, sensorInterfaceOption);
    if (sensorInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, sensorInterfaceRet);
        return sensorInterfaceRet;
    }

    if (!sensorInterfaceReply.ReadUint32(majorVer)) {
        HDF_LOGE("%{public}s: read majorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!sensorInterfaceReply.ReadUint32(minorVer)) {
        HDF_LOGE("%{public}s: read minorVer failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return sensorInterfaceRet;
}

} // V3_0
} // Sensor
} // HDI
} // OHOS
