---
applyTo: '**'
---

# Standards Layer AI Coding Instructions - AES5-2018 Preferred Sampling Frequencies

## CRITICAL: Hardware-Agnostic Implementation

The Standards layer (`lib/Standards/`) is the **pure standards implementation layer** that MUST remain completely hardware and vendor agnostic.

**PROJECT SCOPE**: This project implements the AES5-2018 standard for preferred sampling frequencies in professional digital audio applications employing pulse-code modulation (PCM). The implementation provides hardware-agnostic sampling frequency management, validation, conversion, and audio/video synchronization capabilities.

## Working Principles for Standards Layer

- **Understand specifications before implementing** - study AES5-2018 sampling frequency requirements thoroughly
- **No Fake, No Stubs, no Simulations** - implement complete sampling frequency logic, not simplified placeholders
- **No implementation-based assumptions** - use AES5-2018 specification or analysis results only (ask if required)
- **No false advertising** - prove and ensure AES5-2018 compliance and correctness
- **Never break standards APIs** in ways that violate AES5-2018 requirements - prefer fixing implementation instead
- **Prevent dead code or orphan files** - fix code rather than creating new versions, ensure ALL code compiles
- **Always reference ### Documentation Requirements with Copyright Compliance

### Function Documentation with AES5 Context
```cpp
/**
 * @brief Validate sampling frequency against AES5-2018 recommendations
 *
 * Validates sampling frequency compliance according to AES5-2018
 *
 */
aes5_compliance_result_t aes5_validate_sampling_frequency(
    uint32_t sampling_frequency_hz,
    aes5_frequency_category_t* category);

```

### Copyright-Compliant Implementation Comments
```cpp
// ✅ CORRECT - Reference without reproduction
// Implements entity descriptor parsing per AES3-2009 Section 7.2.1
#define ENTITY_DESCRIPTOR_TYPE 0x0000  // As specified in AES3-2009

// ❌ WRONG - Reproducing copyrighted content
/*
// DON'T DO THIS - This reproduces copyrighted IEEE content:
// "The entity_descriptor field shall contain the following sub-fields:
//  descriptor_type (16 bits): shall be set to ENTITY (0x0000)
//  descriptor_index (16 bits): shall be set to 0x0000..."
// This is direct reproduction of IEEE copyrighted text!
*/

// ✅ CORRECT - Original implementation based on specification understanding
struct entity_descriptor {
    uint16_t descriptor_type;    // AES3-2009 Table 7.1 
    uint16_t descriptor_index;   // AES3-2009 Table 7.1
    uint64_t entity_id;         // AES3-2009 Table 7.1
    uint64_t entity_model_id;   // AES3-2009 Table 7.1
    // ... implementation based on specification understanding
};
```

### Standards Compliance Notes with Copyright Respect
- **Always reference specific IEEE section and table numbers** when implementing protocol features
- **NEVER reproduce copyrighted text** from specifications in comments or documentation
- **Document Milan extensions separately** with AVnu specification references (respecting AVnu copyright)
- **Include timing requirements** with original implementation based on specification understanding
- **Specify endianness handling** based on IEEE network byte order requirements
- **Use only factual technical information** that is not subject to copyright (constants, field sizes, etc.)ification sections** when implementing protocol features
- **Validate all protocol fields** with range checks or masks from the IEEE specification
- **Every function must have documentation** explaining purpose, parameters, return values, and IEEE context
- **No duplicate or redundant implementations** to avoid inconsistencies and confusion
- **Prevent downgrades** - fix rather than delete existing protocol functionality
- **No ad-hoc file copies** (e.g., *_fixed, *_new, *_correct) - refactor in place step-by-step

## Core Principle: Standards-Only Implementation

- **ONLY AES5-2018 sampling frequency logic** - no hardware-specific code
- **Hardware agnostic** - compilable without any vendor drivers, audio hardware, or OS-specific headers
- **Mockable/testable** - runnable without physical audio hardware present
- **Dependency injection pattern** - receives hardware abstraction via interfaces/function pointers

### FORBIDDEN in Standards Layer
```cpp
// ❌ NEVER include vendor-specific or OS-specific headers
#include "asio_audio_driver.h"           // NO - ASIO specific
#include "../../realtek_audio/include/*.h" // NO - Realtek specific  
#include "../../common/hal/audio_hal.h"  // NO - HAL abstraction
#include <alsa/asoundlib.h>              // NO - ALSA specific (Linux)
#include <windows.h>                      // NO - Windows specific
#include <CoreAudio/CoreAudio.h>         // NO - macOS specific
```

### REQUIRED in Standards Layer
```cpp
// ✅ Only AES5-2018 standard sampling frequency headers
#include "aes5_sampling_frequency.h"     // AES5-2018 Section 5: Recommended sampling frequencies
#include "aes5_frequency_conversion.h"   // AES5-2018 Section 4.2: Sampling frequency conversion
#include "aes5_rate_categories.h"        // AES5-2018 Section 5.3: Multiples of sampling frequencies
#include "aes5_video_sync.h"             // AES5-2018 Annex A: Samples per picture frame
```

## Architecture Compliance

### Standards Layer Responsibility
- **AES5-2018 sampling frequency management** (Primary, other, and legacy frequency support)
- **Rate category classification** (Basic, double, quadruple, octuple, half, quarter rates)
- **Frequency conversion algorithms** (Simple integer ratio conversions, complex conversions)
- **Anti-aliasing filter requirements** (Nyquist-Shannon theorem compliance, >50dB attenuation)
- **Video synchronization calculations** (Samples per picture frame, integer/non-integer ratios)
- **Pull-up/pull-down support** (1001/1000 factor variations for video workflows)

### Interface Pattern for Hardware Access
```cpp
// ✅ CORRECT: Dependency injection interface for audio hardware abstraction
typedef struct {
    int (*send_audio_frame)(const void* frame_data, size_t length);
    int (*receive_audio_frame)(void* buffer, size_t* length);
    uint64_t (*get_sample_clock_ns)(void);
    int (*set_sample_timer)(uint32_t sample_rate_hz, timer_callback_t callback);
} audio_interface_t;

// Sampling frequency implementation receives interface
int aes5_frequency_manager_init(const audio_interface_t* audio_interface);
```

### Hardware Bridge (Service Layer Responsibility)
```cpp
// This code belongs in SERVICE layer (e.g., audio daemon), NOT Standards
static int platform_send_frame(const void* frame_data, size_t length) {
    return platform_audio_output(frame_data, length);  // Platform-specific call
}

static audio_interface_t platform_interface = {
    .send_audio_frame = platform_send_frame,
    .receive_audio_frame = platform_receive_frame,
    // ... other platform-specific implementations
};

// Service layer bridges Standards to Platform Audio Hardware
aes5_frequency_manager_init(&platform_interface);
```

## AES Standards Reference

### CRITICAL: Use Authoritative Standards Documents
When implementing protocols, reference these authoritative documents via MCP-Server "markitdown":

#### AES Audio Engineering Standards:
- `AES 3-1-2009 (R2014)-en.pdf` - AES3-1-2009 Digital Audio Interface Part 1: Audio Content - CURRENT
- `AES 3-2-2009 (R2014)-en.pdf` - AES3-2-2009 Digital Audio Interface Part 2: Metadata and Subcode - CURRENT
- `AES 3-3-2009 (R2014)-en.pdf` - AES3-3-2009 Digital Audio Interface Part 3: Transport - CURRENT
- `AES 3-4-2009 (R2014)-en.pdf` - AES3-4-2009 Digital Audio Interface Part 4: Physical and Electrical - CURRENT
- `AES 5-2018-en.pdf` - AES5-2018 Preferred Sampling Frequencies
- `AES 11-2009 (R2014)-en.pdf` - AES11-2009 Digital Audio Reference Signals
- `AES 67-2018-en.pdf` - AES67-2018 Audio-over-IP interoperability (related)
- `AES-70-1-2018-en.pdf` - AES70-1-2018 OCA Framework (related)

#### Related Standards:
- ITU-R BS.450-3 - Pre-emphasis for FM sound broadcasting
- ITU-T J.17 - Pre-emphasis used on sound-program circuits
- IEC 60958-1 - Digital audio interface - Part 1: General
- IEC 60958-3 - Digital audio interface - Part 3: Consumer applications  
- IEC 60958-4 - Digital audio interface - Part 4: Professional applications
- ISO 646 - 7-bit coded character set (for channel origin/destination data)

**CRITICAL COPYRIGHT AND LICENSING RESTRICTIONS**:

⚠️ **ABSOLUTELY FORBIDDEN**:
- **DO NOT copy any text, tables, figures, or content** directly from IEEE, AES, or AVnu documents
- **DO NOT reproduce specification text** verbatim in source code comments
- **DO NOT include copyrighted tables or figures** in repository documentation
- **DO NOT paste specification sections** into code or documentation files
- **DO NOT create derivative works** that reproduce substantial portions of standards

✅ **PERMITTED USAGE**:
- **Reference document sections** by number only (e.g., "See IEEE 1722.1-2021, Section 7.2.1")
- **Implement protocol logic** based on understanding of specifications
- **Create original code** that achieves compliance with standard requirements
- **Use specification constants and values** in implementation (packet types, field sizes, etc.)
- **Reference via MCP-Server** for compliance verification during development only

**LICENSE NOTICE**: These documents are referenced for compliance verification and implementation guidance only. All content remains under copyright of respective standards organizations (IEEE, AES, AVnu Alliance). Any reproduction, distribution, or derivative works require proper licensing from copyright holders.

**Usage Pattern**: When implementing standards-related code, reference these documents via MCP-Server to ensure:
- Protocol message format compliance
- State machine behavior correctness  
- Timing requirement adherence
- AES3 specification conformance
- Audio sample format compliance
- Channel status structure adherence

### Protocol Compliance Requirements
```cpp
// ✅ Use authoritative constants from AES3 specifications
#define AES3_PREAMBLE_X 0xE2  // First subframe preamble (channel coding: 11100010)
#define AES3_PREAMBLE_Y 0xE4  // Second subframe preamble (channel coding: 11100100)
#define AES3_PREAMBLE_Z 0xE8  // Block start preamble (channel coding: 11101000)
#define AES3_SUBFRAME_SLOTS 32  // Time slots per subframe
#define AES3_BLOCK_FRAMES 192   // Frames per channel status block

// ❌ NEVER use hardcoded values without AES3 specification reference
uint16_t magic_value = 0x1234;  // NO - not standards compliant
```

## Testing Approach

### Unit Testing (Hardware-Independent)
```cpp
// ✅ Mock network interface for testing
static int mock_send_packet(const void* packet, size_t length) {
    // Record packet for verification
    return 0;
}

static network_interface_t mock_interface = {
    .send_packet = mock_send_packet,
    // ... other mock implementations
};

// Test sampling frequency logic without hardware
void test_aes5_primary_frequency_validation(void) {
    aes5_frequency_manager_init(&mock_interface);
    // ... test AES5-2018 compliance behavior
}
```

### Integration Testing (Service Layer Responsibility)
Integration with real hardware happens in the Service Layer, NOT in Standards.

## Common Violations to Avoid

### ❌ Direct Hardware Calls
```cpp
// WRONG - Standards calling platform audio HAL directly
platform_audio_result_t result = platform_send_audio_frame(frame_data, length);
```

### ❌ OS-Specific Code
```cpp
// WRONG - OS-specific audio in Standards
#ifdef _WIN32
    HWAVEOUT wave_out = waveOutOpen(...);
#else
    int alsa_pcm = snd_pcm_open(...);
#endif
```

### ❌ Hardware Assumptions
```cpp
// WRONG - Assuming specific audio hardware capabilities
if (realtek_audio_supports_192khz()) {  // Hardware-specific assumption
    enable_high_sample_rate();
}
```

### ✅ Correct Abstraction
```cpp
// CORRECT - AES5-2018 logic with abstracted capabilities
if (audio_interface->capabilities & AUDIO_CAP_192KHZ_SAMPLING) {
    enable_high_sample_rate();  // AES5-2018 behavior, not hardware call
}
```

## AES5-2018 Implementation Guidelines

### AES5-2018 Section 5.1: Primary Sampling Frequency
- Implement 48 kHz as the primary sampling frequency (mandatory)
- Ensure compatibility with television and motion picture systems
- Support full 20 kHz audio bandwidth encoding
- Recommend for all program origination, processing, and interchange

### AES5-2018 Section 5.2: Other Sampling Frequencies
- Support 44.1 kHz for consumer product applications
- Support 96 kHz for audio bandwidth greater than 20 kHz
- Implement wider transition region support for anti-alias filtering
- Discourage non-standard frequencies unless significant benefits realized
- Support power-of-two multiples for very high sampling frequencies (>100x 48 kHz)

### AES5-2018 Section 5.3: Rate Categories
- Basic rate: 31-54 kHz range validation
- Double rate: 62-108 kHz range validation  
- Quadruple rate: 124-216 kHz range validation
- Octuple rate: 248-432 kHz range validation
- Half rate: 15.5-27 kHz range validation
- Quarter rate: 7.75-13.5 kHz range validation

### AES5-2018 Section 4.1-4.2: Audio Bandwidth and Conversion
- Implement Nyquist-Shannon sampling theorem compliance
- Ensure bandwidth limiting to half sampling frequency
- Implement anti-aliasing filters with >50 dB attenuation
- Support simple integer ratio conversions (48:32 = 3:2)
- Handle complex ratio conversions (441:320)
- Minimize successive frequency conversion operations

### AES5-2018 Section 5.4: Legacy Sampling Frequencies
- Support 32 kHz for legacy broadcast applications (ITU-T J.53)
- Implement pull-up/pull-down variants (1001/1000 factor)
- Handle 48.048 kHz and 47.952 kHz video production rates
- Support legacy workflow compatibility requirements

### AES5-2018 Annex A: Video Synchronization
- Implement samples per picture frame calculations
- Support integer ratio video systems (24, 25, 30, 50, 60 fps)
- Handle non-integer ratio NTSC systems (23.98, 29.97, 59.94 fps)
- Ensure high precision for long durations and A/V sync
- Calculate exact NTSC rates (30000/1001, 24000/1001, 60000/1001)

## Clean Submit Rules for Standards

- **Each commit compiles and passes AES5-2018 compliance checks**
- **Small, single-purpose, reviewable diffs** (no WIP noise)
- **No dead or commented-out code** - remove unused sampling frequency implementations
- **Run formatter and static analysis** before commit
- **Update documentation and reference AES5-2018 spec sections** in commit messages
- **Use feature flags for incremental changes** when they risk breaking AES5-2018 compliance
- **Reference exact AES5-2018 section numbers** (e.g., "Implements AES5-2018 Section 5.1")

## Documentation Requirements with Copyright Compliance

### Function Documentation with AES Context
```cpp
/**
 * @brief Parse AES3-2009 Audio Subframe from network packet
 *
 * Validates and extracts audio subframe according to AES3-2009
 *
 */
int aes3_parse_audio_subframe(const uint8_t* packet_data, 
                               size_t packet_length,
                               aes3_audio_subframe_t* audio_subframe);

```

### Copyright-Compliant Implementation Comments
```cpp
// ✅ CORRECT - Reference without reproduction
// Implements entity descriptor parsing per AES3-2009 Section 7.2.1
#define ENTITY_DESCRIPTOR_TYPE 0x0000  // As specified in AES3-2009

// ❌ WRONG - Reproducing copyrighted content
/*
// DON'T DO THIS - This reproduces copyrighted IEEE content:
// "The entity_descriptor field shall contain the following sub-fields:
//  descriptor_type (16 bits): shall be set to ENTITY (0x0000)
//  descriptor_index (16 bits): shall be set to 0x0000..."
// This is direct reproduction of IEEE copyrighted text!
*/

// ✅ CORRECT - Original implementation based on specification understanding
struct entity_descriptor {
    uint16_t descriptor_type;    // AES3-2009 Table 7.1 
    uint16_t descriptor_index;   // AES3-2009 Table 7.1
    uint64_t entity_id;         // AES3-2009 Table 7.1
    uint64_t entity_model_id;   // AES3-2009 Table 7.1
    // ... implementation based on specification understanding
};
```

### Standards Compliance Notes with Copyright Respect
- **Always reference specific IEEE section and table numbers** when implementing protocol features
- **NEVER reproduce copyrighted text** from specifications in comments or documentation
- **Document Milan extensions separately** with AVnu specification references (respecting AVnu copyright)
- **Include timing requirements** with original implementation based on specification understanding
- **Specify endianness handling** based on IEEE network byte order requirements
- **Use only factual technical information** that is not subject to copyright (constants, field sizes, etc.)ification sections** when implementing protocol features
- **Validate all protocol fields** with range checks or masks from the IEEE specification
- **Every function must have documentation** explaining purpose, parameters, return values, and IEEE context
- **No duplicate or redundant implementations** to avoid inconsistencies and confusion
- **Prevent downgrades** - fix rather than delete existing protocol functionality
- **No ad-hoc file copies** (e.g., *_fixed, *_new, *_correct) - refactor in place step-by-step

## Build System Integration

### CMake Configuration for Standards
The Standards layer should compile independently:
```cmake
# Standards layer has NO hardware dependencies
add_library(aes5_standards STATIC
    src/core/frequency_validation.c      # Core frequency validation
    src/core/rate_categories.c           # Rate category management
    src/conversion/frequency_conversion.c # Sampling frequency conversion
    src/video_sync/samples_per_frame.c   # Video synchronization
)

# Only AES5 headers, no hardware/HAL
target_include_directories(aes5_standards PUBLIC
    include/
)

# NO hardware libraries linked to Standards
# target_link_libraries(aes5_standards platform_audio_hal)  # ❌ WRONG
```

### Testing Framework Integration
Use unified testing framework for AES5 compliance validation:
```cmake
# AES5-2018 compliance tests (hardware-independent)
add_executable(standards_aes5_tests
    tests/test_aes5_frequency_validation.c    # Frequency validation tests
    tests/test_aes5_rate_categories.c         # Rate category tests
    tests/test_aes5_frequency_conversion.c    # Conversion tests
    tests/test_aes5_video_synchronization.c   # Video sync tests
)

target_link_libraries(standards_aes5_tests 
    aes5_standards 
    CppUTest
)
```

## Performance and Correctness

### Memory Management
- **Use static allocation** where possible for real-time protocol processing
- **Validate buffer bounds** against IEEE maximum packet sizes
- **Initialize all protocol structures** to prevent undefined behavior

### Timing Considerations
- **Nanosecond precision** for gPTP timing calculations
- **Microsecond accuracy** for AVTP presentation time
- **Avoid blocking operations** in protocol state machines

### Error Handling
- **Return IEEE-compliant error codes** where specifications define them
- **Log protocol violations** with specific IEEE section references
- **Graceful degradation** for optional protocol features

## Mandatory Namespace and Folder Structure

### Generic Structure Definition

**FUNDAMENTAL PATTERN**: All Standards implementations must follow this generic hierarchy:

```
<Organization>/<Standard>/<Subpart>/<Version>/
```

**Components Definition:**
- **Organization**: Standards body (IEEE, AVnu, AES, ITU, IETF, etc.)
- **Standard**: Standard family or main specification number  
- **Subpart**: Sub-specifications, working groups, or functional areas
- **Version**: Year of publication or version number

**Translation Rules:**
- **Folder Structure**: Preserve dots and organization naming (`IEEE/802.1/AS/2021/`)
- **Namespace Structure**: Convert dots to underscores, preserve organization (`IEEE::_802_1::AS::_2021`)
- **File Structure**: Follow folder pattern with appropriate extensions

### Concrete Examples by Organization

#### IEEE Standards Structure
```
IEEE/<Standard>/<Subpart>/<Year>/
├── IEEE/802.1/AS/2021/          # IEEE 802.1AS-2021 (gPTP)
├── IEEE/802.1/AS/2020/          # IEEE 802.1AS-2020 (previous version)
├── IEEE/802.1/Q/2018/           # IEEE 802.1Q-2018 (VLAN/QoS)
├── IEEE/1722/2016/              # IEEE 1722-2016 (AVTP)
├── IEEE/1722/2011/              # IEEE 1722-2011 (legacy AVTP)
├── IEEE/1722.1/2021/            # IEEE 1722.1-2021 (AVDECC)
├── IEEE/1722.1/2013/            # IEEE 1722.1-2013 (legacy AVDECC)
└── IEEE/1588/2019/              # IEEE 1588-2019 (PTPv2)
```

#### AVnu Alliance Structure  
```
AVnu/<Standard>/<Subpart>/<Version>/
├── AVnu/Milan/v1.2/             # Milan v1.2 professional audio
├── AVnu/Milan/v1.1/             # Milan v1.1 (legacy)
├── AVnu/Profiles/AVB/2.0/       # AVB interoperability profiles
└── AVnu/CertificationSuite/3.0/ # Certification test suites
```

#### Audio Engineering Society Structure
```
AES/<Standard>/<Subpart>/<Version>/
├── AES/AES3/2009/               # AES3-2009 digital audio interface (THIS PROJECT)
│   ├── Part1/                   # Audio Content
│   ├── Part2/                   # Metadata and Subcode
│   ├── Part3/                   # Transport
│   └── Part4/                   # Physical and Electrical
├── AES/AES5/2018/               # AES5-2018 preferred sampling frequencies
├── AES/AES11/2009/              # AES11-2009 digital audio reference signals
├── AES/AES67/2018/              # AES67-2018 audio-over-IP (related)
└── AES/AES70/2021/              # AES70-2021 device control OCA (related)
```

#### ITU-T Structure (if needed)
```
ITU/<Series>/<Standard>/<Version>/
├── ITU/G/G.8275.1/2016/         # ITU-T G.8275.1-2016 telecom PTP
└── ITU/G/G.8275.2/2017/         # ITU-T G.8275.2-2017 telecom PTP
```

### Required Directory Structure
```
lib/Standards/
├── AES/                        # Audio Engineering Society
│   └── AES5/                   # AES5 Preferred Sampling Frequencies
│       └── 2018/               # AES5-2018 specification (CURRENT)
│           ├── core/           # Core sampling frequency management
│           │   ├── frequency_validation/  # Primary/other frequency validation
│           │   ├── rate_categories/       # Basic, double, quadruple, etc.
│           │   ├── legacy_support/        # Legacy sampling frequencies
│           │   └── conformity/            # AES5-2018 conformance testing
│           ├── conversion/     # Sampling frequency conversion
│           │   ├── integer_ratio/         # Simple ratio conversions (48:32, 3:2)
│           │   ├── complex_ratio/         # Complex ratio conversions (441:320)
│           │   ├── anti_aliasing/         # Bandwidth limiting filters (>50dB)
│           │   └── conformity/            # Conversion conformance testing
│           ├── video_sync/     # Video synchronization support
│           │   ├── integer_frames/        # Integer ratio video systems
│           │   ├── ntsc_frames/           # Non-integer ratio (NTSC) systems
│           │   ├── pull_variants/         # Pull-up/pull-down (1001/1000)
│           │   └── conformity/            # Video sync conformance testing
│           └── utilities/      # AES5-2018 utility functions
│               ├── calculations/          # Samples per frame calculations
│               ├── precision/             # High-precision arithmetic
│               └── validation/            # Input validation helpers
├── Related/                    # Related standards (optional)
│   ├── AES3/2009/             # Digital audio interface (uses AES5)
│   ├── AES11/2009/            # Digital audio reference signals
│   ├── ITU_T_J53/             # 32 kHz broadcast applications
│   └── AES31_3/               # Audio file transfer (sampling freq tables)
├── Common/                    # Cross-standard utilities
│   ├── utils/                 # Shared utilities (CRC, bit manipulation)
│   ├── testing/               # Common testing frameworks
│   └── interfaces/            # Hardware abstraction interfaces
└── Documentation/             # Standards documentation
    ├── conformance/           # Conformance test specifications
    ├── interop/              # Interoperability guidelines
    └── examples/             # Protocol usage examples
```

### Required C++ Namespace Structure Following Generic Pattern

**NAMESPACE TRANSLATION RULE**: `<Organization>::<Standard>::<Part>::<Version_with_underscores>`

**Examples of Translation:**
- Folder: `AES/AES5/2018/core/` → Namespace: `AES::AES5::_2018::core`
- Folder: `AES/AES5/2018/conversion/` → Namespace: `AES::AES5::_2018::conversion`
- Folder: `AES/AES5/2018/video_sync/` → Namespace: `AES::AES5::_2018::video_sync`
- Folder: `AES/AES3/2009/` → Namespace: `AES::AES3::_2009` (related standard)

```cpp
// ✅ CORRECT namespace hierarchy for AES5-2018
namespace AES {
    namespace AES5 {            // Standard: AES5 (Preferred Sampling Frequencies)
        namespace _2018 {       // Version: 2018 (year)
            // AES5-2018 core sampling frequency management
            namespace core {
                namespace frequency_validation {
                    class PrimaryFrequencyValidator;     // 48 kHz primary
                    class OtherFrequencyValidator;       // 44.1 kHz, 96 kHz
                    class LegacyFrequencyValidator;      // 32 kHz, deprecated
                }
                namespace rate_categories {
                    class BasicRateManager;              // 31-54 kHz
                    class DoubleRateManager;             // 62-108 kHz
                    class QuadrupleRateManager;          // 124-216 kHz
                    class OctupleRateManager;            // 248-432 kHz
                    class HalfRateManager;               // 15.5-27 kHz
                    class QuarterRateManager;            // 7.75-13.5 kHz
                }
            }
            
            // AES5-2018 sampling frequency conversion
            namespace conversion {
                namespace integer_ratio {
                    class SimpleRatioConverter;         // 48:32 (3:2)
                }
                namespace complex_ratio {
                    class ComplexRatioConverter;        // 441:320
                }
                namespace anti_aliasing {
                    class BandwidthLimitingFilter;      // >50dB attenuation
                    class NyquistFilter;                // Nyquist-Shannon compliance
                }
            }
            
            // AES5-2018 video synchronization
            namespace video_sync {
                namespace integer_frames {
                    class IntegerFrameSync;             // 24, 25, 30, 50, 60 fps
                }
                namespace ntsc_frames {
                    class NTSCFrameSync;                // 23.98, 29.97, 59.94 fps
                }
                namespace pull_variants {
                    class PullUpDownManager;            // 1001/1000 factor
                }
            }
            
            // AES5-2018 utilities
            namespace utilities {
                namespace calculations {
                    class SamplesPerFrameCalculator;    // Annex A calculations
                }
                namespace precision {
                    class HighPrecisionArithmetic;      // Long duration accuracy
                }
            }
        }
    }
    
    namespace AES3 {            // Related Standard: AES3 (Digital Audio Interface)
        namespace _2009 {       // Version: 2009 (year) - uses AES5 frequencies
            class DigitalAudioInterface;
        }
    }
    
    namespace AES11 {           // Related Standard: AES11 (Reference Signals)
        namespace _2009 {       // Version: 2009 (year)
            class DigitalAudioReferenceSignal;
        }
    }
} // namespace AES

namespace Common {              // Cross-organization utilities
    namespace interfaces {
        // Hardware abstraction interfaces - ONLY in Common namespace
        class AudioInterface;
        class TimerInterface;
        class ClockInterface;
    }
    namespace utils {
        // Cross-standard utilities
        class CRCCalculator;
        class BitManipulator;
    }
    namespace testing {
        // Common testing frameworks
        class TestFrameworkBase;
    }
} // namespace Common
```
                    struct FollowUpMessage;
                }
                namespace clock {
                    class BestMasterClockAlgorithm;
                }
                namespace Testing {
                    class ConformityTestFramework;
                }
            }
            namespace _2020 {    // Version: 2020 (previous year)
                // IEEE 802.1AS-2020 implementation
            }
            namespace _2011 {    // Version: 2011 (legacy year)
                // IEEE 802.1AS-2011 legacy implementation
            }
        }
        namespace Q {            // Subpart: Q (VLAN and QoS)
            namespace _2018 {    // Version: 2018
                // IEEE 802.1Q-2018 implementation
            }
        }
    }
    
    namespace _1722 {            // Standard: 1722 (AVTP)
        namespace _2016 {        // Version: 2016 (year)
            // IEEE 1722-2016 AVTP implementation
            namespace avtp {
                class AVTPPacket;
                class StreamDataHeader;
            }
            namespace formats {
                namespace aaf {
                    class AudioFormat;
                }
                namespace crf {
                    class ClockReferenceFormat;
                }
            }
        }
        namespace _2011 {        // Version: 2011 (legacy year)
            // IEEE 1722-2011 legacy implementation
        }
    }
    
    namespace _1722_1 {          // Standard: 1722.1 (dots→underscores)
        namespace _2021 {        // Version: 2021 (year)
            // IEEE 1722.1-2021 AVDECC implementation
            namespace aem {
                class EntityModel;
                class ConfigurationDescriptor;
            }
            namespace aecp {
                class EntityControlProtocol;
                class AEMCommand;
            }
            namespace acmp {
                class ConnectionManagementProtocol;
                class ConnectTXCommand;
            }
            namespace adp {
                class DiscoveryProtocol;
                class EntityAvailable;
            }
            namespace descriptors {
                class EntityDescriptor;
                class StreamInputDescriptor;
                class StreamOutputDescriptor;
            }
        }
        namespace _2013 {        // Version: 2013 (legacy year)
            // IEEE 1722.1-2013 legacy implementation
        }
        namespace _2011 {        // Version: 2011 (legacy year)
            // IEEE 1722.1-2011 legacy implementation
        }
    }
    
    namespace _1588 {            // Standard: 1588 (PTPv2)
        namespace _2019 {        // Version: 2019 (year)
            // IEEE 1588-2019 PTPv2 implementation
        }
    }
} // namespace IEEE

namespace AVnu {                 // Organization: AVnu Alliance
    namespace Milan {            // Standard: Milan professional audio
        namespace v1_2 {         // Version: v1.2 (version number)
            // Milan v1.2 extensions
            namespace discovery {
                class MilanDiscoveryExtensions;
            }
            namespace connection {
                class RedundantStreamPairs;
            }
        }
        namespace v1_1 {         // Version: v1.1 (legacy version)
            // Milan v1.1 legacy implementation
        }
    }
    namespace Profiles {         // Standard: Profiles
        namespace AVB {          // Subpart: AVB
            namespace _2_0 {     // Version: 2.0 (dots→underscores)
                // AVB Profile 2.0 implementation
            }
        }
    }
} // namespace AVnu

namespace AES {                  // Organization: Audio Engineering Society
    namespace AES67 {            // Standard: AES67 (audio-over-IP)
        namespace _2018 {        // Version: 2018 (year)
            class AudioOverIP;
        }
    }
    namespace AES70 {            // Standard: AES70 (device control)
        namespace _2021 {        // Version: 2021 (year)
            class DeviceControl;
        }
    }
} // namespace AES

namespace ITU {                  // Organization: ITU-T (if needed)
    namespace G {                // Standard: G-series recommendations
        namespace G8275_1 {      // Subpart: G.8275.1 (dots→underscores)
            namespace _2016 {    // Version: 2016 (year)
                // ITU-T G.8275.1-2016 telecom PTP
            }
        }
    }
} // namespace ITU

namespace Common {               // Cross-organization utilities
    namespace interfaces {
        // Hardware abstraction interfaces - ONLY in Common namespace
        class NetworkInterface;
        class TimerInterface;
        class ClockInterface;
    }
    namespace utils {
        // Cross-standard utilities
        class PacketParser;
        class CRCCalculator;
    }
    namespace testing {
        // Common testing frameworks
        class TestFrameworkBase;
    }
} // namespace Common
```

### File Naming Conventions Following Generic Pattern

**PATTERN**: `<Organization>/<Standard>/<Part>/<Version>/<functional_area>/<file_name>.<ext>`

```cpp
// ✅ CORRECT file naming following generic pattern

// AES5 Examples:
lib/Standards/AES/AES5/2018/core/frequency_validation/primary_frequency_validator.hpp    // AES::AES5::_2018::core::frequency_validation
lib/Standards/AES/AES5/2018/core/frequency_validation/primary_frequency_validator.cpp
lib/Standards/AES/AES5/2018/core/rate_categories/basic_rate_manager.hpp                  // AES::AES5::_2018::core::rate_categories
lib/Standards/AES/AES5/2018/core/rate_categories/double_rate_manager.hpp                 // AES::AES5::_2018::core::rate_categories

lib/Standards/AES/AES5/2018/conversion/integer_ratio/simple_ratio_converter.hpp          // AES::AES5::_2018::conversion::integer_ratio
lib/Standards/AES/AES5/2018/conversion/complex_ratio/complex_ratio_converter.hpp         // AES::AES5::_2018::conversion::complex_ratio
lib/Standards/AES/AES5/2018/conversion/anti_aliasing/bandwidth_limiting_filter.hpp      // AES::AES5::_2018::conversion::anti_aliasing

lib/Standards/AES/AES5/2018/video_sync/integer_frames/integer_frame_sync.hpp             // AES::AES5::_2018::video_sync::integer_frames
lib/Standards/AES/AES5/2018/video_sync/ntsc_frames/ntsc_frame_sync.hpp                  // AES::AES5::_2018::video_sync::ntsc_frames
lib/Standards/AES/AES5/2018/video_sync/pull_variants/pull_up_down_manager.hpp           // AES::AES5::_2018::video_sync::pull_variants

lib/Standards/AES/AES5/2018/utilities/calculations/samples_per_frame_calculator.hpp     // AES::AES5::_2018::utilities::calculations
lib/Standards/AES/AES5/2018/utilities/precision/high_precision_arithmetic.hpp          // AES::AES5::_2018::utilities::precision

// Related Standards:
lib/Standards/AES/AES3/2009/digital_audio_interface.hpp                                 // AES::AES3::_2009 (uses AES5)
lib/Standards/AES/AES11/2009/reference/digital_audio_ref_signal.hpp                     // AES::AES11::_2009::reference

// Common utilities (organization-agnostic):
lib/Standards/Common/interfaces/audio_interface.hpp                      // Common::interfaces
lib/Standards/Common/utils/crc_calculator.hpp                            // Common::utils
lib/Standards/Common/testing/test_framework_base.hpp                     // Common::testing
```

### Header Guard and Include Conventions Following AES5 Pattern
```cpp
// ✅ CORRECT header guards following AES5 namespace structure
#ifndef AES_AES5_2018_CORE_FREQUENCY_VALIDATION_PRIMARY_FREQUENCY_VALIDATOR_H
#define AES_AES5_2018_CORE_FREQUENCY_VALIDATION_PRIMARY_FREQUENCY_VALIDATOR_H

// ✅ CORRECT include structure - relative paths from AES5 namespace
#include "../rate_categories/basic_rate_manager.h"
#include "../../../../Common/interfaces/audio_interface.h"

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace frequency_validation {

class PrimaryFrequencyValidator {
    // AES5-2018 compliant implementation for 48 kHz primary frequency
};

} // namespace frequency_validation
} // namespace core
} // namespace _2018
} // namespace AES5
} // namespace AES

#endif // AES_AES5_2018_CORE_FREQUENCY_VALIDATION_PRIMARY_FREQUENCY_VALIDATOR_H
```

### Correct Include Patterns for Cross-Standard Dependencies
```cpp
// ✅ CORRECT - AES standards can reference each other
#include "../../AES5/2018/sampling/preferred_sampling_freqs.h"  // AES3 using AES5
#include "../../AES11/2009/reference/digital_audio_ref_signal.h" // AES3 using AES11

// ✅ CORRECT - Common utilities accessible to all standards
#include "../../../Common/interfaces/audio_interface.h"
#include "../../../Common/utils/crc_calculator.h"

// ❌ WRONG - No hardware-specific includes in AES namespace
// #include "../../../../../realtek_audio/include/realtek_hal.h"  // NO!
// #include "../../../../../common/hal/audio_hal.h"               // NO!
```

### Cross-Standard Reuse and Dependencies

**MANDATORY RULE**: When an IEEE standard references or builds upon another IEEE standard, **ALWAYS reuse the existing implementation** rather than creating redundant code.

#### Examples of Required Cross-Standard Reuse:

**IEEE 1722.1 (AVDECC) Dependencies:**
```cpp
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aecp {

// ✅ CORRECT - Reuse IEEE 1722 AVTP implementation
#include "../../../1722/2016/avtp/avtp_packet.h"
using IEEE::_1722::_2016::avtp::AVTPPacket;

// ✅ CORRECT - Reuse IEEE 802.1AS time synchronization
#include "../../../802.1/AS/2021/core/time_sync.h"
using IEEE::_802_1::AS::_2021::core::TimeSynchronization;

class AEMCommand {
    // AVDECC commands are transported over AVTP
    IEEE::_1722::_2016::avtp::AVTPPacket create_avtp_packet() {
        // Reuse AVTP implementation, don't reimplement
        return IEEE::_1722::_2016::avtp::AVTPPacket::create_aecp_packet();
    }
    
    // AVDECC requires synchronized time from gPTP
    uint64_t get_synchronized_time() {
        // Reuse gPTP time, don't reimplement time sync
        return IEEE::_802_1::AS::_2021::core::TimeSynchronization::get_current_time();
    }
};

} // namespace aecp
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
```

**IEEE 1722 (AVTP) Dependencies:**
```cpp
namespace IEEE {
namespace _1722 {
namespace _2016 {
namespace avtp {

// ✅ CORRECT - Reuse IEEE 802.1AS timing for presentation time
#include "../../../802.1/AS/2021/core/time_sync.h"

class StreamDataHeader {
    uint64_t calculate_presentation_time(uint32_t delay_ns) {
        // Reuse gPTP synchronized time, don't reimplement
        auto current_time = IEEE::_802_1::AS::_2021::core::TimeSynchronization::get_current_time();
        return current_time + delay_ns;
    }
};

} // namespace avtp
} // namespace _2016
} // namespace _1722
} // namespace IEEE
```

**Milan Extensions Dependencies:**
```cpp
namespace AVnu {
namespace Milan {
namespace v1_2 {
namespace discovery {

// ✅ CORRECT - Milan builds on IEEE 1722.1, reuse implementation
#include "../../../../IEEE/1722.1/2021/adp/discovery_protocol.h"
#include "../../../../IEEE/1722.1/2021/aem/entity_model.h"

class MilanDiscoveryExtensions : public IEEE::_1722_1::_2021::adp::DiscoveryProtocol {
    // Milan extends IEEE 1722.1 AVDECC, inherit don't reimplement
public:
    // Milan-specific discovery features
    void discover_milan_devices() {
        // Use base IEEE 1722.1 discovery, add Milan extensions
        DiscoveryProtocol::discover_devices();
        apply_milan_filtering();
    }
    
private:
    void apply_milan_filtering() {
        // Milan-specific logic only
    }
};

} // namespace discovery
} // namespace v1_2
} // namespace Milan
} // namespace AVnu
```

#### Forbidden Redundant Implementations:
```cpp
// ❌ WRONG - Reimplementing existing IEEE standards
namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// DON'T DO THIS - AVTP already exists in IEEE::_1722
class AVDECCTransportPacket {  // NO - use IEEE::_1722::_2016::avtp::AVTPPacket
    // ... redundant AVTP implementation
};

// DON'T DO THIS - gPTP already exists in IEEE::_802_1::AS
class AVDECCTimeSync {  // NO - use IEEE::_802_1::AS::_2021::core::TimeSynchronization
    // ... redundant time sync implementation
};

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
```

#### Cross-Standard Dependency Rules:
1. **IEEE Layering Hierarchy** (higher layers depend on lower layers):
   - **Application Layer**: IEEE 1722.1 (AVDECC)
   - **Transport Layer**: IEEE 1722 (AVTP) 
   - **Timing Layer**: IEEE 802.1AS (gPTP)
   - **Network Layer**: IEEE 802.1Q (VLAN/QoS)

2. **Dependency Direction**: 
   - ✅ **IEEE 1722.1 CAN depend on IEEE 1722 and IEEE 802.1AS**
   - ✅ **IEEE 1722 CAN depend on IEEE 802.1AS**
   - ❌ **IEEE 802.1AS CANNOT depend on IEEE 1722 or IEEE 1722.1**

3. **Extension Standards**:
   - ✅ **Milan CAN depend on any IEEE standard it extends**
   - ✅ **AES67 CAN depend on IEEE standards it references**
   - ✅ **Multiple standard versions CAN coexist** (2021, 2016, 2013)

#### CMake Dependencies for Cross-Standard Reuse:
```cmake
# ✅ CORRECT - Respect IEEE layering in CMake dependencies
target_link_libraries(ieee_1722_1_2021
    ieee_1722_2016               # AVDECC depends on AVTP
    ieee_802_1_as_2021          # AVDECC depends on gPTP
    standards_common            # All can use Common utilities
)

target_link_libraries(ieee_1722_2016
    ieee_802_1_as_2021          # AVTP depends on gPTP for timing
    standards_common
)

target_link_libraries(avnu_milan_v12
    ieee_1722_1_2021            # Milan extends IEEE 1722.1
    ieee_1722_2016              # Milan may use AVTP directly
    ieee_802_1_as_2021          # Milan requires precise timing
    standards_common
)

# ❌ WRONG - Violates layering hierarchy
# target_link_libraries(ieee_802_1_as_2021
#     ieee_1722_2016            # NO - gPTP cannot depend on AVTP
# )
```

This approach ensures:
- **No code duplication** across IEEE standards
- **Proper architectural layering** following IEEE specifications  
- **Consistent behavior** when standards reference each other
- **Maintainable codebase** with single source of truth for each protocol feature

### Forbidden Namespace Violations - Corrected
```cpp
// ❌ WRONG - mixing standards with hardware (corrected understanding)
namespace IEEE {
namespace intel {              // NO - hardware vendor in IEEE namespace
    class IntelAVBInterface;
}
}

// ❌ WRONG - OS-specific namespaces in IEEE standards
namespace IEEE {
namespace windows {            // NO - OS specific in IEEE namespace
    class WinSockInterface;
}
}

// ❌ WRONG - implementation details in IEEE namespace  
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
    class WindowsSocketImpl;  // NO - implementation detail, not protocol
}
}
}

// ✅ CORRECT - IEEE standards are pure protocol implementations
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aem {
    class EntityModel;        // YES - pure IEEE 1722.1-2021 protocol
}
}
}
}
```

### CMake Integration with Correct Structure
```cmake
# ✅ CORRECT CMake structure following actual IEEE hierarchy
add_library(ieee_802_1_as_2021 STATIC
    IEEE/802.1/AS/2021/core/gptp_state_machine.cpp
    IEEE/802.1/AS/2021/messages/sync_message.cpp
    IEEE/802.1/AS/2021/clock/best_master_clock.cpp
    IEEE/802.1/AS/2021/core/conformity_test_framework.cpp
)

add_library(ieee_1722_1_2021 STATIC
    IEEE/1722.1/2021/aem/entity_model.cpp
    IEEE/1722.1/2021/aecp/aem_command.cpp
    IEEE/1722.1/2021/descriptors/entity_descriptor.cpp
)

add_library(ieee_1722_2016 STATIC
    IEEE/1722/2016/avtp/avtp_packet.cpp
    IEEE/1722/2016/formats/aaf/audio_format.cpp
)

add_library(avnu_milan_v12 STATIC
    AVnu/Milan/v1.2/discovery/milan_discovery.cpp
    AVnu/Milan/v1.2/connection/redundant_streams.cpp
)

add_library(standards_common STATIC
    Common/interfaces/network_interface.cpp
    Common/utils/packet_parser.cpp
    Common/testing/test_framework_base.cpp
)

# IEEE standards libraries can depend on each other following IEEE layering
target_link_libraries(ieee_1722_1_2021
    ieee_1722_2016               # AVDECC depends on AVTP
    ieee_802_1_as_2021          # AVDECC depends on gPTP
    standards_common            # All can use Common utilities
)
```

### Documentation Structure Requirements - Corrected
```cpp
/**
 * @file conformity_test_framework.h
 * @brief IEEE 802.1AS-2021 Conformity Testing Framework
 * @namespace IEEE::_802_1::AS::_2021::Testing
 * 
 * Implements conformity testing according to IEEE 802.1AS-2021 specification.
 * This namespace contains all testing functionality for validating IEEE 802.1AS-2021
 * compliance including state machine behavior, timing requirements, and interoperability.
 * 
 * @see IEEE 802.1AS-2021, Clauses 11.2-11.5 "Conformance requirements"
 * @see IEEE 802.1AS-2021, Annex A "Implementation conformance statement (ICS)"
 */
```

### Enforcement Rules - Corrected with Copyright Compliance
1. **IEEE namespaces are top-level** - not wrapped in `openavnu::standards`
2. **Namespace must match folder structure** exactly (`IEEE::_802_1::AS::_2021` = `IEEE/802.1/AS/2021/`)
3. **Version numbers use underscores** in namespaces (`_2021`, `_2016`) to avoid conflicts
4. **Dots become underscores** in namespaces (`802.1` becomes `_802_1`, `1722.1` becomes `_1722_1`)
5. **No hardware vendors** in IEEE namespace hierarchy
6. **No OS-specific namespaces** in IEEE standards
7. **IEEE layering respected** - higher layer standards can depend on lower layers
8. **Cross-standard utilities** only in Common namespace
9. **Conformance testing** isolated in Testing sub-namespaces
10. **COPYRIGHT COMPLIANCE MANDATORY**:
    - **NEVER reproduce copyrighted specification content** in source code or documentation
    - **Reference specifications by section number only** (e.g., "IEEE 1722.1-2021, Section 7.2.1")
    - **Implement based on understanding**, not by copying specification text
    - **Use MCP-Server for compliance verification only**, not content reproduction
    - **Respect all copyright holders**: IEEE, AES, AVnu Alliance, ITU, etc.
    - **Include copyright disclaimer** in implementations referencing multiple standards
    - **Document original implementation** that achieves compliance through understanding

### Repository Copyright Policy
```cpp
// ✅ REQUIRED copyright notice for standards-based implementations
/*
 * This file implements protocol functionality based on understanding of:
 * - IEEE 1722.1-2021 (AVDECC) - Copyright IEEE
 * - IEEE 1722-2016 (AVTP) - Copyright IEEE  
 * - IEEE 802.1AS-2021 (gPTP) - Copyright IEEE
 * - Milan v1.2 - Copyright AVnu Alliance
 * 
 * No copyrighted content from these specifications is reproduced.
 * Implementation is original work achieving compliance through
 * understanding of specification requirements.
 * 
 * For authoritative requirements, refer to original specifications
 * available from respective standards organizations.
 */
```

This structure ensures clear separation of IEEE standards versions, prevents architectural violations, and maintains the hardware-agnostic principle while following the actual implementation pattern used in the codebase.

This architecture ensures the Standards layer remains pure, testable, reusable across different hardware platforms, and maintains strict IEEE compliance while following OpenAvnu's core development principles.

## ⚠️ MANDATORY: YAML Front Matter Schema Compliance

**CRITICAL ENFORCEMENT**: All specification files MUST use EXACT YAML front matter format per authoritative schemas.

**Authoritative Schema Sources** (SINGLE SOURCE OF TRUTH):
- Requirements: `spec-kit-templates/schemas/requirements-spec.schema.json`  
- Architecture: `spec-kit-templates/schemas/architecture-spec.schema.json`
- Design: `spec-kit-templates/schemas/ieee-design-spec.schema.json`
- Phase Gates: `spec-kit-templates/schemas/phase-gate-validation.schema.json`

**ZERO TOLERANCE POLICY**: 
- ❌ DO NOT modify schemas to fit incorrect front matter
- ❌ DO NOT use alternative YAML formats  
- ❌ DO NOT use full standard names where schemas expect short patterns
- ✅ ALWAYS reference authoritative schema files for format
- ✅ ALWAYS validate against schemas before submitting
- ✅ ALWAYS use phase-specific copilot-instructions for examples

**CI ENFORCEMENT**: Validation failures will block CI pipeline and prevent merges. There are no exceptions to schema compliance.