
cmake_minimum_required (VERSION 3.12)

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
    )
elseif(LINUX)
    message ("Use Linux data collection method.")
    list(APPEND DATA_COLLECTION_SOURCES
        datasource/linux/cpu.c
    )
else()
     message (FATAL_ERROR "Unsupported data collection method!")
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
