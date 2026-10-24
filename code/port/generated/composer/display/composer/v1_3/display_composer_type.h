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

#ifndef OHOS_HDI_DISPLAY_COMPOSER_V1_3_DISPLAYCOMPOSERTYPE_H
#define OHOS_HDI_DISPLAY_COMPOSER_V1_3_DISPLAYCOMPOSERTYPE_H

#include <cstdbool>
#include <cstdint>
#include "display\composer\v1_0\display_composer_type.h"
#include "display\composer\v1_1\display_composer_type.h"
#include "display\composer\v1_2\display_composer_type.h"
#include "hdifd_parcelable.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace Composer {
namespace V1_3 {

using namespace OHOS;
using namespace OHOS::HDI::Display::Composer::V1_0;
using namespace OHOS::HDI::Display;
using namespace OHOS::HDI::Display::Composer::V1_1;
using namespace OHOS::HDI::Display::Composer::V1_2;

enum TunnelLayerProperty : int32_t {
    TUNNEL_PROP_INVALID = 0,
    TUNNEL_PROP_POSTION = (1 << 0),
    TUNNEL_PROP_BUFFER_ADDR = (1 << 1),
    TUNNEL_PROP_CLIENT_COMMIT = (1 << 2),
    TUNNEL_PROP_DEVICE_COMMIT = (1 << 3),
    TUNNEL_PROP_SECURE_DEVICE_COMMIT = (1 << 4),
};

enum LayerType : int32_t {
    LAYER_TYPE_GRAPHIC,
    LAYER_TYPE_OVERLAY,
    LAYER_TYPE_SDIEBAND,
    LAYER_TYPE_CURSOR,
    LAYER_TYPE_BUTT,
    LAYER_TYPE_TUNNEL,
};
} // V1_3
} // Composer
} // Display
} // HDI
} // OHOS

#endif // OHOS_HDI_DISPLAY_COMPOSER_V1_3_DISPLAYCOMPOSERTYPE_H

