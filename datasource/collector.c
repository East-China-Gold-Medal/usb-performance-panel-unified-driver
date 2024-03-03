/* @file collector.c

    Generic collector that re-routes data sources to desired channels.
    SPDX-License-Identifier: WTFPL

*/

#include <datasource.h>
#include <platform.h>
#include <stdio.h>
#include <stdlib.h>

data_source_collection_callback_t* data_source_callbacks;
uint8_t* calibration_values;

#ifdef NVIDIA_PLEASE
extern void data_source_nvidia_prepare(void);
#endif

#ifdef AMD_CPU_PLEASE
extern void data_source_amd_cpu_prepare(void);
#endif

uint8_t get_channel_count(void)
{
    uint8_t ret_val = 0;
    status_t status = transfer_control(COMMAND_QUERY_CAP, 0, &ret_val, 1);
    if (ret_val & VOLTMETER_CHANNEL_REPORT_RAW_VALUE) {
        ret_val &= ~VOLTMETER_CHANNEL_REPORT_RAW_VALUE;
    }
    else { // Backward compatibility.
        uint8_t count = 0;
        while (ret_val & 1) {
            ++count;
        }
        ret_val = count;
    }
    if (status == STATUS_SUCCESS) {
        printf("This device supports %d channels.\n",ret_val);
        return ret_val;
    }
    fprintf(stderr, "Cannot retrieve channal count:%x.\n", status);
    return 0;
}

// Initialize data source binding functions.
status_t initialize_data_source_binding(IN uint8_t total_channel_count)
{

    status_t status = STATUS_SUCCESS;

    if (data_source_callbacks) {
        free(data_source_callbacks);
    }
    if (calibration_values) {
        free(calibration_values);
    }
    data_source_callbacks = calloc(1, sizeof(data_source_collection_callback_t)*total_channel_count);
    calibration_values = calloc(1, sizeof(uint8_t) * total_channel_count);
    if (!(data_source_callbacks) || !(calibration_values)) {
        return STATUS_DEVICE_CONFIGURATION_FAILED;
    }

#ifdef NVIDIA_PLEASE
    data_source_nvidia_prepare();
#endif
#ifdef AMD_CPU_PLEASE
    data_source_amd_cpu_prepare();
#endif

    for (uint8_t i = 0; i < total_channel_count; i++) {
        status = get_bound_data_source(i,&data_source_callbacks[i], &calibration_values[i]);
        if (status != STATUS_SUCCESS) {
            break;
        }
    }

    return status;
}

uint8_t data_source_calibration(void)
{
    return 0xFF;
}

status_t send_usage(uint8_t channel, uint8_t calibrated_value)
{
    return transfer_control(COMMAND_SET_USAGE, ((uint16_t)channel)<<8 | calibrated_value, NULL, 0);
}

status_t main_loop_callback(void)
{
    uint8_t value;
    status_t status;

    for (int i = 0; i < channel_count; i++) {
        if (data_source_callbacks[i] != NULL) {
            value = data_source_callbacks[i]();
            printf("Channel %d, value %d*%d\n",i,value,calibration_values[i]);
            status = send_usage((uint8_t)i, (uint8_t)((((uint32_t)value) * calibration_values[i])>>8));
            if (status != STATUS_SUCCESS) {
                return status;
            }
        }
    }
    return STATUS_SUCCESS;
}
