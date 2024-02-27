/* @file registryreader.c

    Registry reader that loads data source binding.
    SPDX-License-Identifier: WTFPL

*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <datasource.h>
#include <tchar.h>

// Bound data source.
typedef enum {
    DATA_SOURCE_UNALLOCATED,
    DATA_SOURCE_CALIBRATION, // Always return 0xFF
    DATA_SOURCE_CPU_UTILITY,
    DATA_SOURCE_GPU_UTILITY,
    DATA_SOURCE_CPU_TEMPERATURE,
    DATA_SOURCE_GPU_TEMPERATURE,
    DATA_SOURCE_RAM_USAGE,

    DATA_SOURCE_MAX
} data_source_t;

// For calibration: always return 0xFF (MAX).
extern uint8_t data_source_calibration(void);
#ifdef NVIDIA_PLEASE
extern uint8_t nvidia_gpu_found;
extern uint8_t data_source_nvidia_gpu_utility(void);
extern uint8_t data_source_nvidia_gpu_temperature(void);
#endif

static HKEY registry_key;

// Get bound data from platform registration.
status_t get_bound_data_source(IN uint8_t channel, OUT data_source_collection_callback_t *callback, OUT uint8_t *calibration_val)
{

    LSTATUS status;
    DWORD key_data = 0;
    DWORD key_data_size = sizeof(key_data);
    TCHAR key_name_buffer[32];
    data_source_t data_source;

    if (!registry_key) {
        status = RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"Software\\East China Gold Medal\\Performance Panel", 0,
            NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &registry_key, NULL);
        if (status != ERROR_SUCCESS) { // Not in administrator mode. Re-open user config.
            status = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\East China Gold Medal\\Performance Panel", 0,
                NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &registry_key, NULL);
            if (status != ERROR_SUCCESS) {
                fprintf(stderr, "Cannot open registry key: 0x%lx\n", status);
                return STATUS_INVALID_CONFIGURATION;
            }
        }
    }

    _stprintf_s(key_name_buffer, 16, __TEXT("Channel%d"), channel);
    status = RegQueryValueEx(registry_key, key_name_buffer, 0, NULL, (VOID*)&key_data, &key_data_size);

    if (status != ERROR_SUCCESS) {
        // Cannot read, try to set.
        status = RegSetValueEx(registry_key, key_name_buffer, 0, REG_DWORD, (VOID*)&key_data, key_data_size);
        if (status != ERROR_SUCCESS) {
            fprintf(stderr, "Cannot get registry value: 0x%lx\n", status);
            return STATUS_INVALID_CONFIGURATION;
        }
    }
    data_source = (data_source_t)key_data;

    switch (data_source) {
        case DATA_SOURCE_CALIBRATION: {
            *callback = data_source_calibration;
            break;
        }
        case DATA_SOURCE_GPU_UTILITY: {
#ifdef NVIDIA_PLEASE
            if (nvidia_gpu_found) {
                *callback = data_source_nvidia_gpu_utility;
            }
#endif
            break;
        }
        case DATA_SOURCE_GPU_TEMPERATURE: {
#ifdef NVIDIA_PLEASE
            if (nvidia_gpu_found) {
                *callback = data_source_nvidia_gpu_temperature;
            }
#endif
            break;
        }
        default:;
    }

    _stprintf_s(key_name_buffer, 32, __TEXT("Calibration%d"), channel);

    // Calibration value defaults to 0x80 (128) to prevent voltmeter damage.
    // If user INTENTS to calibrate, set value to 0xFF.
    key_data = 0x80;
    if (data_source == DATA_SOURCE_CALIBRATION) {
        key_data = 0xFF;
    }
    status = RegQueryValueEx(registry_key, key_name_buffer, 0, NULL, (VOID*)&key_data, &key_data_size);
    if (status != ERROR_SUCCESS) {
        // Cannot read, try to set.
        status = RegSetValueEx(registry_key, key_name_buffer, 0, REG_DWORD, (VOID*)&key_data, key_data_size);
        if (status != ERROR_SUCCESS) {
            fprintf(stderr, "Cannot get registry value: 0x%lx\n", status);
            return STATUS_INVALID_CONFIGURATION;
        }
    }
    *calibration_val = (uint8_t)key_data;

    return STATUS_SUCCESS;
}
