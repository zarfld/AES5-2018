# AES5-2018 Standards Implementation Directory

This directory contains the hardware-agnostic implementation of the AES5-2018 standard for preferred sampling frequencies in professional digital audio applications employing pulse-code modulation (PCM).

## Directory Structure

The implementation follows the standards-compliant namespace pattern:

```
lib/Standards/AES/AES5/2018/
├── core/                           # Core sampling frequency management
│   ├── frequency_validation/       # Primary/other frequency validation
│   ├── rate_categories/           # Basic, double, quadruple, etc.
│   └── legacy_support/            # Legacy sampling frequencies
├── conversion/                     # Sampling frequency conversion
│   ├── integer_ratio/             # Simple ratio conversions (48:32, 3:2)
│   ├── complex_ratio/             # Complex ratio conversions (441:320)
│   └── anti_aliasing/             # Bandwidth limiting filters (>50dB)
├── video_sync/                     # Video synchronization support
│   ├── integer_frames/            # Integer ratio video systems
│   ├── ntsc_frames/               # Non-integer ratio (NTSC) systems
│   └── pull_variants/             # Pull-up/pull-down (1001/1000)
└── utilities/                      # AES5-2018 utility functions
    ├── calculations/              # Samples per frame calculations
    └── precision/                 # High-precision arithmetic
```

## AES5-2018 Implementation Compliance

This implementation provides complete coverage of the AES5-2018 specification:

### Section 5.1: Primary Sampling Frequency
- **48 kHz**: Primary frequency with optimized processing paths
- Highest performance and lowest latency for 48 kHz
- Default behavior when sample rate not specified
- All conversion algorithms optimized for 48 kHz as hub frequency

### Section 5.2: Other Sampling Frequencies  
- **44.1 kHz**: Consumer applications, compact disc compatibility
- **32 kHz**: Legacy broadcast and transmission systems
- **96 kHz**: High-bandwidth applications (>20 kHz audio)
- High-quality conversion with artifacts below -80 dB THD+N

### Section 5.3: Multiple Sampling Frequencies
- **2× rates**: 88.2 kHz, 96 kHz
- **4× rates**: 176.4 kHz, 192 kHz  
- **8× rates**: 352.8 kHz, 384 kHz
- Efficient conversion between integer multiples
- Optimized algorithms for factor-of-N conversions

### Section 4.1-4.2: Audio Bandwidth and Conversion
- Nyquist-Shannon sampling theorem compliance
- Bandwidth limiting to half sampling frequency
- Anti-aliasing filters with >50 dB attenuation
- Support for simple integer ratio conversions (48:32 = 3:2)
- Complex ratio conversions (441:320) with quality preservation

### Annex A: Video Synchronization
- Samples per picture frame calculations
- Integer ratio video systems (24, 25, 30, 50, 60 fps)
- Non-integer ratio NTSC systems (23.98, 29.97, 59.94 fps)
- Pull-up/pull-down variants (1001/1000 factor)

## C++ Namespace Structure

All AES5-2018 functionality is organized under the namespace hierarchy:

```cpp
namespace AES {
namespace AES5 {
namespace _2018 {
    namespace core {
        namespace frequency_validation { /* Primary/other freq validation */ }
        namespace rate_categories { /* Basic/double/quad/octuple rates */ }
    }
    namespace conversion {
        namespace integer_ratio { /* Simple ratio conversions */ }
        namespace complex_ratio { /* Complex ratio conversions */ }
        namespace anti_aliasing { /* Bandwidth limiting filters */ }
    }
    namespace video_sync {
        namespace integer_frames { /* Integer ratio video systems */ }
        namespace ntsc_frames { /* NTSC non-integer ratios */ }
        namespace pull_variants { /* Pull-up/down (1001/1000) */ }
    }
    namespace utilities {
        namespace calculations { /* Samples per frame calc */ }
        namespace precision { /* High-precision arithmetic */ }
    }
}}}
```

## Hardware Abstraction

The Standards layer maintains strict hardware independence through:

- **No direct hardware calls**: All hardware access through interfaces
- **Dependency injection**: Hardware abstraction provided via function pointers
- **Platform agnostic**: Compilable without vendor drivers or OS headers
- **Mockable/testable**: Runnable without physical audio hardware

### Interface Pattern Example
```cpp
// Hardware abstraction interface (in Common/interfaces/)
typedef struct {
    int (*send_audio_frame)(const void* frame_data, size_t length);
    int (*receive_audio_frame)(void* buffer, size_t* length);
    uint64_t (*get_sample_clock_ns)(void);
} audio_interface_t;

// AES5-2018 implementation receives interface
int aes5_frequency_manager_init(const audio_interface_t* audio_interface);
```

## Build Integration

The Standards layer compiles independently as a static library:

```cmake
add_library(aes5_standards STATIC
    # AES5-2018 core sampling frequency management
    AES/AES5/2018/core/frequency_validation/*.cpp
    AES/AES5/2018/core/rate_categories/*.cpp
    # ... (no hardware dependencies)
)
```

## Compliance Testing

Each AES5-2018 feature includes compliance validation:

- **Section-specific test suites**: Validate each AES5-2018 clause
- **Frequency accuracy testing**: <0.01% tolerance verification
- **Conversion quality testing**: -80 dB THD+N artifact limits
- **Performance benchmarking**: Real-time processing requirements
- **Cross-platform validation**: Embedded and desktop compatibility

For implementation guidelines, see the root `copilot-instructions.md` file.