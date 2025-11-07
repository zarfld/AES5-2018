# AES5-2018 Implementation

[![CI/CD](https://github.com/zarfld/AES5-2018/actions/workflows/ci-standards-compliance.yml/badge.svg)](https://github.com/zarfld/AES5-2018/actions)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](#-license)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Standards](https://img.shields.io/badge/Standards-IEEE%2FISO%2FIEC-green.svg)](docs/lifecycle-guide.md)

> **AES Recommended Practice for Professional Digital Audio**  
> Preferred sampling frequencies for applications employing pulse-code modulation

A production-ready, standards-compliant C++ library implementing **AES5-2018** sampling frequency validation, management, and conversion for professional digital audio applications. Developed following **IEEE/ISO/IEC software engineering standards** and **Extreme Programming (XP) practices** with comprehensive Test-Driven Development (TDD).

---

## 📋 Table of Contents

- [Features](#-features)
- [Quick Start](#-quick-start)
- [Installation](#-installation)
- [Usage](#-usage)
- [Project Structure](#-project-structure)
- [Standards Compliance](#-standards-compliance)
- [Development](#-development)
- [Testing](#-testing)
- [Documentation](#-documentation)
- [Contributing](#-contributing)
- [License](#-license)
- [Acknowledgments](#-acknowledgments)

---

## ✨ Features

### Core Capabilities
- ✅ **AES5-2018 Frequency Validation** - Validates sampling frequencies against AES5-2018 specifications
- ✅ **Multi-Rate Support** - Primary (48 kHz), consumer (44.1 kHz), high-bandwidth (96 kHz), and multiples up to 384 kHz
- ✅ **Rate Category Management** - Automatic classification into basic, double, quadruple, and octuple rates
- ✅ **Tolerance Checking** - Sub-50μs validation with configurable PPM tolerance
- ✅ **Pull-up/Pull-down Support** - Video synchronization variants (1001/1000 factor)
- ✅ **Legacy Compatibility** - Support for 32 kHz legacy frequencies

### Technical Highlights
- 🚀 **Real-Time Performance** - <50μs frequency validation latency
- 🔒 **Thread-Safe** - Lock-free concurrent operations
- 📦 **Zero Dependencies** - Header-only core, optional GoogleTest for testing
- 🎯 **Hardware Agnostic** - Pure standards implementation, platform adapters separate
- 🧪 **87 Unit Tests** - >80% code coverage with TDD methodology
- 📊 **Conformance Testing** - 24 AES5-2018 conformity test cases
- 🔧 **CMake Build System** - Cross-platform (Linux, macOS, Windows)

### Standards-Compliant Design
Following **ISO/IEC/IEEE 12207:2017** software lifecycle with:
- Requirements traceability (IEEE 29148:2018)
- Architecture documentation (IEEE 42010:2011)
- Design specifications (IEEE 1016-2009)
- Verification & validation (IEEE 1012-2016)

---

## 🚀 Quick Start

### Prerequisites
```bash
# Compiler: GCC 7.3+, Clang 6.0+, MSVC 2019+
# CMake 3.20+
# C++17 standard library
```

### Build and Test
```bash
git clone https://github.com/zarfld/AES5-2018.git
cd AES5-2018/05-implementation
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
ctest --output-on-failure
```

### Hello World Example
```cpp
#include "AES/AES5/2018/core/frequency_validation/frequency_validator.hpp"

using namespace AES::AES5::_2018::core;

int main() {
    // Create validator components
    auto compliance_engine = std::make_unique<compliance::ComplianceEngine>();
    auto validation_core = std::make_unique<validation::ValidationCore>();
    
    // Create frequency validator
    auto validator = frequency_validation::FrequencyValidator::create(
        std::move(compliance_engine), 
        std::move(validation_core));
    
    // Validate 48 kHz (primary frequency per AES5-2018 Section 5.1)
    auto result = validator->validate_frequency(48000);
    
    if (result.is_valid()) {
        std::cout << "✓ 48 kHz is valid (AES5-2018 compliant)\n";
        std::cout << "  Clause: " 
                  << static_cast<int>(result.applicable_clause) << "\n";
        std::cout << "  Tolerance: " << result.tolerance_ppm << " ppm\n";
    }
    
    return 0;
}
```

---

## 📦 Installation

### Option 1: CMake FetchContent (Recommended)
```cmake
include(FetchContent)
FetchContent_Declare(
    aes5_2018
    GIT_REPOSITORY https://github.com/zarfld/AES5-2018.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(aes5_2018)

target_link_libraries(your_target PRIVATE aes5_standards)
```

### Option 2: Git Submodule
```bash
git submodule add https://github.com/zarfld/AES5-2018.git external/aes5-2018
```

```cmake
add_subdirectory(external/aes5-2018/05-implementation)
target_link_libraries(your_target PRIVATE aes5_standards)
```

### Option 3: System Install
```bash
cd 05-implementation
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build .
sudo cmake --install .
```

---

## 💡 Usage

### Basic Frequency Validation
```cpp
// Validate standard frequencies
auto result_48k = validator->validate_frequency(48000);   // Primary (Section 5.1)
auto result_441k = validator->validate_frequency(44100);  // Consumer (Section 5.2)
auto result_96k = validator->validate_frequency(96000);   // High-bandwidth (Section 5.2)
auto result_192k = validator->validate_frequency(192000); // Quadruple rate (Section 5.2)

// Check validation status
assert(result_48k.is_valid());
assert(result_48k.detected_frequency == 48000);
assert(result_48k.applicable_clause == AES5Clause::Section_5_1);
```

### Tolerance-Based Validation
```cpp
// Validate with custom tolerance (±50 ppm)
auto result = validator->validate_frequency(48001, 50); // 48001 Hz ≈ 20.8 ppm from 48000

if (result.is_valid()) {
    std::cout << "Within tolerance: " << result.tolerance_ppm << " ppm\n";
} else {
    std::cout << "Out of tolerance\n";
    std::cout << "Closest standard: " << result.closest_standard_frequency << " Hz\n";
}
```

### Rate Category Classification
```cpp
auto rate_manager = rate_categories::RateCategoryManager::create();

// Classify frequencies by rate category
auto category_48k = rate_manager->classify_frequency(48000);
// Returns: RateCategory::Basic (31-54 kHz)

auto category_96k = rate_manager->classify_frequency(96000);
// Returns: RateCategory::Double (62-108 kHz)

auto category_192k = rate_manager->classify_frequency(192000);
// Returns: RateCategory::Quadruple (124-216 kHz)
```

### Legacy and Pull-up/Pull-down Support
```cpp
// Legacy 32 kHz support (Section 5.4)
auto legacy = validator->validate_frequency(32000);
assert(legacy.applicable_clause == AES5Clause::Section_5_4);

// Pull-up/pull-down for video sync (Annex A)
auto pullup = validator->validate_frequency(48048);    // 48000 * 1001/1000
auto pulldown = validator->validate_frequency(47952);  // 48000 * 1000/1001
assert(pullup.applicable_clause == AES5Clause::Annex_A);
```

### Performance Monitoring
```cpp
// Get validation metrics
auto& metrics = validation_core->get_metrics();
std::cout << "Total validations: " 
          << metrics.total_validations.load() << "\n";
std::cout << "Success rate: " 
          << (100.0 * metrics.successful_validations.load() / 
              metrics.total_validations.load()) << "%\n";
std::cout << "Avg latency: " 
          << metrics.average_latency_ns.load() << " ns\n";
```

---

## 📁 Project Structure

```
AES5-2018/
├── 01-stakeholder-requirements/    # IEEE 29148:2018 Stakeholder Requirements
│   ├── business-context/
│   └── stakeholders/
├── 02-requirements/                # IEEE 29148:2018 System Requirements
│   ├── functional/
│   ├── non-functional/
│   └── user-stories/
├── 03-architecture/                # IEEE 42010:2011 Architecture
│   ├── decisions/                  # Architecture Decision Records (ADRs)
│   ├── views/                      # 4+1 architectural views
│   └── architecture-quality-scenarios.md
├── 04-design/                      # IEEE 1016-2009 Design Specifications
│   ├── components/
│   └── interfaces/
├── 05-implementation/              # ISO/IEC/IEEE 12207:2017 Implementation
│   ├── src/lib/Standards/AES/AES5/2018/
│   │   ├── core/
│   │   │   ├── compliance/         # AES5-2018 compliance engine
│   │   │   ├── frequency_validation/  # Frequency validator
│   │   │   ├── rate_categories/    # Rate category manager
│   │   │   └── validation/         # Validation core
│   │   ├── conversion/             # Sample rate conversion
│   │   └── video_sync/             # Video synchronization
│   ├── tests/                      # TDD unit tests (87 tests)
│   └── CMakeLists.txt
├── 06-integration/                 # Integration tests
├── 07-verification-validation/     # IEEE 1012-2016 V&V
│   ├── test-cases/
│   └── traceability/
├── docs/                           # Project documentation
│   ├── lifecycle-guide.md
│   ├── xp-practices.md
│   └── spec-driven-development.md
├── spec-kit-templates/             # Spec-Kit markdown templates
└── README.md                       # This file
```

---

## 🎯 Standards Compliance

This implementation strictly adheres to:

### Audio Engineering Standards
- **AES5-2018** - Preferred sampling frequencies for PCM applications
- **AES3-2009** - Digital audio interface (related)
- **AES11-2009** - Digital audio reference signals (related)

### Software Engineering Standards
- **ISO/IEC/IEEE 12207:2017** - Software life cycle processes
- **ISO/IEC/IEEE 29148:2018** - Requirements engineering
- **IEEE 1016-2009** - Software design descriptions
- **ISO/IEC/IEEE 42010:2011** - Architecture description
- **IEEE 1012-2016** - Verification and validation

### Development Methodology
- **Extreme Programming (XP)** practices:
  - Test-Driven Development (TDD) - Red-Green-Refactor cycle
  - Continuous Integration - Multiple builds per day
  - Simple Design - YAGNI principle
  - Refactoring - Continuous code improvement

---

## 🛠️ Development

### Building from Source
```bash
# Debug build with coverage
cd 05-implementation
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
cmake --build .

# Run tests
ctest --output-on-failure

# Generate coverage report (requires gcovr)
gcovr -r .. --html --html-details -o coverage.html
```

### TDD Workflow
```bash
# Red-Green-Refactor cycle
cmake --build . --target tdd_watch    # Continuous test execution

# Run specific test suites
./compliance_engine_tests
./frequency_validator_tests
./rate_category_manager_tests
./aes5_2018_conformity_tests
./aes5_2018_architecture_tests
./aes5_2018_constraint_tests
```

### Code Quality Tools
```bash
# Static analysis
clang-tidy src/**/*.cpp -- -std=c++17

# Format checking
clang-format -style=file -i src/**/*.cpp

# Memory leak detection
valgrind --leak-check=full ./frequency_validator_tests
```

---

## 🧪 Testing

### Test Coverage
- **Total Tests**: 87 unit tests + 24 conformity tests
- **Code Coverage**: >80% line coverage, >90% branch coverage
- **Test Frameworks**: GoogleTest 1.14.0
- **CI/CD**: GitHub Actions (Linux, macOS, Windows)

### Test Categories
1. **Unit Tests** (`tests/unit/`)
   - Compliance Engine: 8 tests
   - Frequency Validator: 12 tests
   - Rate Category Manager: 10 tests
   - Validation Core: 10 tests

2. **Conformity Tests** (`tests/conformity/`)
   - 24 AES5-2018 specification compliance tests
   - Validates Section 5.1, 5.2, 5.4, and Annex A

3. **Architecture Tests** (`tests/`)
   - 15 architectural constraint tests (REQ-F-011, REQ-F-012, REQ-F-013)
   - Interface abstraction, platform adapters, conversion support

4. **Constraint Tests** (`tests/`)
   - 15 design constraint tests (REQ-C-001 through REQ-C-005)
   - Hardware abstraction, resource constraints, C++17 compliance

### Running Tests
```bash
# All tests
ctest --output-on-failure

# Specific test suite
./build/frequency_validator_tests --gtest_filter="*ToleranceCheck*"

# With verbose output
./build/aes5_2018_conformity_tests --gtest_color=yes
```

---

## 📚 Documentation

### User Documentation
- [Installation Guide](docs/installation.md) - Detailed setup instructions
- [API Reference](docs/api-reference.md) - Complete API documentation
- [Usage Examples](docs/examples/) - Real-world usage scenarios
- [FAQ](docs/faq.md) - Frequently asked questions

### Developer Documentation
- [Lifecycle Guide](docs/lifecycle-guide.md) - 9-phase SDLC process
- [XP Practices](docs/xp-practices.md) - TDD, CI, refactoring guidelines
- [Spec-Driven Development](docs/spec-driven-development.md) - Markdown specifications
- [Architecture Overview](03-architecture/views/README.md) - System architecture
- [Contributing Guide](CONTRIBUTING.md) - How to contribute

### Standards References
- [AES5-2018 Summary](docs/standards/aes5-2018-summary.md)
- [Traceability Matrix](07-verification-validation/traceability/architecture-traceability-matrix.md)
- [Conformance Checklist](standards-compliance/checklists/architecture-review-checklist.md)

---

## 🤝 Contributing

We welcome contributions! This project follows a strict standards-compliant development process.

### How to Contribute
1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Follow** the [9-phase lifecycle](docs/lifecycle-guide.md)
4. **Write** tests first (TDD)
5. **Ensure** all tests pass (`ctest`)
6. **Commit** with clear messages (`git commit -m 'Add amazing feature'`)
7. **Push** to your branch (`git push origin feature/amazing-feature`)
8. **Open** a Pull Request

### Development Guidelines
- ✅ Follow **C++17** standard (no compiler extensions)
- ✅ Maintain **>80% test coverage**
- ✅ Write tests **before** implementation (TDD)
- ✅ Update **traceability matrices** for requirements changes
- ✅ Document **architecture decisions** (ADRs)
- ✅ Follow **XP practices** (simple design, refactoring)
- ✅ Use **Spec-Kit** markdown templates for specifications

### Code Review Checklist
- [ ] All tests pass locally
- [ ] Code coverage maintained or improved
- [ ] Requirements traced in documentation
- [ ] Architecture decisions documented (if applicable)
- [ ] API documentation updated
- [ ] No compiler warnings
- [ ] Code formatted (clang-format)

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

### Copyright Notice
```
Copyright (c) 2025 AES5-2018 Implementation Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
...
```

### Standards Compliance Notice
This implementation is based on understanding of **AES5-2018** specifications. 
No copyrighted content from standards documents is reproduced. For authoritative 
requirements, refer to original specifications available from respective 
standards organizations (AES, IEEE, ISO/IEC).

---

## 🙏 Acknowledgments

- **Audio Engineering Society (AES)** for the AES5-2018 specification
- **IEEE** for software engineering standards (29148, 1016, 42010, 1012, 12207)
- **ISO/IEC** for international software lifecycle standards
- **Google Test** project for the excellent testing framework
- **Contributors** who follow rigorous standards-compliant development practices

### Built With
- [CMake](https://cmake.org/) - Cross-platform build system
- [GoogleTest](https://github.com/google/googletest) - C++ testing framework
- [C++17](https://en.cppreference.com/w/cpp/17) - Modern C++ standard

---

## 📞 Support & Contact

- **Issues**: [GitHub Issues](https://github.com/zarfld/AES5-2018/issues)
- **Discussions**: [GitHub Discussions](https://github.com/zarfld/AES5-2018/discussions)
- **CI/CD**: [GitHub Actions](https://github.com/zarfld/AES5-2018/actions)

### Project Status
- ✅ **Phase 05**: Implementation (TDD Complete)
- ✅ **Test Coverage**: 55.38% requirements coverage (36/65 requirements)
- ✅ **Scenario Coverage**: 69.23% scenario linkage (45/65 requirements)
- 🚀 **Next Phase**: Integration testing (Phase 06)

---

<div align="center">

**⭐ Star this repository if you find it useful!**

Made with ❤️ following IEEE/ISO/IEC standards and XP practices

[Documentation](docs/) • [Contributing](CONTRIBUTING.md) • [License](LICENSE) • [Changelog](CHANGELOG.md)

</div>
