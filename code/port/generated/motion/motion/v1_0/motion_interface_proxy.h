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

#ifndef OHOS_HDI_MOTION_V1_0_MOTIONINTERFACEPROXY_H
#define OHOS_HDI_MOTION_V1_0_MOTIONINTERFACEPROXY_H

#include "v1_0/imotion_interface.h"
#include <unistd.h>
#include <iproxy_broker.h>

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_0 {

class MotionInterfaceProxy : public IProxyBroker<OHOS::HDI::Motion::V1_0::IMotionInterface> {
public:
    class IServiceManagerDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        IServiceManagerDeathRecipient(wptr<OHOS::HDI::Motion::V1_0::MotionInterfaceProxy> proxy) : proxy_(proxy) {} 
        ~IServiceManagerDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            int32_t result = HDF_FAILURE;
            const int sleepInterval = 500000;
            const int waitTimes = 10;
            int currentTime = waitTimes;
            do {
                usleep(sleepInterval);
                auto proxy = proxy_.promote();
                if (proxy != nullptr) {
                    result = OHOS::HDI::Motion::V1_0::MotionInterfaceProxy::Reconnect(proxy);
                }
                --currentTime;
            } while (result != HDF_SUCCESS && currentTime >0);
        }
    private:
        wptr<OHOS::HDI::Motion::V1_0::MotionInterfaceProxy> proxy_;
    };

    explicit MotionInterfaceProxy(const sptr<IRemoteObject>& remote) : IProxyBroker<OHOS::HDI::Motion::V1_0::IMotionInterface>(remote) {
        reconnectRemote_ = nullptr;
        servMgr_ = nullptr;
        deathRecipient_ = nullptr;
        isReconnected_ = false;
    }
    virtual ~MotionInterfaceProxy() {
        if (servMgr_ != nullptr && deathRecipient_ != nullptr) {
            servMgr_->RemoveDeathRecipient(deathRecipient_);
        }
    }

    inline bool IsProxy() override
    {
        return true;
    }

    int32_t EnableMotion(int32_t motionType) override;

    int32_t DisableMotion(int32_t motionType) override;

    int32_t Register(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj) override;

    int32_t Unregister(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj) override;

    int32_t GetVersion(uint32_t& majorVer, uint32_t& minorVer) override;

    static int32_t EnableMotion_(int32_t motionType, const sptr<IRemoteObject> remote);

    static int32_t DisableMotion_(int32_t motionType, const sptr<IRemoteObject> remote);

    static int32_t Register_(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj,
         const sptr<IRemoteObject> remote);

    static int32_t Unregister_(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj,
         const sptr<IRemoteObject> remote);

    static int32_t GetVersion_(uint32_t& majorVer, uint32_t& minorVer, const sptr<IRemoteObject> remote);

    static int32_t Reconnect(sptr<OHOS::HDI::Motion::V1_0::MotionInterfaceProxy> proxy);

    sptr<IRemoteObject> GetCurrentRemote() {
        return isReconnected_ ? reconnectRemote_ : Remote();
    }

    bool isReconnected_;
    std::string serviceName_;
    sptr<IRemoteObject> servMgr_;
    sptr<OHOS::HDI::Motion::V1_0::MotionInterfaceProxy::IServiceManagerDeathRecipient> deathRecipient_;
    sptr<IRemoteObject> reconnectRemote_;
private:
    static inline BrokerDelegator<OHOS::HDI::Motion::V1_0::MotionInterfaceProxy> delegator_;
};

} // V1_0
} // Motion
} // HDI
} // OHOS

#endif // OHOS_HDI_MOTION_V1_0_MOTIONINTERFACEPROXY_H

