/* @file unix_generic_timer.c

    Much more generic timer that calls main_loop_callback every period of time.
    SPDX-License-Identifier: WTFPL

*/

#include <common.h>
#include <time.h>
#include <unistd.h>

extern status_t main_loop_callback(void);

status_t enter_loop(void)
{
    status_t status;

    while(1) {
        status = main_loop_callback();
        if(status!=STATUS_SUCCESS) {
            return status;
        }
        sleep(1);
    }
    return status;
}
