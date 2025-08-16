cmake_minimum_required(VERSION 3.10)
PROJECT(Rotcev
    VERSION 0.1.0
    DESCRIPTION "Rotcev - A Custom Vector Implementation"
    LANGUAGES CXX)

PROJECT(Rotcev_Profiling
        VERSION 0.1.0
        DESCRIPTION "Rotcev Profiling Project"
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
add_library(rotcev SHARED src/rotcev.cpp)

add_executable(Rotcev_Profiling src/main.cpp src/logging_profiling.cpp)
target_link_libraries(Rotcev_Profiling PRIVATE rotcev)

# Print target-specific info after target creation
get_target_property(TARGET_COMPILE_FLAGS rotcev COMPILE_FLAGS)
if(TARGET_COMPILE_FLAGS)
    message(STATUS "Target 'rotcev' compile flags: ${TARGET_COMPILE_FLAGS}")
endif()

message(STATUS "Library 'rotcev' configured successfully!")