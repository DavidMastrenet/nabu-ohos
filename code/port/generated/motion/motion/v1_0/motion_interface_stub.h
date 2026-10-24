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

#ifndef OHOS_HDI_MOTION_V1_0_MOTIONINTERFACESTUB_H
#define OHOS_HDI_MOTION_V1_0_MOTIONINTERFACESTUB_H

#include <ipc_object_stub.h>
#include <message_option.h>
#include <message_parcel.h>
#include <object_collector.h>
#include <refbase.h>
#include "v1_0/imotion_interface.h"

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_0 {

using namespace OHOS;
class MotionInterfaceStub : public IPCObjectStub {
public:
    explicit MotionInterfaceStub(const sptr<IMotionInterface> &impl);
    virtual ~MotionInterfaceStub();

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    static int32_t MotionInterfaceStubEnableMotion_(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption, sptr<OHOS::HDI::Motion::V1_0::IMotionInterface> impl);

    static int32_t MotionInterfaceStubDisableMotion_(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption, sptr<OHOS::HDI::Motion::V1_0::IMotionInterface> impl);

    static int32_t MotionInterfaceStubRegister_(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption, sptr<OHOS::HDI::Motion::V1_0::IMotionInterface> impl);

    static int32_t MotionInterfaceStubUnregister_(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption, sptr<OHOS::HDI::Motion::V1_0::IMotionInterface> impl);

    static int32_t MotionInterfaceStubGetVersion_(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption, sptr<OHOS::HDI::Motion::V1_0::IMotionInterface> impl);

private:
    int32_t MotionInterfaceStubEnableMotion(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption);

    int32_t MotionInterfaceStubDisableMotion(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption);

    int32_t MotionInterfaceStubRegister(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption);

    int32_t MotionInterfaceStubUnregister(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption);

    int32_t MotionInterfaceStubGetVersion(MessageParcel& motionInterfaceData, MessageParcel& motionInterfaceReply, MessageOption& motionInterfaceOption);


    static inline ObjectDelegator<OHOS::HDI::Motion::V1_0::MotionInterfaceStub, OHOS::HDI::Motion::V1_0::IMotionInterface> objDelegator_;
    sptr<OHOS::HDI::Motion::V1_0::IMotionInterface> impl_;
};
} // V1_0
} // Motion
} // HDI
} // OHOS

#endif // OHOS_HDI_MOTION_V1_0_MOTIONINTERFACESTUB_H

