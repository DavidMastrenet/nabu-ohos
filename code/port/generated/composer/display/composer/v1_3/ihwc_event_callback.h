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

#ifndef OHOS_HDI_DISPLAY_COMPOSER_V1_3_IHWCEVENTCALLBACK_H
#define OHOS_HDI_DISPLAY_COMPOSER_V1_3_IHWCEVENTCALLBACK_H

#include <stdint.h>
#include <vector>
#include <hdf_base.h>
#include <hdi_base.h>
#include "display\composer\v1_3\display_composer_type.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace Composer {
namespace V1_3 {
using namespace OHOS;
using namespace OHOS::HDI;

class IHwcEventCallback : public HdiBase {
public:
    DECLARE_HDI_DESCRIPTOR(u"ohos.hdi.display.composer.v1_3.IHwcEventCallback");

    virtual ~IHwcEventCallback() = default;

    virtual int32_t OnHwcEvent(uint32_t devId, uint32_t eventId, const std::vector<int32_t>& eventData) = 0;

    virtual int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer)
    {
        majorVer = 1;
        minorVer = 3;
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
} // V1_3
} // Composer
} // Display
} // HDI
} // OHOS

#endif // OHOS_HDI_DISPLAY_COMPOSER_V1_3_IHWCEVENTCALLBACK_H

