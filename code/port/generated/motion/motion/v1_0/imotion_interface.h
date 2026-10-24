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

#ifndef OHOS_HDI_MOTION_V1_0_IMOTIONINTERFACE_H
#define OHOS_HDI_MOTION_V1_0_IMOTIONINTERFACE_H

#include <stdint.h>
#include <hdf_base.h>
#include <hdi_base.h>
#include "motion\v1_0\imotion_callback.h"
#include "motion\v1_0\motion_types.h"

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
namespace Motion {
namespace V1_0 {
using namespace OHOS;
using namespace OHOS::HDI;

enum {
    CMD_MOTION_INTERFACE_GET_VERSION = 0,
    CMD_MOTION_INTERFACE_ENABLE_MOTION = 1,
    CMD_MOTION_INTERFACE_DISABLE_MOTION = 2,
    CMD_MOTION_INTERFACE_REGISTER = 3,
    CMD_MOTION_INTERFACE_UNREGISTER = 4,
};

class IMotionInterface : public HdiBase {
public:
    DECLARE_HDI_DESCRIPTOR(u"ohos.hdi.motion.v1_0.IMotionInterface");

    virtual ~IMotionInterface() = default;

    static sptr<OHOS::HDI::Motion::V1_0::IMotionInterface> Get(bool isStub = false);
    static sptr<OHOS::HDI::Motion::V1_0::IMotionInterface> Get(const std::string &serviceName, bool isStub = false);

    virtual int32_t EnableMotion(int32_t motionType) = 0;

    virtual int32_t DisableMotion(int32_t motionType) = 0;

    virtual int32_t Register(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj) = 0;

    virtual int32_t Unregister(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj) = 0;

    virtual int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer)
    {
        majorVer = 1;
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
} // V1_0
} // Motion
} // HDI
} // OHOS

#endif // OHOS_HDI_MOTION_V1_0_IMOTIONINTERFACE_H

