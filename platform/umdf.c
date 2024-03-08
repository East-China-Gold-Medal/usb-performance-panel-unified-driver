/* @file

    Communication bridge between MCU & PC via UMDF2 USB.
    SPDX-License-Identifier: WTFPL

*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <common.h>
#include <datasource.h>
#include <platform.h>
#include <wdf.h>
#include <usb.h>
#include <wdfusb.h>
#include <initguid.h>
#include "windows_trace.h"
#include <umdf.c.tmh>  //Generated

extern const __declspec(selectany) LONGLONG DEFAULT_CONTROL_TRANSFER_TIMEOUT = 5 * -1 * WDF_TIMEOUT_TO_SEC;

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD UsbPerformancePanelEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP UsbPerformancePanelEvtDriverContextCleanup;
NTSTATUS UsbPerformancePanelCreateDevice(_Inout_ PWDFDEVICE_INIT DeviceInit);
EVT_WDF_DEVICE_PREPARE_HARDWARE UsbPerformancePanelEvtDevicePrepareHardware;
extern status_t main_loop_callback(void);

//
// Device Interface GUID
// 6da0b5d7-b496-4790-9227-c009c96eba6e
//

static WDFUSBDEVICE UsbDevice;
static WDFTIMER Timer;
static WDFDEVICE Device;
uint8_t channel_count;

NTSTATUS UsbPerformancePanelCreateDevice(_Inout_ PWDFDEVICE_INIT DeviceInit)
{
    WDF_PNPPOWER_EVENT_CALLBACKS pnpPowerCallbacks;
    NTSTATUS status;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
    pnpPowerCallbacks.EvtDevicePrepareHardware = UsbPerformancePanelEvtDevicePrepareHardware;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);
    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &Device);
    if (NT_SUCCESS(status)) {
        status = WdfDeviceCreateDeviceInterface(Device,&GUID_DEVINTERFACE_UsbPerformancePanel,NULL);
    }
    else {
        TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Fail");
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
    return status;
}

NTSTATUS UsbPerformancePanelEvtDevicePrepareHardware(
    _In_ WDFDEVICE WdfDevice,_In_ WDFCMRESLIST ResourceList, _In_ WDFCMRESLIST ResourceListTranslated)
{
    NTSTATUS status;
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS configParams;

    UNREFERENCED_PARAMETER(ResourceList);
    UNREFERENCED_PARAMETER(ResourceListTranslated);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = STATUS_SUCCESS;
    if (UsbDevice == NULL) {
        status = WdfUsbTargetDeviceCreate(Device,
            WDF_NO_OBJECT_ATTRIBUTES,
            &UsbDevice
        );
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE,
                "WdfUsbTargetDeviceCreateWithParameters failed 0x%x", status);
            return status;
        }
    }
    WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_MULTIPLE_INTERFACES(&configParams,
        0,
        NULL
    );
    status = WdfUsbTargetDeviceSelectConfig(UsbDevice,
        WDF_NO_OBJECT_ATTRIBUTES,
        &configParams
    );
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE,
            "WdfUsbTargetDeviceSelectConfig failed 0x%x", status);
        return status;
    }
    
    channel_count = get_channel_count();
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE,
        "This device supports %d channels", channel_count);
    if (initialize_data_source_binding(channel_count) != STATUS_SUCCESS) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE,
            "initialize_data_source_binding failed");
    }
    Device = WdfDevice;
    enter_loop();
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
    return status;
}
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

    // Initialize WPP Tracing
    WPP_INIT_TRACING(DriverObject, RegistryPath);
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = UsbPerformancePanelEvtDriverContextCleanup;

    WDF_DRIVER_CONFIG_INIT(&config, UsbPerformancePanelEvtDeviceAdd);

    status = WdfDriverCreate(DriverObject, RegistryPath, &attributes, &config, WDF_NO_HANDLE);
 
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
        WPP_CLEANUP(DriverObject);
        return status;
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
    return status;
}

NTSTATUS UsbPerformancePanelEvtDeviceAdd(_In_ WDFDRIVER Driver, _Inout_ PWDFDEVICE_INIT DeviceInit)
{
    NTSTATUS status;
    UNREFERENCED_PARAMETER(Driver);
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");
    status = UsbPerformancePanelCreateDevice(DeviceInit);
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
    return status;
}

VOID UsbPerformancePanelEvtDriverContextCleanup(_In_ WDFOBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");
    if (Timer != NULL) {
        WdfTimerStop(Timer, TRUE);
    }
    for (int i = 0; i < channel_count; i++) {
        send_usage(i, 0);
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
    WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)DriverObject));
}

_Function_class_(EVT_WDF_TIMER)
VOID MonitorThreadFunction(WDFTIMER Unused)
{
    UNREFERENCED_PARAMETER(Unused);
    main_loop_callback();
}

status_t enter_loop(void)
{
    WDF_TIMER_CONFIG  timerConfig;
    WDF_OBJECT_ATTRIBUTES  timerAttributes;
    NTSTATUS  status;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");
    WDF_TIMER_CONFIG_INIT_PERIODIC(&timerConfig, MonitorThreadFunction, LOOP_INTERVAL_MS);
    timerConfig.TolerableDelay = TolerableDelayUnlimited;
    WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);
    timerAttributes.ParentObject = Device;
    status = WdfTimerCreate(&timerConfig, &timerAttributes, &Timer);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfTimerCreate failed %!STATUS!", status);
        Timer = NULL;
        return STATUS_DRIVER_INITIALIZATION_FAILED;
    }
    else {
        WdfTimerStart(Timer, WDF_REL_TIMEOUT_IN_MS(LOOP_INTERVAL_MS));
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
    return STATUS_SUCCESS;
}

_IRQL_requires_(PASSIVE_LEVEL)
status_t transfer_control(IN host_operation_command_t command, IN uint16_t value, OUT OPTIONAL uint8_t* retbuf, size_t retbuflen)
{
    NTSTATUS status = 0;
    WDF_USB_CONTROL_SETUP_PACKET    controlSetupPacket;
    WDF_REQUEST_SEND_OPTIONS        sendOptions;
    WDF_MEMORY_DESCRIPTOR  memoryDescriptor;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_VERBOSE, TRACE_DEVICE, "-->TransportUsage\n");
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memoryDescriptor, retbuf, (ULONG)retbuflen);
    WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_TIMEOUT);
    WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&sendOptions, DEFAULT_CONTROL_TRANSFER_TIMEOUT);
    WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket, BmRequestDeviceToHost, BmRequestToDevice, command, value, 0);
    status = WdfUsbTargetDeviceSendControlTransferSynchronously(UsbDevice, NULL, &sendOptions, &controlSetupPacket, &memoryDescriptor, NULL);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE,
            "Failed to set Usage - 0x%x \n", status);
        return STATUS_TRANSFER_FAILED;
    }
    else {
        TraceEvents(TRACE_LEVEL_VERBOSE, TRACE_DEVICE,
            "Set Usage: Success\n");
        return STATUS_SUCCESS;
    }
}
