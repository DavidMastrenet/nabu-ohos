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

#include <hdf_base.h>
#include <hdf_core_log.h>
#include <hdf_device_desc.h>
#include <hdf_sbuf_ipc.h>
#include "v3_0/sensor_interface_stub.h"

#define HDF_LOG_TAG    sensor_interface_driver

using namespace OHOS::HDI::Sensor::V3_0;

struct HdfSensorInterfaceHost {
    struct IDeviceIoService ioService;
    OHOS::sptr<OHOS::IRemoteObject> stub;
};

static int32_t SensorInterfaceDriverDispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    auto *hdfSensorInterfaceHost = CONTAINER_OF(client->device->service, struct HdfSensorInterfaceHost, ioService);

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

    return hdfSensorInterfaceHost->stub->SendRequest(cmdId, *dataParcel, *replyParcel, option);
}

static int HdfSensorInterfaceDriverInit(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: driver init start", __func__);
    return HDF_SUCCESS;
}

static int HdfSensorInterfaceDriverBind(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: driver bind start", __func__);
    auto *hdfSensorInterfaceHost = new (std::nothrow) HdfSensorInterfaceHost;
    if (hdfSensorInterfaceHost == nullptr) {
        HDF_LOGE("%{public}s: failed to create create HdfSensorInterfaceHost object", __func__);
        return HDF_FAILURE;
    }

    hdfSensorInterfaceHost->ioService.Dispatch = SensorInterfaceDriverDispatch;
    hdfSensorInterfaceHost->ioService.Open = NULL;
    hdfSensorInterfaceHost->ioService.Release = NULL;

    auto serviceImpl = OHOS::HDI::Sensor::V3_0::ISensorInterface::Get(true);
    if (serviceImpl == nullptr) {
        HDF_LOGE("%{public}s: failed to get of implement service", __func__);
        delete hdfSensorInterfaceHost;
        return HDF_FAILURE;
    }

    hdfSensorInterfaceHost->stub = OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(serviceImpl,
        OHOS::HDI::Sensor::V3_0::ISensorInterface::GetDescriptor());
    if (hdfSensorInterfaceHost->stub == nullptr) {
        HDF_LOGE("%{public}s: failed to get stub object", __func__);
        delete hdfSensorInterfaceHost;
        return HDF_FAILURE;
    }

    deviceObject->service = &hdfSensorInterfaceHost->ioService;
    return HDF_SUCCESS;
}

static void HdfSensorInterfaceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    HDF_LOGI("%{public}s: driver release start", __func__);
    if (deviceObject->service == nullptr) {
        return;
    }

    auto *hdfSensorInterfaceHost = CONTAINER_OF(deviceObject->service, struct HdfSensorInterfaceHost, ioService);
    if (hdfSensorInterfaceHost != nullptr) {
        delete hdfSensorInterfaceHost;
    }
}

struct HdfDriverEntry g_sensorinterfaceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "",
    .Bind = HdfSensorInterfaceDriverBind,
    .Init = HdfSensorInterfaceDriverInit,
    .Release = HdfSensorInterfaceDriverRelease,
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
HDF_INIT(g_sensorinterfaceDriverEntry);
#ifdef __cplusplus
}
#endif /* __cplusplus */
