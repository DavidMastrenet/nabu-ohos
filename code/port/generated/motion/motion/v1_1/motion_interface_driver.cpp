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

#include <hdf_base.h>
#include <hdf_core_log.h>
#include <hdf_device_desc.h>
#include <hdf_sbuf_ipc.h>
#include "v1_1/motion_interface_stub.h"

#define HDF_LOG_TAG    motion_interface_driver

using namespace OHOS::HDI::Motion::V1_1;

struct HdfMotionInterfaceHost {
    struct IDeviceIoService ioService;
    OHOS::sptr<OHOS::IRemoteObject> stub;
};

static int32_t MotionInterfaceDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    auto *hdfMotionInterfaceHost = CONTAINER_OF(client->device->service, struct HdfMotionInterfaceHost, ioService);

    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;
    OHOS::MessageOption option;

    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (SbufToParcel(reply, &replyParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: invalid reply sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return hdfMotionInterfaceHost->stub->SendRequest(cmdId, *dataParcel, *replyParcel, option);
}

static int HdfMotionInterfaceDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: driver init start", __func__);
    return HDF_SUCCESS;
}

static int HdfMotionInterfaceDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: driver bind start", __func__);
    auto *hdfMotionInterfaceHost = new (std::nothrow) HdfMotionInterfaceHost;
    if (hdfMotionInterfaceHost == nullptr) {
        HDF_LOGE("%{public}s: failed to create create HdfMotionInterfaceHost object", __func__);
        return HDF_FAILURE;
    }

    hdfMotionInterfaceHost->ioService.Dispatch = MotionInterfaceDriverDispatch;
    hdfMotionInterfaceHost->ioService.Open = NULL;
    hdfMotionInterfaceHost->ioService.Release = NULL;

    auto serviceImpl = OHOS::HDI::Motion::V1_1::IMotionInterface::Get(true);
    if (serviceImpl == nullptr) {
        HDF_LOGE("%{public}s: failed to get of implement service", __func__);
        delete hdfMotionInterfaceHost;
        return HDF_FAILURE;
    }

    hdfMotionInterfaceHost->stub = OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(serviceImpl,
        OHOS::HDI::Motion::V1_1::IMotionInterface::GetDescriptor());
    if (hdfMotionInterfaceHost->stub == nullptr) {
        HDF_LOGE("%{public}s: failed to get stub object", __func__);
        delete hdfMotionInterfaceHost;
        return HDF_FAILURE;
    }

    deviceObject->service = &hdfMotionInterfaceHost->ioService;
    return HDF_SUCCESS;
}

static void HdfMotionInterfaceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: driver release start", __func__);
    if (deviceObject->service == nullptr) {
        return;
    }

    auto *hdfMotionInterfaceHost = CONTAINER_OF(deviceObject->service, struct HdfMotionInterfaceHost, ioService);
    if (hdfMotionInterfaceHost != nullptr) {
        delete hdfMotionInterfaceHost;
    }
}

struct HdfDriverEntry g_motioninterfaceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "",
    .Bind = HdfMotionInterfaceDriverBind,
    .Init = HdfMotionInterfaceDriverInit,
    .Release = HdfMotionInterfaceDriverRelease,
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
HDF_INIT(g_motioninterfaceDriverEntry);
#ifdef __cplusplus
}
#endif /* __cplusplus */
