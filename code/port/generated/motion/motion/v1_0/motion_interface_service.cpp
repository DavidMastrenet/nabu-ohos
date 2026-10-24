/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "v1_0/motion_interface_service.h"
#include <hdf_base.h>

#define HDF_LOG_TAG    motion_interface_service

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_0 {
extern "C" IMotionInterface *MotionInterfaceImplGetInstance(void)
{
    return new (std::nothrow) MotionInterfaceService();
}

int32_t MotionInterfaceService::EnableMotion(int32_t motionType)
{
    return HDF_SUCCESS;
}

int32_t MotionInterfaceService::DisableMotion(int32_t motionType)
{
    return HDF_SUCCESS;
}

int32_t MotionInterfaceService::Register(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

int32_t MotionInterfaceService::Unregister(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj)
{
    return HDF_SUCCESS;
}

} // V1_0
} // Motion
} // HDI
} // OHOS
