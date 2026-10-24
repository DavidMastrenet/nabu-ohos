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

#ifndef OHOS_HDI_MOTION_V1_0_MOTIONCALLBACKSTUB_H
#define OHOS_HDI_MOTION_V1_0_MOTIONCALLBACKSTUB_H

#include <ipc_object_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include <object_collector.h>
#include <refbase.h>
#include "v1_0/imotion_callback.h"

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_0 {

using namespace OHOS;
class MotionCallbackStub : public IPCObjectStub {
public:
    explicit MotionCallbackStub(const sptr<IMotionCallback> &impl);
    virtual ~MotionCallbackStub();

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    static int32_t MotionCallbackStubOnDataEvent_(MessageParcel& motionCallbackData, MessageParcel& motionCallbackReply, MessageOption& motionCallbackOption, sptr<OHOS::HDI::Motion::V1_0::IMotionCallback> impl);

    static int32_t MotionCallbackStubGetVersion_(MessageParcel& motionCallbackData, MessageParcel& motionCallbackReply, MessageOption& motionCallbackOption, sptr<OHOS::HDI::Motion::V1_0::IMotionCallback> impl);

private:
    int32_t MotionCallbackStubOnDataEvent(MessageParcel& motionCallbackData, MessageParcel& motionCallbackReply, MessageOption& motionCallbackOption);

    int32_t MotionCallbackStubGetVersion(MessageParcel& motionCallbackData, MessageParcel& motionCallbackReply, MessageOption& motionCallbackOption);


    static inline ObjectDelegator<OHOS::HDI::Motion::V1_0::MotionCallbackStub, OHOS::HDI::Motion::V1_0::IMotionCallback> objDelegator_;
    sptr<OHOS::HDI::Motion::V1_0::IMotionCallback> impl_;
};
} // V1_0
} // Motion
} // HDI
} // OHOS

#endif // OHOS_HDI_MOTION_V1_0_MOTIONCALLBACKSTUB_H

