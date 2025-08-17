cmake_minimum_required(VERSION 3.10)
PROJECT(Rotcev
    VERSION 0.1.0
    DESCRIPTION "Rotcev - A Custom Vector Implementation"
    LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)


# Define custom build types with more options
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Profile;Testing;Development;Sanitize;FastDebug;MinSize" CACHE STRING "Available build types" FORCE)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "Profile" "Testing" "Development" "Sanitize" "FastDebug" "MinSize")
endif()

# Debug build configuration
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -Wall -Wextra -Werror")

# Release build configuration
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -DRELEASE")

# Profile build configuration
set(CMAKE_CXX_FLAGS_PROFILE "-O2 -g -pg -DPROFILE -fno-omit-frame-pointer")

# Testing build configuration
set(CMAKE_CXX_FLAGS_TESTING "-O1 -g -DTESTING -Wall -Wextra --coverage -fprofile-arcs -ftest-coverage")

# Development build configuration
set(CMAKE_CXX_FLAGS_DEVELOPMENT "-O1 -g -DDEVELOPMENT -Wall -Wextra -Wpedantic -Wconversion")

# Sanitize build configuration (AddressSanitizer + UBSan)
set(CMAKE_CXX_FLAGS_SANITIZE "-O1 -g -DSANITIZE -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS_SANITIZE "-fsanitize=address -fsanitize=undefined")

# Fast Debug build configuration
set(CMAKE_CXX_FLAGS_FASTDEBUG "-O1 -g -DFASTDEBUG -Wall")

# Minimum Size build configuration
set(CMAKE_CXX_FLAGS_MINSIZE "-Os -DMINSIZE -DNDEBUG -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZE "-Wl,--gc-sections")

# Print build configuration info
message(STATUS "=== BUILD CONFIGURATION ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")

# Print the flags being used
if(CMAKE_BUILD_TYPE)
    string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)
    message(STATUS "CXX flags for ${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${BUILD_TYPE_UPPER}}")
    
    # Also print linker flags if they exist
    if(CMAKE_EXE_LINKER_FLAGS_${BUILD_TYPE_UPPER})
        message(STATUS "Linker flags for ${CMAKE_BUILD_TYPE}: ${CMAKE_EXE_LINKER_FLAGS_${BUILD_TYPE_UPPER}}")
    endif()
else()
    message(WARNING "CMAKE_BUILD_TYPE is not set!")
endif()

# Print some additional useful info
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
message(STATUS "=== END CONFIGURATION ===")

include_directories(src)

# Create a header-only interface library instead of a compiled library
add_library(rotcev INTERFACE)

# Specify that this is a header-only library and set include directories
target_include_directories(rotcev INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Set C++17 requirement for users of the library
target_compile_features(rotcev INTERFACE cxx_std_17)

# Optional: Add compile definitions for users
target_compile_definitions(rotcev INTERFACE 
    $<$<CONFIG:Debug>:ROTCEV_DEBUG>
    $<$<CONFIG:Release>:ROTCEV_RELEASE>
)

# Create the example/test executable that uses the header-only library
add_executable(Rotcev_Profiling src/main.cpp src/logging_profiling.cpp)
target_link_libraries(Rotcev_Profiling PRIVATE rotcev)

# Create a custom target to copy headers to build output directory
add_custom_target(copy_headers ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/rotcev
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/src/rotcev.hpp
        ${CMAKE_BINARY_DIR}/include/rotcev/rotcev.hpp
    COMMENT "Copying rotcev.hpp to build output directory"
    SOURCES ${CMAKE_SOURCE_DIR}/src/rotcev.hpp
)

# Make sure headers are copied when building the executable
add_dependencies(Rotcev_Profiling copy_headers)

# Installation rules for header-only library
install(TARGETS rotcev
    EXPORT rotcev-targets
    INCLUDES DESTINATION include
)

install(FILES src/rotcev.hpp
    DESTINATION include/rotcev
)

install(EXPORT rotcev-targets
    FILE rotcev-config.cmake
    DESTINATION lib/cmake/rotcev
)

# Print target-specific info after target creation
get_target_property(TARGET_COMPILE_FLAGS rotcev COMPILE_FLAGS)
if(TARGET_COMPILE_FLAGS)
    message(STATUS "Target 'rotcev' compile flags: ${TARGET_COMPILE_FLAGS}")
endif()

message(STATUS "Library 'rotcev' configured successfully!")