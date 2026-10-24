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

#ifndef OHOS_HDI_SENSOR_V3_0_ISENSORINTERFACE_H
#define OHOS_HDI_SENSOR_V3_0_ISENSORINTERFACE_H

#include <stdint.h>
#include <vector>
#include <hdf_base.h>
#include <hdi_base.h>
#include "sensor\v3_0\isensor_callback.h"
#include "sensor\v3_0\isensor_plug_callback.h"
#include "sensor\v3_0\sensor_types.h"

#ifndef HDI_BUFF_MAX_SIZE
#define HDI_BUFF_MAX_SIZE (1024 * 200)
#endif

#ifndef HDI_VECTOR_MAX_SIZE
#define HDI_VECTOR_MAX_SIZE 102400000
#endif

#ifndef HDI_MAP_MAX_SIZE
#define HDI_MAP_MAX_SIZE 102400000
#endif

#ifndef HDI_CHECK_VALUE_RETURN
#define HDI_CHECK_VALUE_RETURN(lv, compare, rv, ret) do { \
    if ((lv) compare (rv)) { \
        return ret; \
    } \
} while (false)
#endif

#ifndef HDI_CHECK_VALUE_RET_GOTO
#define HDI_CHECK_VALUE_RET_GOTO(lv, compare, rv, ret, value, table) do { \
    if ((lv) compare (rv)) { \
        ret = value; \
        goto table; \
    } \
} while (false)
#endif

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {
using namespace OHOS;
using namespace OHOS::HDI;

enum {
    CMD_SENSOR_INTERFACE_GET_VERSION = 0,
    CMD_SENSOR_INTERFACE_GET_ALL_SENSOR_INFO = 1,
    CMD_SENSOR_INTERFACE_GET_DEVICE_SENSOR_INFO = 2,
    CMD_SENSOR_INTERFACE_ENABLE = 3,
    CMD_SENSOR_INTERFACE_DISABLE = 4,
    CMD_SENSOR_INTERFACE_SET_BATCH = 5,
    CMD_SENSOR_INTERFACE_SET_MODE = 6,
    CMD_SENSOR_INTERFACE_SET_OPTION = 7,
    CMD_SENSOR_INTERFACE_REGISTER = 8,
    CMD_SENSOR_INTERFACE_UNREGISTER = 9,
    CMD_SENSOR_INTERFACE_READ_DATA = 10,
    CMD_SENSOR_INTERFACE_SET_SDC_SENSOR = 11,
    CMD_SENSOR_INTERFACE_GET_SDC_SENSOR_INFO = 12,
    CMD_SENSOR_INTERFACE_REGISTER_ASYNC = 13,
    CMD_SENSOR_INTERFACE_UNREGISTER_ASYNC = 14,
    CMD_SENSOR_INTERFACE_REG_SENSOR_PLUG_CALL_BACK = 15,
    CMD_SENSOR_INTERFACE_UN_REG_SENSOR_PLUG_CALL_BACK = 16,
};

class ISensorInterface : public HdiBase {
public:
    DECLARE_HDI_DESCRIPTOR(u"ohos.hdi.sensor.v3_0.ISensorInterface");

    virtual ~ISensorInterface() = default;

    static sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> Get(bool isStub = false);
    static sptr<OHOS::HDI::Sensor::V3_0::ISensorInterface> Get(const std::string &serviceName, bool isStub = false);

    virtual int32_t GetAllSensorInfo(std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info) = 0;

    virtual int32_t GetDeviceSensorInfo(int32_t deviceId,
         std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorInformation>& info) = 0;

    virtual int32_t Enable(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo) = 0;

    virtual int32_t Disable(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo) = 0;

    virtual int32_t SetBatch(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
         int64_t samplingInterval, int64_t reportInterval) = 0;

    virtual int32_t SetMode(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, int32_t mode) = 0;

    virtual int32_t SetOption(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, uint32_t option) = 0;

    virtual int32_t Register(int32_t groupId, const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj) = 0;

    virtual int32_t Unregister(int32_t groupId, const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj) = 0;

    virtual int32_t ReadData(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo,
         std::vector<OHOS::HDI::Sensor::V3_0::HdfSensorEvents>& event) = 0;

    virtual int32_t SetSdcSensor(const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& deviceSensorInfo, bool enabled,
         int32_t rateLevel) = 0;

    virtual int32_t GetSdcSensorInfo(std::vector<OHOS::HDI::Sensor::V3_0::SdcSensorInfo>& sdcSensorInfo) = 0;

    virtual int32_t RegisterAsync(int32_t groupId,
         const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj) = 0;

    virtual int32_t UnregisterAsync(int32_t groupId,
         const sptr<OHOS::HDI::Sensor::V3_0::ISensorCallback>& callbackObj) = 0;

    virtual int32_t RegSensorPlugCallBack(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj) = 0;

    virtual int32_t UnRegSensorPlugCallBack(const sptr<OHOS::HDI::Sensor::V3_0::ISensorPlugCallback>& callbackObj) = 0;

    virtual int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer)
    {
        majorVer = 3;
        minorVer = 0;
        return HDF_SUCCESS;
    }

    virtual bool IsProxy()
    {
        return false;
    }

    virtual const std::u16string GetDesc()
    {
        return metaDescriptor_;
    }
};
} // V3_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V3_0_ISENSORINTERFACE_H

