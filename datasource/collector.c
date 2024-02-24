/* @file collector.c

    Generic collector that re-routes data sources to desired channels.
    SPDX-License-Identifier: WTFPL

*/

#include <datasource.h>
#include <stdio.h>

uint8_t get_channel_count(void)
{
    uint8_t ret_val = 0;
    status_t status = transfer_control(COMMAND_QUERY_CAP, 0, &ret_val, 1);
    if (status == STATUS_SUCCESS) {
        printf("This device supports %d channels.\n",ret_val);
        return ret_val;
    }
    fprintf(stderr, "Cannot retrieve channal count:%x.\n", status);
    return 0;
}