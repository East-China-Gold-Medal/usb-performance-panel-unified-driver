/* @file platform.h

    Abstracted communication bridge to communicate between MCU & PC.
    SPDX-License-Identifier: WTFPL

*/
#ifndef PLATFORM_H
#define PLATFORM_H

#include <common.h>

// Opens target MCU.
void open_device(void);

// On some platforms, the USB connection needs to be explicitly closed.
void close_device(void);

// Transfer control to MCU and (Optionally) gets a return value.
uint8_t transfer_control(host_operation_command_t command, uint16_t value);

#endif