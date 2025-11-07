# Contributing to AES5-2018

Thank you for considering contributing to the AES5-2018 Implementation project! This document provides guidelines for contributing to this standards-compliant audio processing library.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Process](#development-process)
- [Standards Compliance](#standards-compliance)
- [Coding Guidelines](#coding-guidelines)
- [Testing Requirements](#testing-requirements)
- [Pull Request Process](#pull-request-process)
- [Documentation](#documentation)

---

## Code of Conduct

This project adheres to professional software engineering standards and expects all contributors to maintain respectful, constructive communication. We follow these principles:

- **Be respectful** - Value diverse perspectives and experience levels
- **Be constructive** - Focus on improving code quality and standards compliance
- **Be collaborative** - Work together to achieve excellence in software engineering
- **Be professional** - Maintain high standards in code, documentation, and communication

---

## Getting Started

### Prerequisites

Before contributing, ensure you have:

1. **Required Tools**:
   - C++ compiler (GCC 7.3+, Clang 6.0+, or MSVC 2019+)
   - CMake 3.20+
   - Git 2.20+
   - GoogleTest (automatically fetched by CMake)

2. **Knowledge Requirements**:
   - C++17 standard
   - Test-Driven Development (TDD)
   - IEEE/ISO/IEC software engineering standards
   - AES5-2018 specification (for audio-specific contributions)

### Fork and Clone

```bash
# Fork the repository on GitHub
# Clone your fork
git clone https://github.com/YOUR_USERNAME/AES5-2018.git
cd AES5-2018

# Add upstream remote
git remote add upstream https://github.com/zarfld/AES5-2018.git
```

---

## Development Process

This project follows a **9-phase software development lifecycle** per ISO/IEC/IEEE 12207:2017:

1. **Phase 01**: Stakeholder Requirements
2. **Phase 02**: Requirements Analysis
3. **Phase 03**: Architecture Design
4. **Phase 04**: Detailed Design
5. **Phase 05**: Implementation (current phase)
6. **Phase 06**: Integration
7. **Phase 07**: Verification & Validation
8. **Phase 08**: Transition
9. **Phase 09**: Operation & Maintenance

See [docs/lifecycle-guide.md](docs/lifecycle-guide.md) for detailed phase guidance.

### XP Practices

We follow **Extreme Programming (XP)** practices:

- ✅ **Test-Driven Development (TDD)** - Write tests before implementation
- ✅ **Continuous Integration** - Integrate frequently (multiple times daily)
- ✅ **Simple Design** - YAGNI (You Aren't Gonna Need It)
- ✅ **Refactoring** - Continuous improvement while keeping tests green
- ✅ **Pair Programming** - Encouraged but not required

---

## Standards Compliance

All contributions MUST comply with:

### Audio Standards
- **AES5-2018**: Preferred sampling frequencies for PCM applications

### Software Engineering Standards
- **ISO/IEC/IEEE 12207:2017**: Software life cycle processes
- **ISO/IEC/IEEE 29148:2018**: Requirements engineering
- **IEEE 1016-2009**: Software design descriptions
- **ISO/IEC/IEEE 42010:2011**: Architecture description
- **IEEE 1012-2016**: Verification and validation

### Critical Compliance Rules

❌ **NEVER**:
- Reproduce copyrighted content from specifications
- Add hardware-specific code to Standards layer (`lib/Standards/`)
- Commit code without tests
- Break existing tests
- Ignore CI failures

✅ **ALWAYS**:
- Reference specifications by section number only
- Keep Standards layer hardware-agnostic
- Write tests first (TDD)
- Maintain requirements traceability
- Update documentation

---

## Coding Guidelines

### C++ Standards

```cpp
// ✅ CORRECT: C++17 standard features
#include <optional>
#include <variant>
#include <string_view>

std::optional<uint32_t> get_frequency() {
    return std::nullopt;
}

// ❌ WRONG: Compiler extensions
__attribute__((always_inline)) void foo();  // NO
#pragma pack(1)  // NO - use alignas()
```

### Naming Conventions

```cpp
// Classes: PascalCase
class FrequencyValidator {};

// Functions/Methods: snake_case
uint32_t validate_frequency(uint32_t freq);

// Constants: UPPER_SNAKE_CASE
static constexpr uint32_t DEFAULT_TOLERANCE_PPM = 100;

// Namespaces: snake_case
namespace frequency_validation {
    // ...
}
```

### Hardware Abstraction

```cpp
// ✅ CORRECT: Standards layer receives interface
struct AudioInterface {
    virtual int read_samples(void* buffer, size_t count) = 0;
};

class FrequencyValidator {
    FrequencyValidator(std::unique_ptr<AudioInterface> audio);
};

// ❌ WRONG: Direct hardware access in Standards
#include <alsa/asoundlib.h>  // NO!
class FrequencyValidator {
    snd_pcm_t* pcm_handle;  // NO!
};
```

### Documentation Requirements

```cpp
/**
 * @brief Validate sampling frequency against AES5-2018
 * @param frequency Sampling frequency in Hz
 * @return Validation result with compliance information
 * 
 * @traceability DES-C-001 → REQ-F-001 → STK-REQ-001
 * 
 * Validates frequency according to:
 * - AES5-2018 Section 5.1: Primary frequency (48 kHz)
 * - AES5-2018 Section 5.2: Other frequencies (44.1 kHz, 96 kHz)
 * 
 * @performance <50μs validation latency
 * @thread_safety Thread-safe, lock-free
 */
FrequencyValidationResult validate_frequency(uint32_t frequency);
```

---

## Testing Requirements

### TDD Workflow (Red-Green-Refactor)

```bash
# 1. RED: Write failing test
cat > tests/test_new_feature.cpp << EOF
TEST(NewFeature, ValidatesCorrectly) {
    EXPECT_TRUE(new_feature(48000));
}
EOF

# 2. GREEN: Write minimal code to pass
# Edit implementation to make test pass

# 3. REFACTOR: Improve design
# Refactor while keeping tests green

# 4. Verify all tests pass
cd 05-implementation/build
ctest --output-on-failure
```

### Test Coverage Requirements

- **Minimum**: 80% line coverage
- **Target**: 90% branch coverage
- **Unit Tests**: Every public method/function
- **Integration Tests**: Cross-component interactions
- **Conformity Tests**: AES5-2018 compliance validation

### Test Structure

```cpp
TEST_F(FrequencyValidatorTest, ValidatesPrimaryFrequency) {
    // Given: A frequency validator
    auto validator = create_test_validator();
    
    // When: Validating 48 kHz (primary frequency)
    auto result = validator->validate_frequency(48000);
    
    // Then: Validation succeeds
    EXPECT_TRUE(result.is_valid());
    EXPECT_EQ(result.applicable_clause, AES5Clause::Section_5_1);
    EXPECT_EQ(result.detected_frequency, 48000u);
}
```

---

## Pull Request Process

### 1. Create Feature Branch

```bash
git checkout -b feature/your-feature-name
# OR
git checkout -b fix/issue-number-description
```

### 2. Make Changes (TDD)

```bash
# Write tests first
vim tests/test_your_feature.cpp

# Implement feature
vim src/your_implementation.cpp

# Run tests continuously
cmake --build . --target tdd_watch
```

### 3. Ensure Quality

```bash
# All tests pass
ctest --output-on-failure

# Code coverage maintained
gcovr -r .. --html --html-details -o coverage.html

# No compiler warnings
cmake --build . 2>&1 | grep -i warning

# Format code
clang-format -i src/**/*.cpp tests/**/*.cpp

# Static analysis (optional but recommended)
clang-tidy src/**/*.cpp -- -std=c++17
```

### 4. Update Documentation

- Update traceability matrices if requirements changed
- Add/update Architecture Decision Records (ADRs) if architecture changed
- Update API documentation
- Update README if user-facing changes

### 5. Commit with Convention

```bash
git add .
git commit -m "feat: Add frequency validation for 384 kHz

- Implement octuple rate support per AES5-2018 Section 5.2
- Add 16 unit tests covering edge cases
- Update frequency lookup table ranges
- Maintain >80% code coverage

Traceability: REQ-F-002 → DES-C-001 → TEST-C-001-015
Closes #42"
```

**Commit Message Format**:
```
<type>: <subject>

<body>

<footer>
```

**Types**: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

### 6. Push and Create PR

```bash
git push origin feature/your-feature-name
```

Then create Pull Request on GitHub with:
- **Title**: Clear, concise description
- **Description**: 
  - What changed and why
  - Requirements/issues addressed
  - Test coverage
  - Traceability references
- **Checklist**: Use PR template (see below)

### Pull Request Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Standards Compliance
- [ ] Follows IEEE/ISO/IEC standards
- [ ] AES5-2018 compliant (if audio-related)
- [ ] No copyrighted content reproduced

## Testing
- [ ] All tests pass locally
- [ ] Added tests for new features
- [ ] Test coverage maintained (>80%)
- [ ] No regression in existing tests

## Code Quality
- [ ] No compiler warnings
- [ ] Code formatted (clang-format)
- [ ] Static analysis clean (if run)
- [ ] Hardware abstraction preserved

## Documentation
- [ ] API documentation updated
- [ ] Traceability maintained
- [ ] ADRs updated (if architecture changed)
- [ ] README updated (if user-facing)

## Traceability
Requirements: REQ-XXX-XXX
Design: DES-X-XXX
Tests: TEST-X-XXX-XXX
```

---

## Documentation

### Required Documentation Updates

When contributing, update relevant documentation:

1. **Requirements** (`02-requirements/`)
   - Functional requirements
   - Non-functional requirements
   - User stories with acceptance criteria

2. **Architecture** (`03-architecture/`)
   - Architecture Decision Records (ADRs)
   - Quality scenarios
   - Views (if structure changed)

3. **Design** (`04-design/`)
   - Component designs
   - Interface specifications
   - Design patterns used

4. **Traceability** (`07-verification-validation/traceability/`)
   - Requirements → Design → Code → Tests
   - Update matrices when adding requirements

### Markdown Specifications

We use **Spec-Kit** markdown templates:

```markdown
---
id: REQ-F-XXX
title: Feature Title
type: functional
priority: high
status: implemented
links:
  - STK-REQ-XXX: stakeholder requirement
  - DES-C-XXX: design component
  - TEST-C-XXX-XXX: test case
---

# REQ-F-XXX: Feature Title

## Description
Clear description of requirement

## Acceptance Criteria
- [ ] Criterion 1
- [ ] Criterion 2

## Rationale
Why this requirement exists

## AES5-2018 Reference
Section X.Y - Description
```

---

## Questions?

- **Documentation**: See [docs/](docs/) directory
- **Issues**: [GitHub Issues](https://github.com/zarfld/AES5-2018/issues)
- **Discussions**: [GitHub Discussions](https://github.com/zarfld/AES5-2018/discussions)
- **Lifecycle**: [docs/lifecycle-guide.md](docs/lifecycle-guide.md)
- **XP Practices**: [docs/xp-practices.md](docs/xp-practices.md)

---

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

**Thank you for contributing to standards-compliant software engineering!** 🚀
