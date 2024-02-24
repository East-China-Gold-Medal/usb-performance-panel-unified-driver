/* @file common.h

    Definition of all user-mode programs' common values.
    SPDX-License-Identifier: WTFPL

*/
#ifndef ENTRY_H
#define ENTRY_H

#include <stdint.h>

#define IN
#define OUT
#define OPTIONAL

// USB Control command to send to MCU.
typedef const enum {
    COMMAND_QUERY_CAP = 0xFF,
    COMMAND_SET_USAGE = 0xFE
} host_operation_command_t;

// USB PID for currently supported products.
typedef const enum {
    PID_CBC = 0x0cbc, // CBC's USB performance panel
    PID_HZ = 0xcbc0   // HZ's "Enhanced" USB performance panel
} panel_pid_t;

// Capabilities reporting methodology returned from MCU.
typedef const enum {
    VOLTMETER_CHANNEL_REPORT_BITMAP = 0,
    VOLTMETER_CHANNEL_REPORT_RAW_VALUE = 0x80
} voltmeter_channel_report_method_t;

// Return statuses.
typedef const enum {
    STATUS_SUCCESS = 0,
    STATUS_DEVICE_NOT_FOUND = -1,
    STATUS_DEVICE_CONFIGURATION_FAILED = -2,
    STATUS_TRANSFER_FAILED = -3
} status_t;

#endif