cmake_minimum_required(VERSION 3.15)
project(clientHTTP LANGUAGES CXX)

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
include_directories(${PROJECT_SOURCE_DIR}/../json)

# Source files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/../json/*.cpp")

# Header files
file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/Include/*.h")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link with required libraries
target_link_libraries(${PROJECT_NAME} Ws2_32.lib)

# Set Unicode character set
if (MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _UNICODE UNICODE)
endif()
