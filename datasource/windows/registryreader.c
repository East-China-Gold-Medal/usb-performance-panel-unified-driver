/* @file registryreader.c

    Registry reader that loads data source binding.
    SPDX-License-Identifier: WTFPL

*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <datasource.h>

// Bound data source.
typedef enum {
    DATA_SOURCE_UNALLOCATED,
    DATA_SOURCE_CALIBRATION, // Always return 0xFF
    DATA_SOURCE_CPU_UTILITY,
    DATA_SOURCE_GPU_UTILITY,
    DATA_SOURCE_CPU_TEMPERATURE,
    DATA_SOURCE_GPU_TEMPERATURE,
    DATA_SOURCE_RAM_UTILITY,

    DATA_SOURCE_MAX
} data_source_t;

const char_t* data_source_names[] = {
    [DATA_SOURCE_UNALLOCATED] = __TEXT("Unallocated"),
    [DATA_SOURCE_CALIBRATION] = __TEXT("Calibration"),
    [DATA_SOURCE_CPU_UTILITY] = __TEXT("CPU Utility"),
    [DATA_SOURCE_GPU_UTILITY] = __TEXT("GPU Utility"),
    [DATA_SOURCE_RAM_UTILITY] = __TEXT("RAM Utility"),
    [DATA_SOURCE_CPU_TEMPERATURE] = __TEXT("CPU Temperature"),
    [DATA_SOURCE_GPU_TEMPERATURE] = __TEXT("GPU Temperature"),
};

// For calibration: always return 0xFF (MAX).
extern uint8_t data_source_calibration(void);
extern uint8_t data_source_cpu_utility(void);
extern uint8_t data_source_ram_utility(void);
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
    char_t key_name_buffer[32];
    char_t key_value_buffer[32];
    DWORD key_value_buffer_size = sizeof(key_value_buffer) / sizeof(char_t);
    DWORD key_calibration;
    DWORD key_calibration_size = sizeof(DWORD);

    if (!registry_key) {
        status = RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"Software\\East China Gold Medal\\Performance Panel", 0,
            NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &registry_key, NULL);
        if (status != ERROR_SUCCESS) { // Not in administrator mode. Re-open user config.
            status = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\East China Gold Medal\\Performance Panel", 0,
                NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &registry_key, NULL);
            if (status != ERROR_SUCCESS) {
                return STATUS_INVALID_CONFIGURATION;
            }
        }
    }

    _stprintf_s(key_name_buffer, 16, __TEXT("Channel%d"), channel);
    status = RegQueryValueEx(registry_key, key_name_buffer, 0, NULL, (VOID*)key_value_buffer, &key_value_buffer_size);

    if (status != ERROR_SUCCESS) {
        // Cannot read, try to set.
        _tcscpy_s(key_value_buffer, sizeof(key_value_buffer)/sizeof(char_t), data_source_names[DATA_SOURCE_UNALLOCATED]);
        status = RegSetValueEx(registry_key, key_name_buffer, 0, REG_SZ, (VOID*)&key_value_buffer, (DWORD)_tcslen(key_value_buffer)*sizeof(char_t));
        if (status != ERROR_SUCCESS) {
            return STATUS_INVALID_CONFIGURATION;
        }
    }

    if (!_tcscmp(key_value_buffer, data_source_names[DATA_SOURCE_CALIBRATION])) {
        *callback = data_source_calibration;
        goto data_source_finished;
    }

    if (!_tcscmp(key_value_buffer, data_source_names[DATA_SOURCE_CPU_UTILITY])) {
        *callback = data_source_cpu_utility;
        goto data_source_finished;
    }

    if (!_tcscmp(key_value_buffer, data_source_names[DATA_SOURCE_GPU_UTILITY])) {
        *callback = NULL;
#ifdef NVIDIA_PLEASE
        if (nvidia_gpu_found) {
            *callback = data_source_nvidia_gpu_utility;
        }
#endif
        goto data_source_finished;
    }

    if (!_tcscmp(key_value_buffer, data_source_names[DATA_SOURCE_GPU_TEMPERATURE])) {
        *callback = NULL;
#ifdef NVIDIA_PLEASE
        if (nvidia_gpu_found) {
            *callback = data_source_nvidia_gpu_temperature;
        }
#endif
        goto data_source_finished;
    }

    if (!_tcscmp(key_value_buffer, data_source_names[DATA_SOURCE_RAM_UTILITY])) {
        *callback = data_source_ram_utility;
        goto data_source_finished;
    }

data_source_finished:
    _stprintf_s(key_name_buffer, 32, __TEXT("Calibration%d"), channel);

    // Calibration value defaults to 0x80 (128) to prevent voltmeter damage.
    // If user INTENTS to calibrate, set value to 0xFF.
    key_calibration = 0x80;
    if (!_tcscmp(key_value_buffer, data_source_names[DATA_SOURCE_CALIBRATION])) {
        key_calibration = 0xFF;
    }
    status = RegQueryValueEx(registry_key, key_name_buffer, 0, NULL, (VOID*)&key_calibration, &key_calibration_size);
    if (status != ERROR_SUCCESS) {
        // Cannot read, try to set.
        status = RegSetValueEx(registry_key, key_name_buffer, 0, REG_DWORD, (VOID*)&key_calibration, sizeof(uint32_t));
        if (status != ERROR_SUCCESS) {
            return STATUS_INVALID_CONFIGURATION;
        }
    }
    *calibration_val = (uint8_t)key_calibration;

    return STATUS_SUCCESS;
}

// Get current binding name of a channel.
const char_t* get_source_binding_name(IN uint8_t channel);

// Get all supported source binding names.
void get_all_source_binding_names(OUT const char* binding_names, OUT size_t binding_name_count);

// Set a channel to a platform-defined source binding, classified by binding name.
status_t set_channel_source_binding(IN uint8_t channel, IN const char_t* binding_name);

// Clear a channel binding, mainly for calibration purposes.
status_t clear_binding(IN uint8_t channel);

// Set the calibration value of a given channel.
status_t set_channel_calibration(IN uint8_t channel, IN uint8_t calibration);