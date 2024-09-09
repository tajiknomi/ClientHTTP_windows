cmake_minimum_required(VERSION 3.15)
project(clientHTTP)

# Set a common directory for all targets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)

# Add subdirectories for each project
add_subdirectory(clientHTTP)
add_subdirectory(curlFileTransfer)	# Its .dll file is named as filetransfer.dll
add_subdirectory(executeCommands)
add_subdirectory(filemanager)
# Add other DLL projects as needed

# Define dependencies if clientHTTP needs to know about the DLLs
add_dependencies(clientHTTP filetransfer executeCommands filemanager)
