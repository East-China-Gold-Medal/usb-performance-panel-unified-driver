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

    if (!data_source_callbacks) {
        free(data_source_callbacks);
    }
    if (!calibration_values) {
        free(calibration_values);
    }
    data_source_callbacks = malloc(sizeof(data_source_collection_callback_t)*total_channel_count);
    calibration_values = malloc(sizeof(uint8_t) * total_channel_count);
    if (!(data_source_callbacks) || !(calibration_values)) {
        return STATUS_DEVICE_CONFIGURATION_FAILED;
    }

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
