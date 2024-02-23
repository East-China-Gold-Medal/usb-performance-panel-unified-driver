/* @file entry.h

    Definition of all user-mode programs' entry point.
    SPDX-License-Identifier: WTFPL

*/
#ifndef ENTRY_H
#define ENTRY_H

#include <stdint.h>

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


#endif