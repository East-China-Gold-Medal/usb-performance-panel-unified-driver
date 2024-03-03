/* @file libusb.c

    libUSB driver for many UNIX-like systems (even including early version of Windows).
    SPDX-License-Identifier: WTFPL

*/

#include <platform.h>
#include <libusb-1.0/libusb.h>
#include <stdio.h>

static struct libusb_device_handle *device_handle;

// Opens target MCU.
status_t open_device(void)
{
    const struct libusb_version* version;
    enum libusb_error libusb_status;

    version = libusb_get_version();
    printf("Using libUSB v%d.%d.%d.%d\n", version->major, version->minor, version->micro, version->nano);
    libusb_status = libusb_init_context(NULL, NULL, 0);
    if(libusb_status!=LIBUSB_SUCCESS) {
        fprintf(stderr,"Error initializing libUSB: 0x%s\n",libusb_error_name(libusb_status));
        return STATUS_DRIVER_INITIALIZATION_FAILED;
    }

    device_handle = libusb_open_device_with_vid_pid(NULL, PANEL_VID, PANEL_PID_CBC);
    if(device_handle == NULL) {
        // Let's try again!
        device_handle = libusb_open_device_with_vid_pid(NULL, PANEL_VID, PANEL_PID_HZ);
    }
    if(device_handle == NULL) {
        fprintf(stderr,"Device cannot be located.\n");
        return STATUS_DEVICE_NOT_FOUND;
    }

    libusb_status = libusb_claim_interface(device_handle, 0); // Default CONTROL endpoint.
    if(libusb_status!=LIBUSB_SUCCESS) {
        fprintf(stderr,"Cannot claim default CONTROL endpoint: 0x%s\n",libusb_error_name(libusb_status));
        return STATUS_DEVICE_CONFIGURATION_FAILED;
    }

    return STATUS_SUCCESS;
}

// On some platforms, the USB connection needs to be explicitly closed.
status_t close_device(void)
{
    libusb_release_interface(device_handle, 0);
    libusb_close(device_handle);
    return STATUS_SUCCESS;
}

// Transfer control to MCU and (Optionally) gets a return value on COMMAND_QUERY_CAP.
status_t transfer_control(IN host_operation_command_t command, IN uint16_t value, OUT OPTIONAL uint8_t* retbuf, size_t retbuflen)
{
    uint8_t request_type = LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_VENDOR|LIBUSB_RECIPIENT_DEVICE;
    int ret = libusb_control_transfer(device_handle, request_type, command, value, 0, retbuf, retbuflen, 5000);
    if(ret<0) {
        fprintf(stderr,"Cannot transter CONTROL: %s(0x%d)\n",libusb_error_name(ret),ret);
        return STATUS_DEVICE_CONFIGURATION_FAILED;
    }
    return STATUS_SUCCESS;
}
