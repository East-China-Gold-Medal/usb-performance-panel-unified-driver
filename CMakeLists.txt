# @file CMakeLists.txt
#
#   CMake config file for project.
#   SPDX-License-Identifier: WTFPL
#

cmake_minimum_required (VERSION 3.12)

include(FetchContent)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("usb-performance-panel-unified-driver")

set(DATA_COLLECTION_SOURCES datasource/collector.c)
# Data collection selection.
if(WIN32)
    message ("Use Windows data collection method.")
    add_definitions("-DUNICODE" "-D_UNICODE")
    list(APPEND DATA_COLLECTION_SOURCES
        datasource/windows/cpu.c
        datasource/windows/eventtimer.c
        datasource/windows/registryreader.c
    )
    if(${NVIDIA_PLEASE})
        add_definitions("-DNVIDIA_PLEASE")
        list(APPEND DATA_COLLECTION_SOURCES
            datasource/windows/gpu_nvidia.c
        )
    endif()
    add_compile_options("/Wall")
elseif(LINUX)
    message ("Use Linux data collection method.")
    list(APPEND DATA_COLLECTION_SOURCES
        datasource/linux/cpu.c
    )
    add_compile_options("-Wall")
else()
     message (FATAL_ERROR "Unsupported data collection method!")
endif()

# If user wants to get data from Nvidia GPU, include it as 3rd party.
if(${NVIDIA_PLEASE})
    message ("You begged from Nvidia, downloading NVAPI......(a long time)")
    if(DEFINED ${GITHUB_MIRROR})
        FetchContent_Declare(
            nvapi
            GIT_REPOSITORY https://${GITHUB_MIRROR}/NVIDIA/nvapi.git
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nvapi
            GIT_TAG origin/main
        )
    else()
        FetchContent_Declare(
            nvapi
            GIT_REPOSITORY https://github.com/NVIDIA/nvapi.git
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nvapi
            GIT_TAG origin/main
        )
    endif()
    FetchContent_MakeAvailable(nvapi)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nvapi)
    
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i[3-6]86")
        message ("Using 32-bit NVAPI lib")
        link_libraries (${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nvapi/x86/nvapi.lib)
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "(AMD|x86_)64")
        message ("Using 64-bit NVAPI lib")
        link_libraries (${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nvapi/amd64/nvapi64.lib)
    else()
        message (FATAL_ERROR "Unknown CPU arch ${CMAKE_HOST_SYSTEM_PROCESSOR}!")
    endif()

endif()

# Platform selection.
if(${BUILD_USER_MODE_DRIVER})
    message ("Building user-mode UMDF driver.")
    set(PLATFORM_SOURCES
        entry.c
    )
elseif(${BUILD_LIBUSB_PROGRAM})
    message ("Building user-mode libusb program.")
    set(PLATFORM_SOURCES
        entry.c
        platform/libusb.c
    )
elseif(${BUILD_WINUSB_PROGRAM})
    message ("Building user-mode WinUSB program.")
    set(PLATFORM_SOURCES
        entry.c
        platform/winusb.c
    )
    link_libraries (setupapi.lib winusb.lib)
else()
    message (FATAL_ERROR "Unsupported platform!")
endif()

add_executable (usb-performance-panel-unified-driver ${DATA_COLLECTION_SOURCES} ${PLATFORM_SOURCES})

target_include_directories (usb-performance-panel-unified-driver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET usb-performance-panel-unified-driver PROPERTY C_STANDARD 11)
endif()
