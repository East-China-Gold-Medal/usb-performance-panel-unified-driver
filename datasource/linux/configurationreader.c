/* @file configurationreader.c

    Configuration reader that loads data source binding.
    SPDX-License-Identifier: WTFPL

*/

#include <datasource.h>
#include <stdio.h>
#include <json-c/json.h>
#include <sys/stat.h>
#include <string.h>

typedef enum {
    DATA_SOURCE_UNALLOCATED,
    DATA_SOURCE_CALIBRATION, // Always return 0xFF
    DATA_SOURCE_CPU_UTILITY,
    DATA_SOURCE_GPU_UTILITY,
    DATA_SOURCE_RAM_UTILITY,
    DATA_SOURCE_CPU_TEMPERATURE,
    DATA_SOURCE_GPU_TEMPERATURE,

    DATA_SOURCE_MAX
} data_source_t;

const char* const data_source_names[] = {
    [DATA_SOURCE_UNALLOCATED] = "Unallocated",
    [DATA_SOURCE_CALIBRATION] = "Calibration",
    [DATA_SOURCE_CPU_UTILITY] = "CPU Utility",
    [DATA_SOURCE_GPU_UTILITY] = "GPU Utility",
    [DATA_SOURCE_RAM_UTILITY] = "RAM Utility",
    [DATA_SOURCE_CPU_TEMPERATURE] = "CPU Temperature",
    [DATA_SOURCE_GPU_TEMPERATURE] = "GPU Temperature",
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
extern data_source_collection_callback_t* data_source_callbacks;
extern uint8_t* calibration_values;

const char* const config_file = "config.json";
static json_object *root_object;


static inline data_source_collection_callback_t name_string_to_callback(IN const char_t* name)
{
    if(!strcmp(name,data_source_names[DATA_SOURCE_CALIBRATION])) {
        return data_source_calibration;
    }
    // TODO: strcmp.
    return NULL;
}

static inline const char_t* callback_to_name_string(IN const data_source_collection_callback_t callback)
{
    if(callback == data_source_calibration) {
        return data_source_names[DATA_SOURCE_CALIBRATION];
    }
    // TODO: more channels.
    return data_source_names[DATA_SOURCE_UNALLOCATED];

}
// Get bound data from platform registration.
status_t get_bound_data_source(IN uint8_t channel, OUT data_source_collection_callback_t *callback, OUT uint8_t *calibration_val)
{

    char_t name_buf[MAX_CHANNEL_NAME_LENGTH];
    json_object* object;

    if(root_object == NULL) {
        struct stat config_stat;
        if(stat(config_file,&config_stat)) { // Not found!
            root_object = json_object_new_object();
        }
        else {
            root_object = json_object_from_file(config_file);
            if(root_object == NULL) {
                fprintf(stderr, "Cannot open load configuration!\n");
                return STATUS_INVALID_CONFIGURATION;
            }
        }
    }

    sprintf(name_buf,"channel%d",channel);
    if(json_object_object_get_ex(root_object,name_buf,&object)) {
        const char* data_source_name = json_object_get_string(object);
        //TODO: strcmp
        *callback = name_string_to_callback(data_source_name);
    }
    else {
        json_object* temp_object = json_object_new_string(data_source_names[DATA_SOURCE_UNALLOCATED]);
        json_object_object_add_ex(root_object, name_buf, temp_object ,0);
        // Flush to disk.
        if(json_object_to_file_ext(config_file,root_object,JSON_C_TO_STRING_PRETTY)) {
            fprintf(stderr, "Cannot save configuration!\n");
            return STATUS_INVALID_CONFIGURATION;
        }
    }

    sprintf(name_buf,"calibration%d",channel);
    if(json_object_object_get_ex(root_object,name_buf,&object)) {
        *calibration_val = (uint8_t)(json_object_get_int(object));
    }
    else {
        json_object* temp_object = json_object_new_int(0x80);
        json_object_object_add_ex(root_object, name_buf, temp_object ,0);
        // Flush to disk.
        if(json_object_to_file_ext(config_file,root_object,JSON_C_TO_STRING_PRETTY)) {
            fprintf(stderr, "Cannot save configuration!\n");
            return STATUS_INVALID_CONFIGURATION;
        }
        *calibration_val = 0x80;
    }
    return STATUS_SUCCESS;
}

// Get current binding name of a channel.
const char_t* get_source_binding_name(IN uint8_t channel)
{
    return callback_to_name_string(data_source_callbacks[channel]);
}

// Get all supported source binding names.
status_t iterate_binding_names(OUT char_t* binding_name_buf, IN size_t binding_name_buf_char_count, IN int index)
{
    if (index >= sizeof(data_source_names) / sizeof(char_t*)) {
        return STATUS_OVERFLOW;
    }
    if (strlen(data_source_names[index]) >= binding_name_buf_char_count) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    strcpy(binding_name_buf, data_source_names[index]);
    return (index == (sizeof(data_source_names) / sizeof(char_t*) - 1))?STATUS_SUCCESS: STATUS_HAS_NEXT;
}

// Set a channel to a platform-defined source binding, classified by binding name.
status_t set_channel_source_binding(IN uint8_t channel, IN const char_t* binding_name)
{
    char_t name_buf[MAX_CHANNEL_NAME_LENGTH];
    sprintf(name_buf,"channel%d",channel);
    json_object* object;
    if(json_object_object_get_ex(root_object,name_buf,&object)) {
        if(json_object_set_string(object,binding_name)) {
            if(!json_object_to_file_ext(config_file,root_object,JSON_C_TO_STRING_PRETTY)) {
                data_source_callbacks[channel] = name_string_to_callback(binding_name);
                return STATUS_SUCCESS;
            }
        }
    }
    fprintf(stderr,"Cannot set new channel mapping!\n");
    return STATUS_DEVICE_CONFIGURATION_FAILED;
}

// Clear a channel binding, mainly for calibration purposes.
status_t clear_binding(IN uint8_t channel)
{
    data_source_callbacks[channel] = NULL;
    return STATUS_SUCCESS;
}

// Set the calibration value of a given channel.
status_t set_channel_calibration(IN uint8_t channel, IN uint8_t calibration)
{
    char_t name_buf[MAX_CHANNEL_NAME_LENGTH];
    sprintf(name_buf,"calibration%d",channel);
    json_object* object;
    if(json_object_object_get_ex(root_object,name_buf,&object)) {
        if(json_object_set_int(object,calibration)) {
            if(!json_object_to_file_ext(config_file,root_object,JSON_C_TO_STRING_PRETTY)) {
                calibration_values[channel] = calibration;
                return STATUS_SUCCESS;
            }
        }
    }
    fprintf(stderr,"Cannot set new calibration value!\n");
    return STATUS_DEVICE_CONFIGURATION_FAILED;
}
