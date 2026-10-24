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

#include "v1_1/motion_interface_proxy.h"
#include <hdf_base.h>
#include <hdf_core_log.h>
#include <hdi_support.h>
#include <iservmgr_hdi.h>
#include <message_option.h>
#include <message_parcel.h>
#include <string_ex.h>

#define HDF_LOG_TAG    motion_interface_proxy

namespace OHOS {
namespace HDI {
namespace Motion {
namespace V1_1 {

template<typename ElementType>
static bool WritePodArray(MessageParcel &parcel, const std::vector<ElementType> &data);

template<typename ChildType, typename ParentType>
static sptr<ChildType> CastFromTemplate(const sptr<ParentType> &parent)
{
    if (parent == nullptr) {
        HDF_LOGE("%{public}s:parent is nullptr!", __func__);
        return nullptr;
    }

    if (!parent->IsProxy()) {
        HDF_LOGE("%{public}s:not proxy, not support castfrom!", __func__);
        return nullptr;
    }

    sptr<IRemoteObject> remote = OHOS::HDI::hdi_objcast<ParentType>(parent);
    if (remote == nullptr) {
        HDF_LOGE("%{public}s:hdi_objcast failed!", __func__);
        return nullptr;
    }

    sptr<ChildType> proxy = OHOS::HDI::hdi_facecast<ChildType>(remote);
    if (proxy == nullptr) {
        HDF_LOGE("%{public}s:hdi_facecast failed!", __func__);
        return nullptr;
    }

    uint32_t serMajorVer = 0;
    uint32_t serMinorVer = 0;
    int32_t motionInterfaceRet = proxy->GetVersion(serMajorVer, serMinorVer);
    if (motionInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:get version failed!", __func__);
        return nullptr;
    }

    if (serMajorVer != 1) {
        HDF_LOGE("%{public}s:check version failed! version of service:%{public}u.%{public}u, version of client:1.1", __func__, serMajorVer, serMinorVer);
        return nullptr;
    }
    if (serMinorVer < 1) {
        HDF_LOGE("%{public}s:check Minor version failed! "
            "client minor version(1) should be less or equal to server minor version(%{public}u).", __func__, serMinorVer);
        return nullptr;
    }


    return proxy;
}

sptr<OHOS::HDI::Motion::V1_1::IMotionInterface> OHOS::HDI::Motion::V1_1::IMotionInterface::Get(bool isStub)
{
    return IMotionInterface::Get("motion_interface_service", isStub);
}

sptr<OHOS::HDI::Motion::V1_1::IMotionInterface> OHOS::HDI::Motion::V1_1::IMotionInterface::Get(const std::string& serviceName, bool isStub)
{
    if (isStub) {
        std::string desc = Str16ToStr8(OHOS::HDI::Motion::V1_1::IMotionInterface::GetDescriptor());
        std::string svrName = (serviceName == "motion_interface_service") ? "service" : serviceName;
        sptr<OHOS::HDI::Motion::V1_1::IMotionInterface> impl = \
            reinterpret_cast<OHOS::HDI::Motion::V1_1::IMotionInterface *>(LoadHdiImpl(desc.c_str(), svrName.c_str()));
        if (impl == nullptr) {
            HDF_LOGE("failed to load hdi impl %{public}s", desc.data());
            return nullptr;
        }
        uint32_t serMajorVer = 0;
        uint32_t serMinorVer = 0;
        int32_t motionInterfaceRet = impl->GetVersion(serMajorVer, serMinorVer);
        if (motionInterfaceRet != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: get version failed!", __func__);
            return nullptr;
        }

        if (serMajorVer != 1) {
            HDF_LOGE("%{public}s:check version failed! version of service:%{public}u.%{public}u, version of client:1.1", __func__, serMajorVer, serMinorVer);
            return nullptr;
        }
        if (serMinorVer < 1) {
            HDF_LOGE("%{public}s:check Minor version failed! "
                "client minor version(1) should be less or equal to implementation minor version(%{public}u).", __func__, serMinorVer);
            return nullptr;
        }

        return impl;
    }

    using namespace OHOS::HDI::ServiceManager::V1_0;
    auto servMgr = IServiceManager::Get();
    if (servMgr == nullptr) {
        HDF_LOGE("%{public}s:get IServiceManager failed!", __func__);
        return nullptr;
    }

    sptr<IRemoteObject> remote = servMgr->GetService(serviceName.c_str());
    if (remote == nullptr) {
        HDF_LOGE("%{public}s:get remote object failed!", __func__);
        return nullptr;
    }

    sptr<OHOS::HDI::Motion::V1_1::MotionInterfaceProxy> proxy = new OHOS::HDI::Motion::V1_1::MotionInterfaceProxy(remote);
    if (proxy == nullptr) {
        HDF_LOGE("%{public}s:iface_cast failed!", __func__);
        return nullptr;
    }

    proxy->servMgr_ = OHOS::HDI::hdi_objcast<IServiceManager>(servMgr);
    proxy->deathRecipient_ = new OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::IServiceManagerDeathRecipient(proxy);
    proxy->servMgr_->AddDeathRecipient(proxy->deathRecipient_);
    proxy->isReconnected_ = false;
    proxy->serviceName_ = serviceName;
    uint32_t serMajorVer = 0;
    uint32_t serMinorVer = 0;
    int32_t motionInterfaceRet = proxy->GetVersion(serMajorVer, serMinorVer);
    if (motionInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:get version failed!", __func__);
        return nullptr;
    }

    if (serMajorVer != 1) {
        HDF_LOGE("%{public}s:check version failed! version of service:%{public}u.%{public}u, version of client:1.1", __func__, serMajorVer, serMinorVer);
        return nullptr;
    }

    if (serMinorVer < 1) {
        HDF_LOGE("%{public}s:check Minor version failed! "
            "client minor version(1) should be less or equal to server minor version(%{public}u).", __func__, serMinorVer);
        return nullptr;
    }

    return proxy;
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::Reconnect(
    sptr<OHOS::HDI::Motion::V1_1::MotionInterfaceProxy> proxy) 
{
    if (proxy == nullptr) {
        HDF_LOGW("Reconnect failed : input proxy is null");
        return HDF_FAILURE;
    }
    using namespace OHOS::HDI::ServiceManager::V1_0;
    proxy->isReconnected_ = false;
    auto iServMgr = IServiceManager::Get();
    if (iServMgr == nullptr) {
        HDF_LOGW("Reconnect failed : iServMgr is null");
        return HDF_FAILURE;
    };
    proxy->reconnectRemote_ = iServMgr->GetService(proxy->serviceName_.c_str());
    if (proxy->reconnectRemote_ == nullptr) {
        HDF_LOGW("Reconnect failed : reconnectRemote_ is null");
        return HDF_FAILURE;
    }
    proxy->servMgr_ = OHOS::HDI::hdi_objcast<IServiceManager>(iServMgr);
    if (proxy->servMgr_ == nullptr) {
        HDF_LOGE("%{public}s:get IServiceManager failed!", __func__);
        return HDF_FAILURE;
    }
    proxy->servMgr_->AddDeathRecipient(
        new OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::IServiceManagerDeathRecipient(proxy));
    proxy->isReconnected_ = true;
    return HDF_SUCCESS;
}

sptr<OHOS::HDI::Motion::V1_1::IMotionInterface> OHOS::HDI::Motion::V1_1::IMotionInterface::CastFrom(const sptr<OHOS::HDI::Motion::V1_0::IMotionInterface> &parent)
{
    return CastFromTemplate<OHOS::HDI::Motion::V1_1::IMotionInterface, OHOS::HDI::Motion::V1_0::IMotionInterface>(parent);
}


template<typename ElementType>
static bool WritePodArray(MessageParcel &parcel, const std::vector<ElementType> &data)
{
    if (!parcel.WriteUint32(data.size())) {
        HDF_LOGE("%{public}s: failed to write data size", __func__);
        return false;
    }
    if (data.empty()) {
        return true;
    }
    size_t sz;
    bool ok = __builtin_mul_overflow(sizeof(ElementType), data.size(), &sz);
    if (ok) { return false; }
    if (!parcel.WriteUnpadBuffer((const void*)data.data(), sz)) {
        HDF_LOGE("%{public}s: failed to write array", __func__);
        return false;
    }
    return true;
}
int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::SetMotionConfig(int32_t motionType,
     const std::vector<uint8_t>& data)
{
    return OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::SetMotionConfig_(motionType, data, GetCurrentRemote());
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::EnableMotion(int32_t motionType)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceProxy::EnableMotion_(motionType, GetCurrentRemote());
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::DisableMotion(int32_t motionType)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceProxy::DisableMotion_(motionType, GetCurrentRemote());
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::Register(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceProxy::Register_(callbackObj, GetCurrentRemote());
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::Unregister(const sptr<OHOS::HDI::Motion::V1_0::IMotionCallback>& callbackObj)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceProxy::Unregister_(callbackObj, GetCurrentRemote());
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::GetVersion(uint32_t& majorVer, uint32_t& minorVer)
{
    return OHOS::HDI::Motion::V1_0::MotionInterfaceProxy::GetVersion_(majorVer, minorVer, GetCurrentRemote());
}

int32_t OHOS::HDI::Motion::V1_1::MotionInterfaceProxy::SetMotionConfig_(int32_t motionType,
     const std::vector<uint8_t>& data, const sptr<IRemoteObject> remote)
{
    MessageParcel motionInterfaceData;
    MessageParcel motionInterfaceReply;
    MessageOption motionInterfaceOption(MessageOption::TF_SYNC);

    if (!motionInterfaceData.WriteInterfaceToken(OHOS::HDI::Motion::V1_1::IMotionInterface::GetDescriptor())) {
        HDF_LOGE("%{public}s: failed to write interface descriptor!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!motionInterfaceData.WriteInt32(motionType)) {
        HDF_LOGE("%{public}s: write motionType failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!WritePodArray(motionInterfaceData, data)) {
        HDF_LOGE("%{public}s: failed to write data", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (remote == nullptr) {
        HDF_LOGE("%{public}s: invalid remote object!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t motionInterfaceRet = remote->SendRequest(CMD_MOTION_INTERFACE_SET_MOTION_CONFIG, motionInterfaceData, motionInterfaceReply, motionInterfaceOption);
    if (motionInterfaceRet != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %{public}d", __func__, motionInterfaceRet);
        return motionInterfaceRet;
    }

    return motionInterfaceRet;
}

} // V1_1
} // Motion
} // HDI
} // OHOS
