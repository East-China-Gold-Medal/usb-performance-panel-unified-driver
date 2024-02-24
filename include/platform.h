/* @file platform.h

    Abstracted communication bridge to communicate between MCU & PC.
    SPDX-License-Identifier: WTFPL

*/
#ifndef PLATFORM_H
#define PLATFORM_H

#include <common.h>

// Opens target MCU.
status_t open_device(void);

// On some platforms, the USB connection needs to be explicitly closed.
status_t close_device(void);

// Transfer control to MCU and (Optionally) gets a return value on COMMAND_QUERY_CAP.
status_t transfer_control(IN host_operation_command_t command, IN uint16_t value, OUT OPTIONAL uint8_t *retbuf, size_t retbuflen);

#endif