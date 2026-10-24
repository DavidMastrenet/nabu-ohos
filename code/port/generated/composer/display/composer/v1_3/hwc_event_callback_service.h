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

#ifndef OHOS_HDI_DISPLAY_COMPOSER_V1_3_HWCEVENTCALLBACKSERVICE_H
#define OHOS_HDI_DISPLAY_COMPOSER_V1_3_HWCEVENTCALLBACKSERVICE_H

#include "v1_3/ihwc_event_callback.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace Composer {
namespace V1_3 {
class HwcEventCallbackService : public OHOS::HDI::Display::Composer::V1_3::IHwcEventCallback {
public:
    HwcEventCallbackService() = default;
    virtual ~HwcEventCallbackService() = default;

    int32_t OnHwcEvent(uint32_t devId, uint32_t eventId, const std::vector<int32_t>& eventData) override;

};
} // V1_3
} // Composer
} // Display
} // HDI
} // OHOS

#endif // OHOS_HDI_DISPLAY_COMPOSER_V1_3_HWCEVENTCALLBACKSERVICE_H

