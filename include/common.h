/* @file common.h

    Definition of all user-mode programs' common values.
    SPDX-License-Identifier: WTFPL

*/

#ifndef ENTRY_H
#define ENTRY_H

#include <stddef.h>
#include <stdint.h>

#define IN
#define OUT
#define OPTIONAL

#ifdef __cplusplus
#define EXTERN_C_START extern "C" {
#define EXTERN_C_END }

#else
#define EXTERN_C_START
#define EXTERN_C_END
#endif

// Loop interval, in ms.
#define LOOP_INTERVAL_MS 1000

// Currently-used USB VID.
#define PANEL_VID 0x0cbc

// USB Control command to send to MCU.
typedef const enum {
    COMMAND_QUERY_CAP = 0xFF,
    COMMAND_SET_USAGE = 0xFE
} host_operation_command_t;

// USB PID for currently supported products.
typedef const enum {
    PANEL_PID_CBC = 0x0cbc, // CBC's USB performance panel
    PANEL_PID_HZ = 0xcbc0   // HZ's "Enhanced" USB performance panel
} panel_pid_t;

// Capabilities reporting methodology returned from MCU.
typedef const enum {
    VOLTMETER_CHANNEL_REPORT_BITMAP = 0,
    VOLTMETER_CHANNEL_REPORT_RAW_VALUE = 0x80
} voltmeter_channel_report_method_t;

// Return statuses.
typedef enum {
    STATUS_SUCCESS = 0,
    STATUS_DEVICE_NOT_FOUND = -1,
    STATUS_DEVICE_CONFIGURATION_FAILED = -2,
    STATUS_TRANSFER_FAILED = -3,
    STATUS_INVALID_CONFIGURATION = -4,
    STATUS_DRIVER_INITIALIZATION_FAILED = -5
} status_t;

// Platform native char type.
// TCHAR on Windows, char on other.
#ifdef _WIN32
#include <tchar.h>
    typedef TCHAR char_t;
#else
    typedef char char_t;
#endif
#endif
