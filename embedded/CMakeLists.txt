cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Set global options.
set(CMAKE_CXX_STANDARD 14) # Use C++14.
set(CMAKE_CXX_STANDARD_REQUIRED OFF) # Actually gnu14 is adopted.
set(CMAKE_CXX_EXTENSIONS OFF)

# Project and target.
project(lint # This project is just for linting.
        LANGUAGES CXX
)
# Find source files.
file(GLOB_RECURSE all_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/app/*")
# Create target.
add_executable(${PROJECT_NAME} ${all_sources})
# Enable intellsense for mbed.
target_compile_definitions(${PROJECT_NAME} PUBLIC DOXYGEN_ONLY)

# Configure mbed.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mbed-os)
target_link_libraries(${PROJECT_NAME} PRIVATE mbed-os)
