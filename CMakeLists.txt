cmake_minimum_required(VERSION 3.14)
project(AES5-2018-Library VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(AES5_BUILD_TESTS "Build AES5-2018 compliance tests" ON)
option(AES5_BUILD_EXAMPLES "Build usage examples" ON)
option(AES5_ENABLE_EMBEDDED "Enable embedded system optimizations" OFF)

# Compiler-specific flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(lib/Standards)

# AES5-2018 Core Library
add_library(aes5_standards STATIC
    # Core frequency validation (AES5-2018 Section 5.1)
    lib/Standards/AES/AES5/2018/core/frequency_validation/primary_frequency_validator.cpp
    lib/Standards/AES/AES5/2018/core/frequency_validation/other_frequency_validator.cpp
    lib/Standards/AES/AES5/2018/core/frequency_validation/legacy_frequency_validator.cpp
    
    # Rate category management (AES5-2018 Section 5.3)
    lib/Standards/AES/AES5/2018/core/rate_categories/basic_rate_manager.cpp
    lib/Standards/AES/AES5/2018/core/rate_categories/double_rate_manager.cpp
    lib/Standards/AES/AES5/2018/core/rate_categories/quadruple_rate_manager.cpp
    lib/Standards/AES/AES5/2018/core/rate_categories/octuple_rate_manager.cpp
    
    # Sampling frequency conversion (AES5-2018 Section 4.2)
    lib/Standards/AES/AES5/2018/conversion/integer_ratio/simple_ratio_converter.cpp
    lib/Standards/AES/AES5/2018/conversion/complex_ratio/complex_ratio_converter.cpp
    lib/Standards/AES/AES5/2018/conversion/anti_aliasing/bandwidth_limiting_filter.cpp
    
    # Video synchronization (AES5-2018 Annex A)
    lib/Standards/AES/AES5/2018/video_sync/integer_frames/integer_frame_sync.cpp
    lib/Standards/AES/AES5/2018/video_sync/ntsc_frames/ntsc_frame_sync.cpp
    lib/Standards/AES/AES5/2018/video_sync/pull_variants/pull_up_down_manager.cpp
    
    # Utilities
    lib/Standards/AES/AES5/2018/utilities/calculations/samples_per_frame_calculator.cpp
    lib/Standards/AES/AES5/2018/utilities/precision/high_precision_arithmetic.cpp
    
    # Common interfaces and utilities
    lib/Standards/Common/interfaces/audio_interface.cpp
    lib/Standards/Common/utils/crc_calculator.cpp
)

target_include_directories(aes5_standards PUBLIC
    lib/Standards/AES/AES5/2018
    lib/Standards/Common
)

# Embedded system optimizations
if(AES5_ENABLE_EMBEDDED)
    target_compile_definitions(aes5_standards PRIVATE AES5_EMBEDDED_OPTIMIZATIONS)
endif()

# Tests
if(AES5_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(AES5_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()