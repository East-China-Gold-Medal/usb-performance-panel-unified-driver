/* @file gpu_nvidia.c

    NVIDIA GPU utilizaion/temperature getter.
    SPDX-License-Identifier: WTFPL

*/

#include <datasource.h>
#include <nvapi.h>
#include <stdio.h>

static NvPhysicalGpuHandle phys;
uint8_t nvidia_gpu_found;

uint8_t data_source_nvidia_gpu_utility(void)
{

    NvAPI_Status ret;
    NV_GPU_DYNAMIC_PSTATES_INFO_EX pstate_info = {
        .version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER
    };
    ret = NvAPI_GPU_GetDynamicPstatesInfoEx(phys, &pstate_info);
    if (!ret == NVAPI_OK) {
        fprintf_s(stderr, "Cannot get temperature from Nvidia GPU.\n");
        return 0;
    }

    if (pstate_info.utilization->percentage == 100) {
        pstate_info.utilization->percentage--;  // Overflow!
    }
    return (uint8_t)(pstate_info.utilization->percentage * 256 / 100);
}

uint8_t data_source_nvidia_gpu_temperature(void)
{

    NvAPI_Status ret;
    NV_GPU_THERMAL_SETTINGS thermal = {
        .version = NV_GPU_THERMAL_SETTINGS_VER
    };

    ret = NvAPI_GPU_GetThermalSettings(phys, 0, &thermal);
    if (!ret == NVAPI_OK) {
        fprintf_s(stderr, "Cannot get temperature from Nvidia GPU.\n");
        return 0;
    }

    if (thermal.sensor[0].currentTemp >= 100) {
        thermal.sensor[0].currentTemp = 99;  // Overflow!
    }
    return (uint8_t)(thermal.sensor[0].currentTemp * 256 / 100);

}

void data_source_nvidia_prepare(void)
{

    NvAPI_Status ret = NVAPI_OK;
    
    ret = NvAPI_Initialize();
    if (!ret == NVAPI_OK) {
        fprintf_s(stderr,"Not a Nvidia GPU.\n");
        return;
    }

    NvU32 cnt;
    ret = NvAPI_EnumPhysicalGPUs(&phys, &cnt);
    if (!ret == NVAPI_OK) {
        fprintf_s(stderr, "Cannot find any Nvidia GPUs.\n");
        return;
    }

    NvAPI_ShortString name;
    ret = NvAPI_GPU_GetFullName(phys, name);
    if (!ret == NVAPI_OK){
        fprintf_s(stderr, "Cannot get GPU name.\n");
        return;
    }

    printf("Selected GPU Name:%s\n", name);
    nvidia_gpu_found = 1;

}
