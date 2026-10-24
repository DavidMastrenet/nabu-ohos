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

#include "v1_1/motion_types.h"
#include <hdf_core_log.h>
#include <message_parcel.h>
#include <securec.h>

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_1 {



bool WaveParamBlockMarshalling(OHOS::MessageParcel& data, const OHOS::HDI::Motion::V1_1::WaveParam& dataBlock)
{
    if (!data.WriteUnpadBuffer((const void*)&dataBlock, sizeof(OHOS::HDI::Motion::V1_1::WaveParam))) {
        return false;
    }
    return true;
}

bool WaveParamBlockUnmarshalling(OHOS::MessageParcel& data, OHOS::HDI::Motion::V1_1::WaveParam& dataBlock)
{
    const OHOS::HDI::Motion::V1_1::WaveParam *dataBlockPtr = reinterpret_cast<const OHOS::HDI::Motion::V1_1::WaveParam*>(data.ReadUnpadBuffer(sizeof(OHOS::HDI::Motion::V1_1::WaveParam)));
    if (dataBlockPtr == NULL) {
        return false;
    }

    if (memcpy_s(&dataBlock, sizeof(OHOS::HDI::Motion::V1_1::WaveParam), dataBlockPtr, sizeof(OHOS::HDI::Motion::V1_1::WaveParam)) != EOK) {
        return false;
    }
    return true;
}


} // V1_1
} // Motion
} // HDI
} // OHOS
