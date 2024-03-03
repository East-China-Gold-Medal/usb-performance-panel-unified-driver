/* @file ram.c

    Windows RAM utilizaion getter.
    SPDX-License-Identifier: WTFPL

*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <datasource.h>
#include <sysinfoapi.h>

static MEMORYSTATUSEX memory_status = {
    .dwLength = sizeof(MEMORYSTATUSEX)
};

uint8_t data_source_ram_utility(void)
{
    DWORD val;
    if (GlobalMemoryStatusEx(&memory_status)) {
        val = memory_status.dwMemoryLoad * 256 / 100;
        return val > 256 ? 255 : (uint8_t)val;
    }
    return 0;
}