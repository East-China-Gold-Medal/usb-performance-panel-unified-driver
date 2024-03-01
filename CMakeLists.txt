# @file CMakeLists.txt
#
#   CMake config file for project.
#   SPDX-License-Identifier: WTFPL
#

cmake_minimum_required (VERSION 3.12)

include (FetchContent)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("usb-performance-panel-unified-driver")

set(DATA_COLLECTION_SOURCES datasource/collector.c)
# Data collection selection.
if(WIN32)
    message ("Use Windows data collection method.")
    add_definitions ("-DUNICODE" "-D_UNICODE")
    list (APPEND DATA_COLLECTION_SOURCES
        datasource/windows/cpu.c
        datasource/windows/registryreader.c
    )
    if(${NVIDIA_PLEASE})
        add_definitions ("-DNVIDIA_PLEASE")
        list(APPEND DATA_COLLECTION_SOURCES
            datasource/windows/gpu_nvidia.c
        )
    endif()
    if(${AMD_CPU_PLEASE})
        add_definitions ("-DAMD_CPU_PLEASE")
        list(APPEND DATA_COLLECTION_SOURCES
            datasource/windows/cpu_amd.c
        )
    endif()
    add_compile_options ("/W4")
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
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nvapi/nvapi.h)
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
    endif()
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/nvapi)
    
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

# if user wants to get CPU temperature from AMD CPU, detect AMD Ryzen Master SDK (Cannot download from Github because EULA)
if(${AMD_CPU_PLEASE})
    if(NOT CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "(AMD|x86_)64")
        message (FATAL_ERROR "AMD Ryzen Master SDK only supports 64-bit system!")
    endif()
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/RyzenMasterMonitoringSDK/include")
        message (FATAL_ERROR "You must download Ryzen Master SDK & copy RyzenMasterMonitoringSDK to 3rdparty!")
    endif()
    add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Platform.dll
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/RyzenMasterMonitoringSDK/bin/Platform.dll
            ${CMAKE_CURRENT_BINARY_DIR}/Platform.dll)
    add_custom_command (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Device.dll
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/RyzenMasterMonitoringSDK/bin/Device.dll
            ${CMAKE_CURRENT_BINARY_DIR}/Device.dll)
    link_libraries (${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/RyzenMasterMonitoringSDK/lib/Device.lib 
                    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/RyzenMasterMonitoringSDK/lib/Platform.lib)
endif()

# Platform selection.
if(${BUILD_USER_MODE_DRIVER})
    message ("Building user-mode UMDF driver.")
    add_definitions ("-DBUILDING_UMDF_DRIVER")
    set(PLATFORM_SOURCES
        "platform/umdf.c"
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
        platform/windows_eventtimer.c
    )
    link_libraries (setupapi.lib winusb.lib)
else()
    message (FATAL_ERROR "Unsupported platform!")
endif()

if(${BUILD_USER_MODE_DRIVER})

    set_property (GLOBAL PROPERTY VS_PLATFORM_TOOLSET "WindowsUserModeDriver10.0")
    get_filename_component (WINDOWS_10_SDK_PATH 
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" ABSOLUTE)
    if(${WINDOWS_10_SDK_PATH} MATCHES "/registry")
        message (FATAL_ERROR "Windows 10 SDK not found!")
    endif()
    message ("Windows 10 SDK path: ${WINDOWS_10_SDK_PATH}")
    
    if(NOT EXISTS "${WINDOWS_10_SDK_PATH}/Include/wdf/umdf/2.0")
        message (FATAL_ERROR "Windows DDK Not installed!")
    endif()

    include_directories ("${WINDOWS_10_SDK_PATH}/Include/wdf/umdf/2.0")
    link_libraries ("ntdll.lib")
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i[3-6]86")
        message ("Building 32-bit UMDF Driver")
        link_libraries ("${WINDOWS_10_SDK_PATH}/Lib/wdf/umdf/x86/2.0/WdfDriverStubUm.lib")
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "(AMD|x86_)64")
        message ("Building 64-bit UMDF Driver")
        link_libraries ("${WINDOWS_10_SDK_PATH}/Lib/wdf/umdf/x64/2.0/WdfDriverStubUm.lib")
    else()
        message (FATAL_ERROR "Unknown CPU arch ${CMAKE_HOST_SYSTEM_PROCESSOR}!")
    endif()
    add_library (UsbPerformancePanelUmdf2 SHARED ${DATA_COLLECTION_SOURCES} ${PLATFORM_SOURCES})

    cmake_host_system_information(RESULT WINDOWS_10_SDK_VERSION QUERY 
    WINDOWS_REGISTRY "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots"
    SUBKEYS)
    # Only pick the first installed SDK.
    list (GET WINDOWS_10_SDK_VERSION 0 WINDOWS_10_SDK_VERSION)
    message ("Windows 10 SDK version:${WINDOWS_10_SDK_VERSION}")
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "i[3-6]86")
        set (STAMPINF_ARCH X86)
        set (WINSDK_10_ARCH x86)
    elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "(AMD|x86_)64")
        set (STAMPINF_ARCH AMD64)
        set (WINSDK_10_ARCH x64)
    endif()
    set (WINDOWS_10_SDK_BIN_PATH "${WINDOWS_10_SDK_PATH}/bin/${WINDOWS_10_SDK_VERSION}/${WINSDK_10_ARCH}")

    # If there isn't signing cert, create one.
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/platform/UsbPerformancePanelUmdf2TestCert.cer)
        message ("Test certificate not found. Creating......")
        # pvk2pfx Bug: only accepts backslash     
        execute_process (
            COMMAND "${WINDOWS_10_SDK_BIN_PATH}/MakeCert.exe" 
            -r -pe -ss PrivateCertStore -n CN=EastChinaGoldMedalTest -eku 1.3.6.1.5.5.7.3.3
            "UsbPerformancePanelUmdf2TestCert.cer"
            -sv "UsbPerformancePanelUmdf2TestCert.pvk"
            ENCODING AUTO
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/platform
            COMMAND_ERROR_IS_FATAL ANY
        )
        execute_process (
            COMMAND "${WINDOWS_10_SDK_BIN_PATH}/pvk2pfx.exe"
            -pvk UsbPerformancePanelUmdf2TestCert.pvk
            -spc UsbPerformancePanelUmdf2TestCert.cer
            -pfx UsbPerformancePanelUmdf2TestCert.pfx
            ENCODING AUTO
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/platform
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()

    # Driver Sign.

    # Stampinf Bug: only accepts backslash     
    file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/UsbPerformancePanelUmdf2.inf" BACKSLASH_INF_PATH)
    add_custom_command (
        TARGET UsbPerformancePanelUmdf2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/UsbPerformancePanelUmdf2Template.inf"
        "${CMAKE_CURRENT_BINARY_DIR}/UsbPerformancePanelUmdf2.inf"
        COMMAND "${WINDOWS_10_SDK_BIN_PATH}/stampinf.exe" -d * -a ${STAMPINF_ARCH} -v * -u 2.0.0 -f
        "${BACKSLASH_INF_PATH}"
        VERBATIM
    )

    add_custom_command (
        TARGET UsbPerformancePanelUmdf2 POST_BUILD
        COMMAND ${WINDOWS_10_SDK_PATH}/bin/${WINDOWS_10_SDK_VERSION}/x86/Inf2cat.exe # Only in x86 folder!!!
        /driver:${CMAKE_CURRENT_BINARY_DIR} /os:8_${WINSDK_10_ARCH}
        COMMAND "${WINDOWS_10_SDK_BIN_PATH}/signtool.exe" sign /fd sha256 
        /f "${CMAKE_CURRENT_SOURCE_DIR}/platform/UsbPerformancePanelUmdf2TestCert.pfx"
        "${CMAKE_CURRENT_BINARY_DIR}/UsbPerformancePanelUmdf2.cat"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM
    )

else()
    add_executable (usb-performance-panel-unified-driver ${DATA_COLLECTION_SOURCES} ${PLATFORM_SOURCES})
endif()
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(GLOBAL PROPERTY C_STANDARD 11)
endif()
