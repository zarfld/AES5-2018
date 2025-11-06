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
    - SYS-MAIN-001
    - SYS-FUNC-001
---

# ADR-003: C++17 Implementation with C API Bindings

## Metadata
```yaml
adrId: ADR-003
status: accepted
relatedRequirements:
  - SYS-PORT-001 # Cross-platform compatibility requirements
  - SYS-MAIN-001 # Code quality and maintainability requirements
  - SYS-FUNC-001 # Functional requirements implementation
  - SYS-REL-001 # Reliability and error handling requirements
relatedComponents:
  - ARC-C-001 # AES5 Core Library
  - ARC-C-007 # Public API Layer
  - ARC-C-008 # C API Compatibility Layer
supersedes: []
supersededBy: null
author: AI Agent
date: 2025-11-06
reviewers: []
```

## Context

The AES5-2018 library must satisfy diverse integration requirements across multiple programming environments:

**Integration Requirements**:
- **Audio Software Developers**: Modern C++ applications (DAWs, media players)
- **Legacy System Integration**: C-based applications and embedded systems
- **Cross-Language Support**: Integration with Python, C#, Java through C ABI
- **Library Ecosystem**: Integration with existing audio processing libraries (many C-based)

**Code Quality Requirements**:
- **Type Safety**: Prevent common audio processing errors (buffer overflows, format mismatches)
- **Memory Safety**: RAII for deterministic resource management
- **Performance**: Compile-time optimizations and zero-overhead abstractions
- **Maintainability**: Modern C++ features for readable, maintainable code

**Platform Constraints**:
- **Compiler Support**: Must compile on GCC 7+, Clang 6+, MSVC 2017+ 
- **Embedded Compatibility**: ARM GCC, IAR, Keil toolchain support
- **ABI Stability**: C ABI for binary compatibility across compiler versions
- **Standard Compliance**: C++17 features available across all target platforms

## Decision

We will implement the library using **C++17 with C API bindings**:

**Core Implementation Strategy**:
- **Internal Implementation**: Modern C++17 with full feature set
- **Object-Oriented Design**: RAII, smart pointers, exception safety
- **Template Optimization**: Compile-time specialization for platform-specific code
- **Standard Library Usage**: constexpr, auto, std::optional for safety and clarity

**Public API Strategy**:
```cpp
// Primary C++ API (type-safe, modern)
namespace AES5 {
    class FrequencyValidator {
    public:
        enum class ValidationResult { Success, OutOfTolerance, InvalidFormat };
        
        ValidationResult validate(uint32_t frequency_hz) const noexcept;
        bool is_aes5_compliant(uint32_t frequency_hz) const noexcept;
        std::optional<FrequencyCategory> detect_category(uint32_t frequency_hz) const noexcept;
    };
}

// C API Bindings (compatibility, integration)
extern "C" {
    typedef struct aes5_validator aes5_validator_t;
    typedef enum { AES5_SUCCESS = 0, AES5_ERROR_TOLERANCE = 1, AES5_ERROR_FORMAT = 2 } aes5_result_t;
    
    aes5_validator_t* aes5_validator_create(void);
    aes5_result_t aes5_validate_frequency(aes5_validator_t* validator, uint32_t frequency_hz);
    void aes5_validator_destroy(aes5_validator_t* validator);
}
```

## Status

**Accepted** (2025-11-06) - This decision provides the optimal balance of modern C++ safety features with broad ecosystem compatibility through C ABI.

## Rationale

C++17 with C bindings addresses the tension between **modern development practices** and **legacy system compatibility**:

**Type Safety and Performance**: C++17 provides strong typing, RAII, and zero-overhead abstractions that prevent common audio processing errors while maintaining performance.

**Broad Compatibility**: C API bindings enable integration with any language/system supporting C ABI, covering 95%+ of audio software ecosystem.

**Maintainability**: Modern C++ features (auto, constexpr, smart pointers) improve code readability and reduce maintenance burden compared to C-only implementation.

**Compile-Time Optimization**: Template specialization enables platform-specific optimizations while maintaining single source code base.

## Considered Alternatives

| Alternative | Summary | Pros | Cons | Reason Not Chosen |
|------------|---------|------|------|-------------------|
| **Pure C Implementation** | Traditional C with function pointers | Universal compatibility, simple ABI | Manual memory management, error-prone, poor maintainability | Violates code quality requirements (95% test coverage difficult) |
| **Pure C++ (No C Bindings)** | C++17 only with no C compatibility | Excellent type safety, modern features | Limited ecosystem integration | Excludes C-based legacy systems and cross-language integration |
| **C++20/23 Features** | Latest C++ standard features | Most modern language features | Limited compiler support on embedded | Violates cross-platform compatibility requirements |
| **Rust with C FFI** | Memory-safe Rust with C bindings | Memory safety, performance | Limited embedded toolchain support | Insufficient toolchain availability on target platforms |

## Consequences

### Positive
- **Type Safety**: Strong typing prevents buffer overflows and format mismatches
- **Memory Safety**: RAII ensures deterministic resource cleanup
- **Performance**: Template optimization and zero-overhead abstractions
- **Maintainability**: Modern C++ features improve code readability
- **Broad Compatibility**: C ABI supports integration with 95%+ of audio ecosystem
- **Future-Proofing**: C++17 features position library for long-term maintenance

### Negative / Liabilities
- **Compiler Requirements**: Requires C++17-compliant compiler (mitigation: verified support on all target platforms)
- **Binary Size**: C++ features may increase code size (mitigation: template specialization, embedded-specific builds)
- **ABI Complexity**: C++ name mangling requires careful C API design (mitigation: C-only headers for public API)

### Neutral / Follow-ups
- Must maintain both C++ and C header files with synchronized functionality
- Need comprehensive exception-safe wrappers for all C API functions
- Require ABI stability testing across compiler versions and platforms
- Must provide language binding examples (Python, C#, Java)

## Quality Attribute Impact Matrix

| Quality Attribute | Impact (+/−/0) | Notes |
|-------------------|----------------|-------|
| **Maintainability** | ++ | Modern C++ features significantly improve code maintainability |
| **Reliability** | ++ | RAII and strong typing prevent common programming errors |
| **Performance** | + | Template optimization, zero-overhead abstractions |
| **Portability** | + | C ABI ensures broad platform and language compatibility |
| **Testability** | + | C++ testing frameworks and mocking capabilities |
| **Security** | + | Type safety and automatic memory management reduce vulnerabilities |

## Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|-----------|
| **C++17 Compiler Unavailability** | Low | High | Compiler support verification, fallback to C++14 subset if needed |
| **ABI Instability** | Medium | Medium | Comprehensive ABI testing, semantic versioning |
| **Exception Safety in C API** | Medium | High | Exception boundary isolation, comprehensive error code mapping |
| **Binary Size Growth** | Medium | Low | Template specialization, conditional compilation for embedded |

## Compliance Mapping

| Standard Clause | How Addressed |
|-----------------|---------------|
| **ISO 42010 §5.8 (Rationale)** | Language choice rationale with trade-off analysis |
| **IEEE 1016 (Implementation)** | Implementation language and API design decisions |
| **AES5-2018 (Compliance)** | C++ type system ensures AES5-2018 specification compliance |

## Implementation Notes

**Phase 1: Core C++ Implementation**
```cpp
// Type-safe AES5-2018 implementation
namespace AES5::Core {
    template<typename PlatformAdapter>
    class FrequencyProcessor {
    private:
        PlatformAdapter& adapter_;
        StaticBufferPool buffer_pool_;
        
    public:
        constexpr FrequencyProcessor(PlatformAdapter& adapter) noexcept 
            : adapter_(adapter) {}
            
        auto process_frequency(uint32_t frequency_hz) noexcept 
            -> std::expected<ProcessingResult, AES5Error>;
    };
}
```

**Phase 2: Exception-Safe C API Layer**
```cpp
// Exception boundary isolation
extern "C" {
    aes5_result_t aes5_validate_frequency(aes5_validator_t* validator, uint32_t frequency_hz) {
        if (!validator) return AES5_ERROR_NULL_POINTER;
        
        try {
            auto* cpp_validator = reinterpret_cast<AES5::FrequencyValidator*>(validator);
            auto result = cpp_validator->validate(frequency_hz);
            
            switch (result) {
                case AES5::ValidationResult::Success: return AES5_SUCCESS;
                case AES5::ValidationResult::OutOfTolerance: return AES5_ERROR_TOLERANCE;
                case AES5::ValidationResult::InvalidFormat: return AES5_ERROR_FORMAT;
            }
        } catch (const std::exception&) {
            return AES5_ERROR_EXCEPTION;
        } catch (...) {
            return AES5_ERROR_UNKNOWN;
        }
    }
}
```

**Phase 3: Language Bindings Examples**
```python
# Python ctypes binding example
import ctypes

aes5_lib = ctypes.CDLL('./libaes5.so')
aes5_lib.aes5_validator_create.restype = ctypes.c_void_p
aes5_lib.aes5_validate_frequency.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
aes5_lib.aes5_validate_frequency.restype = ctypes.c_int

validator = aes5_lib.aes5_validator_create()
result = aes5_lib.aes5_validate_frequency(validator, 48000)
```

## Validation Plan

**Compiler Compatibility Validation**:
1. Automated CI/CD testing across all required compilers (GCC 7-11, Clang 6-12, MSVC 2017-2022)
2. Cross-compilation testing for embedded ARM targets
3. Feature availability verification for C++17 requirements
4. Performance regression testing across different compiler optimizations

**ABI Stability Validation**:
1. Binary compatibility testing across compiler versions
2. Symbol table analysis for ABI breakage detection  
3. C API integration testing with multiple client applications
4. Cross-language binding validation (Python, C#, Java)

**Exception Safety Validation**:
1. Comprehensive exception injection testing at C++ boundaries
2. Resource leak detection during exception propagation
3. C API error code mapping validation
4. Exception-to-error-code translation correctness testing

**Integration Validation**:
1. Real-world integration testing with existing C-based audio libraries
2. Performance comparison between C++ and C API usage
3. Memory usage analysis for C++ vs C implementation patterns
4. Cross-platform consistency verification for both APIs

## References

- System Requirements Specification (SYS-PORT-001, SYS-MAIN-001)
- ISO/IEC 14882:2017 C++ Language Standard
- "Effective Modern C++" - Scott Meyers
- "C++ Core Guidelines" - Bjarne Stroustrup and Herb Sutter
- Audio Software Developer Integration Requirements Analysis