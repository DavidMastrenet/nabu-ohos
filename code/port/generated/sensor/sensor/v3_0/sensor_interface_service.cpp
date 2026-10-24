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

#include "v3_0/sensor_interface_service.h"
#include <hdf_base.h>

#define HDF_LOG_TAG    sensor_interface_service

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {
extern "C" ISensorInterface *SensorInterfaceImplGetInstance(void)
{
    return new (std::nothrow) SensorInterfaceService();
}

int32_t SensorInterfaceService::GetAllSensorInfo(std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::GetDeviceSensorInfo(int32_t deviceId,
     std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::Enable(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::Disable(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::SetBatch(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
     int64_t samplingInterval, int64_t reportInterval)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::SetMode(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, int32_t mode)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::SetOption(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
     uint32_t option)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::Register(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::Unregister(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::ReadData(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
     std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& event)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::SetSdcSensor(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
     bool enabled, int32_t rateLevel)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::GetSdcSensorInfo(std::vector<OHOS::HDI::Sensor::V3_0::SdcSensorInfo>& sdcSensorInfo)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::RegisterAsync(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::UnregisterAsync(int32_t groupId,
     const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::RegSensorPlugCallBack(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

int32_t SensorInterfaceService::UnRegSensorPlugCallBack(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

} // V3_0
} // Sensor
} // HDI
} // OHOS
