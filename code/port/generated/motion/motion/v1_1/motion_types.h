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

#ifndef OHOS_HDI_MOTION_V1_1_MOTIONTYPES_H
#define OHOS_HDI_MOTION_V1_1_MOTIONTYPES_H

#include <cstdbool>
#include <cstdint>

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
namespace Motion {
namespace V1_1 {

using namespace OHOS;

struct WaveParam {
    int32_t waveFrequency;
    int32_t waveAmplitude;
    bool isUseGyroscope;
} __attribute__ ((aligned(8)));

enum HdfMotionTypeTag : int32_t {
    HDF_MOTION_TYPE_PICKUP = 0,
    HDF_MOTION_TYPE_FLIP,
    HDF_MOTION_CLOSE_TO_EAR,
    HDF_MOTION_TYPE_SHAKE,
    HDF_MOTION_TYPE_ROTATION,
    HDF_MOTION_TYPE_POCKET_MODE,
    HDF_MOTION_TYPE_LEAVE_EAR,
    HDF_MOTION_TYPE_WRIST_UP,
    HDF_MOTION_TYPE_WRIST_DOWN,
    HDF_MOTION_TYPE_WAVE,
    HDF_MOTION_TYPE_STEP_COUNTER,
    HDF_MOTION_TYPE_TOUCH_LINK,
    HDF_MOTION_TYPE_HOVER,
    HDF_MOTION_TYPE_PUT_IN_POCKET,
    HDF_MOTION_TYPE_RESERVED,
    HDF_MOTION_TYPE_MAX,
};

bool WaveParamBlockMarshalling(OHOS::MessageParcel &data, const OHOS::HDI::Motion::V1_1::WaveParam& dataBlock);

bool WaveParamBlockUnmarshalling(OHOS::MessageParcel &data, OHOS::HDI::Motion::V1_1::WaveParam& dataBlock);


} // V1_1
} // Motion
} // HDI
} // OHOS

#endif // OHOS_HDI_MOTION_V1_1_MOTIONTYPES_H

