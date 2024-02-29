/* @file datasource.h

    Definition of data source collection actions.
    SPDX-License-Identifier: WTFPL

*/

#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <common.h>

// Data source collection callback function prototype.
typedef uint8_t(*data_source_collection_callback_t)(void);

// Total channel count.
extern uint8_t channel_count;

// Get Device's maximum channel count.
uint8_t get_channel_count(void);

// Get bound data from platform registration.
status_t get_bound_data_source(IN uint8_t channel, OUT data_source_collection_callback_t* callback, OUT uint8_t* calibration_val);

// Initialize data source binding functions.
status_t initialize_data_source_binding(IN uint8_t total_channel_count);

// Send usage to device.
status_t send_usage(uint8_t channel, uint8_t calibrated_value);

#endif