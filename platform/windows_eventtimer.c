/* @file eventtimer.c

    Platform-dependent timer that calls main_loop_callback every period of time.
    SPDX-License-Identifier: WTFPL

*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <common.h>
#include <stdio.h>

extern status_t main_loop_callback(void);

static INT_PTR timer_id;

static void timer_proc(HWND unused1, UINT unused2, UINT_PTR unused3, DWORD unused4)
{
    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;
    main_loop_callback();
}

status_t enter_loop(void)
{
    timer_id = SetTimer(NULL, 0, LOOP_INTERVAL_MS, timer_proc);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return STATUS_SUCCESS;
}