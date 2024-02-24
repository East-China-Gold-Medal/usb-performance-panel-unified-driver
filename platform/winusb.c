/* @file winusb.c

    Communication bridge between MCU & PC via WinUSB.
    SPDX-License-Identifier: WTFPL

*/
#include <platform.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <setupapi.h>
#include <winusb.h>
#include <stdlib.h>
#include <stdio.h>

// Pre-defined GUID interface for WinUSB.
static GUID device_guid = {
    .Data1 = 0xC37CD10D,
    .Data2 = 0x3934,
    .Data3 = 0x49BE,
    .Data4 = { 0xBB, 0x2A, 0x31, 0x80, 0x33, 0xD2, 0x18, 0x7E }
};

static TCHAR *device_path;
static HANDLE device_file_handle;
static HANDLE winusb_file_handle;

// Opens target MCU.
status_t open_device(void)
{

    HDEVINFO device_info = SetupDiGetClassDevs (&device_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (device_info == INVALID_HANDLE_VALUE) {
        return STATUS_DEVICE_NOT_FOUND;
    }

    SP_DEVINFO_DATA device_info_data = {
        .cbSize = sizeof(SP_DEVINFO_DATA)
    };
    SP_DEVICE_INTERFACE_DATA device_interface_data = {
        .cbSize = sizeof(SP_INTERFACE_DEVICE_DATA)
    };
    SP_DEVICE_INTERFACE_DETAIL_DATA* device_interface_detail_data = malloc(sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA));
    if (device_interface_detail_data == NULL) {
        return STATUS_DEVICE_CONFIGURATION_FAILED;
    }
    device_interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    BOOL result = FALSE;
    int enum_index;
    for (enum_index = 0; SetupDiEnumDeviceInfo(device_info, enum_index, &device_info_data); ++enum_index) {

        //Get information about the device interface.
        result = SetupDiEnumDeviceInterfaces(device_info, &device_info_data,&device_guid, 0, &device_interface_data);
        if (GetLastError() == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (!result) {
            fprintf(stderr, "Error SetupDiEnumDeviceInterfaces: 0x%x.\n", GetLastError());
            return STATUS_DEVICE_CONFIGURATION_FAILED;
        }

        int required_length = device_interface_detail_data->cbSize;
        result = SetupDiGetDeviceInterfaceDetail(device_info, &device_interface_data, device_interface_detail_data, required_length, &required_length, &device_info_data);

        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            void* tmp = realloc(device_interface_detail_data, required_length);
            if (tmp == NULL) {
                return STATUS_DEVICE_CONFIGURATION_FAILED;
            }
            device_interface_detail_data = tmp;
            //device_interface_detail_data->cbSize = 0;  Why?
            result = SetupDiGetDeviceInterfaceDetail(device_info, &device_interface_data, device_interface_detail_data, required_length, NULL, &device_info_data);
        }

        if (!result) {
            fprintf(stderr, "Error SetupDiEnumDeviceInterfaces: 0x%x.\n", GetLastError());
            return STATUS_DEVICE_CONFIGURATION_FAILED;
        }

        size_t device_path_len = wcslen(device_interface_detail_data->DevicePath) + 1;
        if (device_path != NULL) {
            free(device_path);
        }
        device_path = malloc(device_path_len * sizeof(device_interface_detail_data->DevicePath[0]));
        if (device_path == NULL) {
            return STATUS_DEVICE_CONFIGURATION_FAILED;
        }
        wcscpy_s(device_path, device_path_len, device_interface_detail_data->DevicePath);
    }

    if (device_path == NULL) {
        fprintf(stderr, "Cannot get device path.\n");
        return STATUS_DEVICE_NOT_FOUND;
    }
#ifdef UNICODE
    wprintf_s(L"Selected Device path:%s\n", device_path);
#else
    printf_s(L"Selected Device path:%s\n", device_path);
#endif

    device_file_handle = CreateFile(device_path, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (device_file_handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Open device handle error:0x%x\n.", GetLastError());
    }

    WinUsb_Initialize(device_file_handle, &winusb_file_handle);
    if (winusb_file_handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Open WinUSB handle error:0x%x\n.", GetLastError());
    }

    free(device_interface_detail_data);
    return STATUS_SUCCESS;
}

// On some platforms, the USB connection needs to be explicitly closed.
status_t close_device(void)
{
    free(device_path);
    CloseHandle(device_file_handle);
    WinUsb_Free(winusb_file_handle);
    return STATUS_SUCCESS;
}

// Transfer control to MCU and (Optionally) gets a return value on COMMAND_QUERY_CAP.
status_t transfer_control(IN host_operation_command_t command, IN uint16_t value, OUT OPTIONAL uint8_t* retbuf, size_t retbuflen)
{
    WINUSB_SETUP_PACKET control_packet = {
        .RequestType = BMREQUEST_VENDOR | BMREQUEST_TO_ENDPOINT,
        .Request = command,
        .Value = value,
        .Length = 0
    };

    if (WinUsb_ControlTransfer(winusb_file_handle, control_packet, NULL, 0, NULL, NULL)) {
        return STATUS_SUCCESS;
    }
    return STATUS_TRANSFER_FAILED;
}
