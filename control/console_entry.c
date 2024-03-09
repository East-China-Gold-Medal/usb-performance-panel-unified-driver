/* @file entry.c

    Implementation of all user-mode programs' entry point.
    SPDX-License-Identifier: WTFPL

*/

#include <common.h>
#include <stdio.h>
#include <platform.h>
#include <datasource.h>
#include <stdlib.h>

uint8_t channel_count;

int main(void)
{
    status_t status = STATUS_SUCCESS;
    open_device();
    channel_count = get_channel_count();
    status = initialize_data_source_binding(channel_count);
    enter_loop();
    close_device();
    free(data_source_callbacks);
    free(calibration_values);
    return 0;
}
