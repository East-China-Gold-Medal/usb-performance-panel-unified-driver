/* @file datasource.h

    Definition of data source collection actions.
    SPDX-License-Identifier: WTFPL

*/

#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#include <common.h>

EXTERN_C_START

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
status_t send_usage(IN uint8_t channel, IN uint8_t calibrated_value);

// Get current binding name of a channel.
const char_t* get_source_binding_name(IN uint8_t channel);

// Get all supported source binding names.
status_t iterate_binding_names(OUT char_t* binding_name_buf, IN size_t binding_name_buf_char_count, IN int index);

// Set a channel to a platform-defined source binding, classified by binding name.
status_t set_channel_source_binding(IN uint8_t channel, IN const char_t* binding_name);

// Clear a channel binding, mainly for calibration purposes.
status_t clear_binding(IN uint8_t channel);

// Set the calibration value of a given channel.
status_t set_channel_calibration(IN uint8_t channel, IN uint8_t calibration);

EXTERN_C_END

#endif