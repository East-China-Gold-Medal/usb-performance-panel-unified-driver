/* @file configurationreader.c

    Configuration reader that loads data source binding.
    SPDX-License-Identifier: WTFPL

*/

#include <datasource.h>
#include <stdio.h>
#include <json-c/json.h>
#include <sys/stat.h>

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

const char* const config_file = "config.json";
static json_object *root_object;

// Get bound data from platform registration.
status_t get_bound_data_source(IN uint8_t channel, OUT data_source_collection_callback_t *callback, OUT uint8_t *calibration_val)
{

    char name_buf[32];
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
        *callback = data_source_calibration;
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
