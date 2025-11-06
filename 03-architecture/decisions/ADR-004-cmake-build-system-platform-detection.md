---
specType: architecture
standard: 42010
phase: 03-architecture
version: 1.0.0
author: AI Agent
date: "2025-11-06"
status: accepted
traceability:
  requirements:
    - SYS-PORT-001
    - STR-CONST-001
---

# ADR-004: CMake Build System with Platform Detection

## Metadata
```yaml
adrId: ADR-004
status: accepted
relatedRequirements:
  - SYS-PORT-001 # Cross-platform compatibility requirements
  - STR-CONST-001 # Integration time constraints (<6 hours)
  - SYS-MAIN-001 # Maintainability and configuration requirements
  - SYS-CONST-001 # Platform abstraction constraints
relatedComponents:
  - ARC-C-009 # Build System Configuration
  - ARC-C-010 # Platform Detection System
  - ARC-C-011 # Cross-Compilation Support
supersedes: []
supersededBy: null
author: AI Agent
date: 2025-11-06
reviewers: []
```

## Context

The AES5-2018 library must support diverse build environments and deployment targets:

**Integration Requirements**:
- **Developer Experience**: <6 hour integration time per stakeholder requirements
- **Platform Support**: Linux, Windows, macOS, embedded RTOS (5+ platforms)
- **Toolchain Compatibility**: GCC, Clang, MSVC, ARM GCC, IAR, Keil compilers
- **IDE Integration**: Visual Studio, CLion, Qt Creator, Xcode, embedded IDEs

**Build Complexity Challenges**:
- **Cross-Compilation**: Desktop → embedded ARM targets
- **Conditional Compilation**: Platform-specific code inclusion/exclusion  
- **Dependency Management**: Optional dependencies, platform-specific libraries
- **Configuration Variants**: Debug/Release, embedded/desktop, feature toggles

**Developer Workflow Requirements**:
- **Simple Configuration**: Single command to configure build for any platform
- **IDE Integration**: Native project file generation for all major IDEs
- **Automated Testing**: Integrated test execution and coverage reporting
- **Documentation**: Automated API documentation generation

## Decision

We will implement **CMake-based build system with automatic platform detection**:

**Build System Architecture**:
```cmake
# Top-level CMakeLists.txt with platform detection
cmake_minimum_required(VERSION 3.16)
project(AES5_2018 VERSION 1.0.0 LANGUAGES CXX)

# Platform detection and configuration
include(cmake/PlatformDetection.cmake)
include(cmake/CompilerConfiguration.cmake)
include(cmake/DependencyManagement.cmake)

# Conditional platform-specific source inclusion
if(AES5_PLATFORM_LINUX)
    target_sources(aes5_library PRIVATE src/platform/linux/alsa_adapter.cpp)
elseif(AES5_PLATFORM_WINDOWS)
    target_sources(aes5_library PRIVATE src/platform/windows/wasapi_adapter.cpp)
elseif(AES5_PLATFORM_MACOS)
    target_sources(aes5_library PRIVATE src/platform/macos/coreaudio_adapter.cpp)
elseif(AES5_PLATFORM_EMBEDDED)
    target_sources(aes5_library PRIVATE src/platform/rtos/embedded_adapter.cpp)
endif()
```

**Platform Detection Strategy**:
- **Automatic Detection**: CMAKE_SYSTEM_NAME-based platform identification
- **Cross-Compilation Support**: CMAKE_CROSSCOMPILING detection and configuration
- **Capability Detection**: Compiler feature and library availability testing
- **Configuration Validation**: Platform compatibility verification

## Status

**Accepted** (2025-11-06) - CMake provides the best combination of cross-platform support, IDE integration, and developer productivity for the AES5-2018 library requirements.

## Rationale

CMake addresses the fundamental challenge of **complexity vs. usability** in cross-platform build systems:

**Universal Platform Support**: CMake has mature support for all target platforms (Linux, Windows, macOS, embedded), meeting the 5+ platform requirement.

**IDE Integration**: Native project file generation for all major IDEs eliminates manual configuration overhead, supporting <6 hour integration time.

**Cross-Compilation Excellence**: Built-in cross-compilation support for embedded targets with toolchain file management.

**Ecosystem Integration**: Excellent integration with package managers (vcpkg, Conan), testing frameworks (CTest), and documentation tools (Doxygen).

## Considered Alternatives

| Alternative | Summary | Pros | Cons | Reason Not Chosen |
|------------|---------|------|------|-------------------|
| **GNU Make/Autotools** | Traditional Unix build system | Universal on Unix, well-known | Windows support poor, complex configuration | Limited Windows/embedded support |
| **Bazel** | Google's scalable build system | Excellent for large codebases | Complex for library projects, learning curve | Over-engineered for single library project |
| **Meson** | Modern Python-based build system | Fast, clean syntax | Less embedded toolchain support | Insufficient embedded ecosystem |
| **Ninja + Custom Scripts** | Low-level build with custom configuration | Maximum control, fast builds | High maintenance burden | Violates <6 hour integration requirement |

## Consequences

### Positive
- **Universal Platform Support**: Single build configuration works on all 5+ target platforms
- **Excellent IDE Integration**: Native project generation for Visual Studio, CLion, Xcode, Qt Creator
- **Cross-Compilation Support**: Built-in toolchain management for embedded targets
- **Package Manager Integration**: Works with vcpkg, Conan, pkg-config for dependency management
- **Testing Integration**: CTest framework for automated test execution
- **Documentation Integration**: Doxygen integration for API documentation generation

### Negative / Liabilities
- **Learning Curve**: CMake syntax can be complex for advanced features (mitigation: comprehensive documentation)
- **Debug Difficulty**: CMake configuration debugging can be challenging (mitigation: verbose logging options)
- **Version Compatibility**: Different CMake versions may have compatibility issues (mitigation: minimum version specification)

### Neutral / Follow-ups
- Must maintain CMake configuration files for all target platforms
- Need comprehensive platform-specific testing in CI/CD pipeline
- Require CMake version compatibility verification across development environments
- Must provide platform-specific build documentation and examples

## Quality Attribute Impact Matrix

| Quality Attribute | Impact (+/−/0) | Notes |
|-------------------|----------------|-------|
| **Portability** | ++ | Excellent cross-platform support reduces porting effort |
| **Maintainability** | + | Single build configuration reduces maintenance burden |
| **Usability** | ++ | IDE integration and simple configuration improve developer experience |
| **Reliability** | + | Mature, well-tested build system with extensive community support |
| **Performance** | 0 | No direct impact on runtime performance |
| **Testability** | + | Integrated testing framework simplifies test automation |

## Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|-----------|
| **CMake Version Incompatibility** | Medium | Medium | Minimum version specification (3.16+), compatibility testing |
| **Platform-Specific Build Issues** | Medium | High | Automated CI/CD testing on all platforms, platform-specific documentation |
| **Embedded Toolchain Complexity** | High | Medium | Toolchain file templates, automated toolchain detection |
| **Dependency Resolution Issues** | Medium | Medium | Multiple package manager support, fallback mechanisms |

## Compliance Mapping

| Standard Clause | How Addressed |
|-----------------|---------------|
| **ISO 42010 §5.8 (Rationale)** | Build system selection rationale with cross-platform requirements |
| **IEEE 1016 (Implementation)** | Build and deployment strategy documentation |
| **Stakeholder Requirements** | <6 hour integration time through simplified build configuration |

## Implementation Notes

**Phase 1: Core Build Configuration**
```cmake
# Platform detection module (cmake/PlatformDetection.cmake)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(AES5_PLATFORM_LINUX TRUE)
    set(AES5_PLATFORM_NAME "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(AES5_PLATFORM_WINDOWS TRUE)
    set(AES5_PLATFORM_NAME "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(AES5_PLATFORM_MACOS TRUE)
    set(AES5_PLATFORM_NAME "macos")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Generic" OR CMAKE_CROSSCOMPILING)
    set(AES5_PLATFORM_EMBEDDED TRUE)
    set(AES5_PLATFORM_NAME "embedded")
endif()

# Compiler feature detection
target_compile_features(aes5_library PRIVATE cxx_std_17)
check_cxx_compiler_flag("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
```

**Phase 2: Cross-Compilation Support**
```cmake
# Toolchain file template for ARM embedded targets
# (cmake/toolchain/arm-none-eabi.cmake)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Platform-specific compilation flags
set(CMAKE_CXX_FLAGS_INIT "-mcpu=cortex-m4 -mthumb -fno-exceptions -fno-rtti")
```

**Phase 3: Integration and Testing**
```cmake
# Testing framework integration
enable_testing()
include(CTest)

# Add compliance tests
add_subdirectory(tests/compliance)
add_test(NAME AES5_Compliance_Test 
         COMMAND compliance_test_runner
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# Performance testing
add_test(NAME AES5_Performance_Test
         COMMAND performance_test_runner --benchmark
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# Cross-platform validation
if(AES5_ENABLE_CROSS_PLATFORM_TESTS)
    add_test(NAME Cross_Platform_Behavior_Test
             COMMAND cross_platform_validator
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
```

## Validation Plan

**Platform Compatibility Validation**:
1. Automated CI/CD builds on all target platforms (Linux, Windows, macOS)
2. Cross-compilation testing for embedded ARM targets
3. IDE project generation testing (Visual Studio, CLion, Xcode)
4. Package manager integration testing (vcpkg, Conan, pkg-config)

**Developer Experience Validation**:
1. Timed integration testing from clean environment to working build
2. Documentation completeness testing for build procedures
3. New developer onboarding time measurement
4. Build system troubleshooting and debugging validation

**Cross-Compilation Validation**:
1. Toolchain file testing for major embedded compilers
2. Cross-compilation output validation and testing
3. Embedded target deployment and execution testing
4. Resource constraint validation on target hardware

**Integration Validation**:
1. Continuous integration pipeline effectiveness testing
2. Automated testing framework validation
3. Documentation generation automation testing
4. Dependency management and resolution testing

## References

- System Requirements Specification (SYS-PORT-001, STR-CONST-001)
- CMake Documentation (cmake.org)
- "Professional CMake: A Practical Guide" - Craig Scott
- Embedded Systems Cross-Compilation Best Practices
- Audio Software Development Build System Analysis