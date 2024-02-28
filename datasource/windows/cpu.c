/* @file cpu.c

    Windows CPU utilizaion getter. 
    SPDX-License-Identifier: WTFPL

*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <datasource.h>
#include <sysinfoapi.h>

static FILETIME last_idle_time, last_kernel_time, last_user_time;

static inline ULONGLONG CalculateDelta(FILETIME* first, FILETIME* last)
{
    ULONGLONG val1 = ((ULONGLONG)first->dwHighDateTime << 32) | (first->dwLowDateTime), val2 = ((ULONGLONG)last->dwHighDateTime << 32) | (last->dwLowDateTime);
    return val2 - val1;
}

uint8_t data_source_cpu_utility(void)
{
    FILETIME idle_time, kernel_time, user_time;

    if (GetSystemTimes(&idle_time, &kernel_time, &user_time)) {
        ULONGLONG idle_delta = CalculateDelta(&last_idle_time, &idle_time);
        ULONGLONG kernel_delta = CalculateDelta(&last_kernel_time, &kernel_time);
        ULONGLONG user_delta = CalculateDelta(&last_user_time, &user_time);
        uint8_t cpu_utility = ((UCHAR)((kernel_delta + user_delta - idle_delta) * 255.0 / (kernel_delta + user_delta)));
        last_idle_time = idle_time;
        last_kernel_time = kernel_time;
        last_user_time = user_time;
        return cpu_utility;
    }
    else {
        return 0;
    }
}