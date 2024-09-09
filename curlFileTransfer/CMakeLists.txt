cmake_minimum_required(VERSION 3.15)
project(filetransfer LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Suppress C++17 deprecation warnings
if (MSVC)
    add_compile_options(/wd4996)  # Disable C4996 warning for deprecated functions
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/Include)

# Source files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Header files
file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/Include/*.h")

# Create the DLL target
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} "export/export.def")

# Link with required libraries
target_link_libraries(${PROJECT_NAME} Ws2_32.lib)

# Determine architecture and link appropriate libcurl library
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # 64-bit
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/libcurl_x64.lib)
else()  # 32-bit
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/libcurl_x86.lib)
endif()

# Set output DLL name
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "filetransfer")

# Set Unicode character set and define CURL_STATICLIB
if (MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _UNICODE UNICODE CURL_STATICLIB)
endif()
