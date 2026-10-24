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

#ifndef OHOS_HDI_SENSOR_V3_0_SENSORTYPES_H
#define OHOS_HDI_SENSOR_V3_0_SENSORTYPES_H

#include <cstdbool>
#include <cstdint>
#include <string>
#include <vector>

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
class MessageParcel;
}

namespace OHOS {
namespace HDI {
namespace Sensor {
namespace V3_0 {

using namespace OHOS;

enum Location : int32_t {
    SENSOR_EXTERNAL = 0,
    LOCAL = 1,
};

struct DeviceSensorInfo {
    int32_t deviceId;
    int32_t sensorType;
    int32_t sensorId;
    int32_t location;
} __attribute__ ((aligned(8)));

struct HdfSensorInformation {
    std::string sensorName;
    std::string vendorName;
    std::string firmwareVersion;
    std::string hardwareVersion;
    float maxRange;
    float accuracy;
    float power;
    int64_t minDelay;
    int64_t maxDelay;
    uint32_t fifoMaxEventCount;
    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    uint32_t reserved;
};

struct HdfSensorEvents {
    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    int32_t version;
    int64_t timestamp;
    uint32_t option;
    int32_t mode;
    std::vector<uint8_t> data;
    uint32_t dataLen;
};

enum HdfSensorTypeTag : int32_t {
    HDF_SENSOR_TYPE_NONE = 0,
    HDF_SENSOR_TYPE_ACCELEROMETER = 1,
    HDF_SENSOR_TYPE_GYROSCOPE = 2,
    HDF_SENSOR_TYPE_PHOTOPLETHYSMOGRAPH = 3,
    HDF_SENSOR_TYPE_ELECTROCARDIOGRAPH = 4,
    HDF_SENSOR_TYPE_AMBIENT_LIGHT = 5,
    HDF_SENSOR_TYPE_MAGNETIC_FIELD = 6,
    HDF_SENSOR_TYPE_CAPACITIVE = 7,
    HDF_SENSOR_TYPE_BAROMETER = 8,
    HDF_SENSOR_TYPE_TEMPERATURE = 9,
    HDF_SENSOR_TYPE_HALL = 10,
    HDF_SENSOR_TYPE_GESTURE = 11,
    HDF_SENSOR_TYPE_PROXIMITY = 12,
    HDF_SENSOR_TYPE_HUMIDITY = 13,
    HDF_SENSOR_TYPE_COLOR = 14,
    HDF_SENSOR_TYPE_SAR = 15,
    HDF_SENSOR_TYPE_AMBIENT_LIGHT1 = 16,
    HDF_SENSOR_TYPE_HALL1 = 17,
    HDF_SENSOR_TYPE_PROXIMITY1 = 18,
    HDF_SENSOR_TYPE_MEDICAL_BEGIN = 128,
    HDF_SENSOR_TYPE_MEDICAL_END = 160,
    HDF_SENSOR_TYPE_PHYSICAL_MAX = 255,
    HDF_SENSOR_TYPE_ORIENTATION = 256,
    HDF_SENSOR_TYPE_GRAVITY = 257,
    HDF_SENSOR_TYPE_LINEAR_ACCELERATION = 258,
    HDF_SENSOR_TYPE_ROTATION_VECTOR = 259,
    HDF_SENSOR_TYPE_AMBIENT_TEMPERATURE = 260,
    HDF_SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED = 261,
    HDF_SENSOR_TYPE_GAME_ROTATION_VECTOR = 262,
    HDF_SENSOR_TYPE_GYROSCOPE_UNCALIBRATED = 263,
    HDF_SENSOR_TYPE_SIGNIFICANT_MOTION = 264,
    HDF_SENSOR_TYPE_PEDOMETER_DETECTION = 265,
    HDF_SENSOR_TYPE_PEDOMETER = 266,
    HDF_SENSOR_TYPE_POSTURE = 267,
    HDF_SENSOR_TYPE_HEADPOSTURE = 268,
    HDF_SENSOR_TYPE_DROP_DETECT = 269,
    HDF_SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR = 277,
    HDF_SENSOR_TYPE_HEART_RATE = 278,
    HDF_SENSOR_TYPE_DEVICE_ORIENTATION = 279,
    HDF_SENSOR_TYPE_WEAR_DETECTION = 280,
    HDF_SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED = 281,
    HDF_SENSOR_TYPE_RPC = 282,
    HDF_SENSOR_TYPE_FUSION_PRESSURE = 283,
    HDF_SENSOR_TYPE_MAX,
};

enum HdfSensorGroupType : int32_t {
    HDF_TRADITIONAL_SENSOR_TYPE = 0,
    HDF_MEDICAL_SENSOR_TYPE = 1,
    HDF_SENSOR_GROUP_TYPE_MAX,
};

struct SdcSensorInfo {
    uint64_t offset;
    int32_t ddrSize;
    int32_t minRateLevel;
    int32_t maxRateLevel;
    uint64_t memAddr;
    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    int32_t reserved;
} __attribute__ ((aligned(8)));

struct SensorPlugInfo {
    OHOS::HDI::Sensor::V3_0::DeviceSensorInfo deviceSensorInfo;
    std::string deviceName;
    int32_t status;
    int32_t reserved;
};

bool DeviceSensorInfoBlockMarshalling(OHOS::MessageParcel &data, const OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& dataBlock);

bool DeviceSensorInfoBlockUnmarshalling(OHOS::MessageParcel &data, OHOS::HDI::Sensor::V3_0::DeviceSensorInfo& dataBlock);

bool HdfSensorInformationBlockMarshalling(OHOS::MessageParcel &data, const OHOS::HDI::Sensor::V3_0::HdfSensorInformation& dataBlock);

bool HdfSensorInformationBlockUnmarshalling(OHOS::MessageParcel &data, OHOS::HDI::Sensor::V3_0::HdfSensorInformation& dataBlock);

bool HdfSensorEventsBlockMarshalling(OHOS::MessageParcel &data, const OHOS::HDI::Sensor::V3_0::HdfSensorEvents& dataBlock);

bool HdfSensorEventsBlockUnmarshalling(OHOS::MessageParcel &data, OHOS::HDI::Sensor::V3_0::HdfSensorEvents& dataBlock);

bool SdcSensorInfoBlockMarshalling(OHOS::MessageParcel &data, const OHOS::HDI::Sensor::V3_0::SdcSensorInfo& dataBlock);

bool SdcSensorInfoBlockUnmarshalling(OHOS::MessageParcel &data, OHOS::HDI::Sensor::V3_0::SdcSensorInfo& dataBlock);

bool SensorPlugInfoBlockMarshalling(OHOS::MessageParcel &data, const OHOS::HDI::Sensor::V3_0::SensorPlugInfo& dataBlock);

bool SensorPlugInfoBlockUnmarshalling(OHOS::MessageParcel &data, OHOS::HDI::Sensor::V3_0::SensorPlugInfo& dataBlock);

} // V3_0
} // Sensor
} // HDI
} // OHOS

#endif // OHOS_HDI_SENSOR_V3_0_SENSORTYPES_H

