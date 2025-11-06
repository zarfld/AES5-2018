# CMakeLists.txt for Phase 5 TDD Implementation
# AES5-2018 Standards-Compliant Audio Processing Library

cmake_minimum_required(VERSION 3.20)
project(AES5-2018 VERSION 1.0.0 LANGUAGES CXX)

# Modern C++ standard required for performance and safety
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Compiler flags for performance and debugging
if(MSVC)
    # Visual Studio flags for Windows development
    set(CMAKE_CXX_FLAGS_DEBUG "/DEBUG /Od /Zi /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Oi /DNDEBUG /MD")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    # GCC/Clang flags for Linux/macOS development
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
    
    # Additional warnings for code quality
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Enable testing framework
enable_testing()

# Find required packages
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    # Download and build Google Test if not found
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Include directories for the project
set(STANDARDS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/lib/Standards")
set(PLATFORM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/lib/Platform")
set(TESTS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# AES5-2018 Standards Library (Hardware-agnostic implementation)
add_library(aes5_standards STATIC
    # Core compliance and validation components (Phase 5.1)
    src/lib/Standards/AES/AES5/2018/core/compliance/compliance_engine.cpp
    
    # Additional components will be added in subsequent TDD cycles:
    # src/lib/Standards/AES/AES5/2018/core/validation/validation_core.cpp          # DES-C-005
    # src/lib/Standards/AES/AES5/2018/core/frequency_validation/frequency_validator.cpp  # DES-C-001
    # src/lib/Standards/AES/AES5/2018/core/rate_categories/rate_category_manager.cpp     # DES-C-003
    # src/lib/Standards/AES/AES5/2018/conversion/frequency_converter.cpp          # DES-C-002
)

# Public include directories for standards library
target_include_directories(aes5_standards PUBLIC
    ${STANDARDS_INCLUDE_DIR}
)

# Compiler-specific optimizations for standards library
target_compile_features(aes5_standards PUBLIC cxx_std_17)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Enable link-time optimization for release builds
    set_property(TARGET aes5_standards PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Test framework library (for mocking and test utilities)
add_library(aes5_test_framework STATIC
    # Mock implementations will be added as components are developed
    # tests/mocks/mock_compliance_engine.cpp
    # tests/mocks/mock_validation_core.cpp
    # tests/mocks/mock_audio_interface.cpp
    
    # Placeholder file to create valid library
    tests/framework/test_utilities.cpp
)

# Create placeholder test utilities file
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/tests/framework/test_utilities.cpp"
    "// Placeholder for test framework utilities\n"
    "// Will be expanded as TDD components are developed\n"
    "#include <gtest/gtest.h>\n"
    "\n"
    "namespace AES5TestFramework {\n"
    "    // Test utilities will be added here\n"
    "}\n"
)

target_include_directories(aes5_test_framework PUBLIC
    ${STANDARDS_INCLUDE_DIR}
    ${TESTS_INCLUDE_DIR}
)

target_link_libraries(aes5_test_framework PUBLIC
    aes5_standards
    gtest
    gtest_main
)

# Unit Tests - ComplianceEngine (DES-C-004)
add_executable(compliance_engine_tests
    tests/unit/Standards/AES/AES5/2018/core/test_compliance_engine.cpp
)

target_link_libraries(compliance_engine_tests PRIVATE
    aes5_standards
    aes5_test_framework
    gtest
    gtest_main
)

# Register ComplianceEngine tests with CTest
add_test(NAME ComplianceEngineUnitTests COMMAND compliance_engine_tests)

# Set test properties for better output
set_tests_properties(ComplianceEngineUnitTests PROPERTIES
    TIMEOUT 30
    ENVIRONMENT "GTEST_COLOR=1"
)

# Custom targets for TDD workflow

# Target: Run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS compliance_engine_tests
    COMMENT "Running all TDD tests"
)

# Target: Check test coverage (if gcovr is available)
find_program(GCOVR_PATH gcovr)
if(GCOVR_PATH AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(NOT MSVC)
        # Enable coverage flags for GCC/Clang
        target_compile_options(aes5_standards PRIVATE --coverage)
        target_link_options(aes5_standards PRIVATE --coverage)
        
        target_compile_options(compliance_engine_tests PRIVATE --coverage)
        target_link_options(compliance_engine_tests PRIVATE --coverage)
    endif()
    
    add_custom_target(coverage
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND ${GCOVR_PATH} --root ${CMAKE_SOURCE_DIR} --html --html-details 
                --exclude ${CMAKE_SOURCE_DIR}/tests 
                --output coverage_report.html
        DEPENDS compliance_engine_tests
        COMMENT "Generating test coverage report"
    )
endif()

# Target: Performance validation (basic timing checks)
add_custom_target(performance_check
    COMMAND compliance_engine_tests --gtest_filter="*Performance*"
    DEPENDS compliance_engine_tests
    COMMENT "Validating performance requirements"
)

# Target: TDD cycle helper - run tests continuously
add_custom_target(tdd_watch
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compliance_engine_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R ComplianceEngine
    COMMENT "TDD Watch: Build and test ComplianceEngine"
)

# Development convenience targets
add_custom_target(clean_build
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    COMMENT "Clean build all targets"
)

# Install configuration (for packaging later)
install(TARGETS aes5_standards
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/lib/Standards/
    DESTINATION include/
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# Summary information
message(STATUS "AES5-2018 TDD Build Configuration:")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Google Test: ${GTest_FOUND}")
message(STATUS "  Coverage Support: ${GCOVR_PATH}")

# TDD Instructions for developers
message(STATUS "")
message(STATUS "TDD Workflow Commands:")
message(STATUS "  Build: cmake --build .")
message(STATUS "  Test: cmake --build . --target run_tests")
message(STATUS "  TDD Cycle: cmake --build . --target tdd_watch")
if(GCOVR_PATH)
    message(STATUS "  Coverage: cmake --build . --target coverage")
endif()
message(STATUS "  Performance: cmake --build . --target performance_check")
message(STATUS "")