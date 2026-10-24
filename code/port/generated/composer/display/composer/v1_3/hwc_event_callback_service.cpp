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

#include "v1_3/hwc_event_callback_service.h"
#include <hdf_base.h>

#define HDF_LOG_TAG    hwc_event_callback_service

namespace OHOS {
namespace HDI {
namespace Display {
namespace Composer {
namespace V1_3 {
int32_t HwcEventCallbackService::OnHwcEvent(uint32_t devId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    return HDF_SUCCESS;
}

} // V1_3
} // Composer
} // Display
} // HDI
} // OHOS
