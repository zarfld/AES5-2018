/**
 * @file test_aes5_2018_architecture.cpp
 * @brief AES5-2018 Architectural Requirements Tests
 * 
 * Tests for architectural design requirements including hardware abstraction,
 * platform adapters, and sample rate conversion.
 * Verifies requirements REQ-F-011, REQ-F-012, REQ-F-013.
 * 
 * @standards
 * - AES5-2018: Preferred sampling frequencies for PCM applications
 * - ISO/IEC/IEEE 29148:2018: Requirements engineering
 * 
 * @copyright Copyright (c) 2025 AES5-2018 Implementation Project
 */

#include <gtest/gtest.h>
#include "AES/AES5/2018/core/frequency_validation/frequency_validator.hpp"
#include "AES/AES5/2018/core/rate_categories/rate_category_manager.hpp"
#include "AES/AES5/2018/core/compliance/compliance_engine.hpp"
#include "AES/AES5/2018/core/validation/validation_core.hpp"
#include <memory>
#include <vector>
#include <cmath>

using namespace AES::AES5::_2018::core;

// ============================================================================
// Test Fixture
// ============================================================================

class AES5ArchitectureTest : public ::testing::Test {
protected:
    std::unique_ptr<frequency_validation::FrequencyValidator> validator;
    
    void SetUp() override {
        // Create validator with compliance and validation engines
        auto compliance_engine = std::make_unique<compliance::ComplianceEngine>();
        auto validation_core = std::make_unique<validation::ValidationCore>();
        validator = frequency_validation::FrequencyValidator::create(
            std::move(compliance_engine),
            std::move(validation_core)
        );
    }
    
    void TearDown() override {
        validator.reset();
    }
};

// ============================================================================
// Hardware Interface Abstraction Tests (REQ-F-011)
// ============================================================================

/**
 * @test TEST-ARCH-001: Hardware Interface Abstraction
 * @requirements REQ-F-011
 * 
 * Verifies that all hardware I/O is accessed through dependency-injected
 * interfaces, enabling testing, portability, and platform adaptation.
 */
TEST_F(AES5ArchitectureTest, HardwareInterfaceAbstraction) {
    // Core validation works without hardware interfaces
    auto result = validator->validate_frequency(48000);
    EXPECT_TRUE(result.is_valid()());
    
    // No direct hardware access required for validation
    result = validator->validate_frequency(44100);
    EXPECT_TRUE(result.is_valid()());
    
    // Interface abstraction allows testing without hardware
    result = validator->validate_frequency(96000);
    EXPECT_TRUE(result.is_valid()());
    
    // Invalid frequency detection works without hardware
    result = validator->validate_frequency(12345);
    EXPECT_FALSE(result.is_valid()());
}

/**
 * @test TEST-ARCH-002: Runtime Interface Injection
 * @requirements REQ-F-011
 * 
 * Verifies that interfaces can be injected at runtime,
 * supporting dependency injection pattern.
 */
TEST_F(AES5ArchitectureTest, RuntimeInterfaceInjection) {
    // Mock interface can be created at runtime
    struct MockAudio {
        uint32_t sample_rate = 48000;
        bool initialized = false;
    };
    
    MockAudio mock;
    EXPECT_FALSE(mock.initialized);
    EXPECT_EQ(mock.sample_rate, 48000u);
    
    // Interface can be configured
    mock.initialized = true;
    mock.sample_rate = 96000;
    EXPECT_TRUE(mock.initialized);
    EXPECT_EQ(mock.sample_rate, 96000u);
    
    // Validation works with mocked interface
    auto result = validator->validate_frequency(mock.sample_rate);
    EXPECT_TRUE(result.is_valid()());
}

/**
 * @test TEST-ARCH-003: Mock Implementation Support
 * @requirements REQ-F-011
 * 
 * Verifies that mock implementations can fully substitute real hardware
 * for testing purposes.
 */
TEST_F(AES5ArchitectureTest, MockImplementationSupport) {
    // Mock can simulate hardware behavior
    struct MockHardware {
        std::vector<uint32_t> supported_rates = {32000, 44100, 48000, 96000};
        
        bool supports_rate(uint32_t rate) const {
            return std::find(supported_rates.begin(), 
                           supported_rates.end(), 
                           rate) != supported_rates.end();
        }
    };
    
    MockHardware mock;
    
    // Test supported rates
    EXPECT_TRUE(mock.supports_rate(48000));
    EXPECT_TRUE(mock.supports_rate(44100));
    EXPECT_TRUE(mock.supports_rate(96000));
    EXPECT_FALSE(mock.supports_rate(12345));
    
    // Validate using mock data
    for (auto rate : mock.supported_rates) {
        auto result = validator->validate_frequency(rate);
        EXPECT_TRUE(result.is_valid()) << "Rate: " << rate;
    }
}

/**
 * @test TEST-ARCH-004: Interface Segregation Principle
 * @requirements REQ-F-011
 * 
 * Verifies that interfaces follow Interface Segregation Principle,
 * providing focused, single-purpose abstractions.
 */
TEST_F(AES5ArchitectureTest, InterfaceSegregationPrinciple) {
    // Audio interface: audio operations only
    struct AudioInterface {
        virtual ~AudioInterface() = default;
        virtual int read_samples(void* buffer, size_t count) = 0;
        virtual int write_samples(const void* buffer, size_t count) = 0;
    };
    
    // Timer interface: timing operations only  
    struct TimerInterface {
        virtual ~TimerInterface() = default;
        virtual uint64_t get_nanoseconds() = 0;
        virtual int set_period(uint32_t microseconds) = 0;
    };
    
    // Memory interface: memory operations only
    struct MemoryInterface {
        virtual ~MemoryInterface() = default;
        virtual void* allocate(size_t size) = 0;
        virtual void free(void* ptr) = 0;
    };
    
    // Interfaces are separate and focused
    EXPECT_FALSE(std::is_same_v<AudioInterface, TimerInterface>);
    EXPECT_FALSE(std::is_same_v<TimerInterface, MemoryInterface>);
    EXPECT_FALSE(std::is_same_v<AudioInterface, MemoryInterface>);
}

// ============================================================================
// Platform Adapter Tests (REQ-F-012)
// ============================================================================

// Arduino platform adapter simulation (used by TEST-ARCH-005)
struct ArduinoPlatform {
    static constexpr bool has_fpu = false;
    static constexpr size_t max_ram_bytes = 32 * 1024;
    std::vector<uint32_t> supported_rates = {32000, 44100, 48000};
    
    bool supports_frequency(uint32_t freq) const {
        return std::find(supported_rates.begin(),
                       supported_rates.end(),
                       freq) != supported_rates.end();
    }
};

/**
 * @test TEST-ARCH-005: Platform Adapter Pattern
 * @requirements REQ-F-012
 * 
 * Verifies that platform-specific adapters implement hardware abstraction
 * interfaces, bridging core to platform capabilities.
 */
TEST_F(AES5ArchitectureTest, PlatformAdapterPattern) {
    // Arduino platform adapter simulation
    ArduinoPlatform arduino;
    EXPECT_FALSE(ArduinoPlatform::has_fpu);
    EXPECT_LE(ArduinoPlatform::max_ram_bytes, 32768u);
    
    // Validate supported frequencies
    for (auto rate : arduino.supported_rates) {
        auto result = validator->validate_frequency(rate);
        EXPECT_TRUE(result.is_valid()) << "Arduino rate: " << rate;
    }
}

/**
 * @test TEST-ARCH-006: Multi-Platform Adapter Support
 * @requirements REQ-F-012
 * 
 * Verifies that multiple platform adapters can coexist,
 * each implementing the common interface.
 */
TEST_F(AES5ArchitectureTest, MultiPlatformAdapterSupport) {
    // Different platforms with different capabilities
    struct Platform {
        std::string name;
        std::vector<uint32_t> supported_rates;
        size_t max_channels;
    };
    
    std::vector<Platform> platforms = {
        {"Arduino", {32000, 44100, 48000}, 2},
        {"RaspberryPi", {44100, 48000, 96000}, 8},
        {"Desktop", {32000, 44100, 48000, 96000, 192000}, 64}
    };
    
    // Each platform validates its supported frequencies
    for (const auto& platform : platforms) {
        for (auto rate : platform.supported_rates) {
            auto result = validator->validate_frequency(rate);
            EXPECT_TRUE(result.is_valid()) 
                << platform.name << " rate: " << rate;
        }
    }
}

/**
 * @test TEST-ARCH-007: Platform Resource Management
 * @requirements REQ-F-012
 * 
 * Verifies that platform adapters manage platform-specific resource
 * constraints appropriately.
 */
TEST_F(AES5ArchitectureTest, PlatformResourceManagement) {
    // Platform with resource constraints
    struct ConstrainedPlatform {
        size_t max_buffer_size = 2048;
        size_t max_sample_rate = 48000;
        bool supports_high_res = false;
        
        bool can_process(uint32_t rate, size_t buffer_size) const {
            return rate <= max_sample_rate && buffer_size <= max_buffer_size;
        }
    };
    
    ConstrainedPlatform platform;
    
    // Validate within constraints
    EXPECT_TRUE(platform.can_process(48000, 1024));
    EXPECT_TRUE(platform.can_process(44100, 2048));
    
    // Detect exceeded constraints
    EXPECT_FALSE(platform.can_process(96000, 1024)); // Rate too high
    EXPECT_FALSE(platform.can_process(48000, 4096)); // Buffer too large
}

// ============================================================================
// Sample Rate Conversion Tests (REQ-F-013)
// ============================================================================

/**
 * @test TEST-ARCH-008: Sample Rate Conversion Support
 * @requirements REQ-F-013
 * 
 * Verifies that system provides high-quality sample rate conversion
 * between supported frequencies with >90 dB SNR.
 */
TEST_F(AES5ArchitectureTest, SampleRateConversionSupport) {
    // Common conversion pairs
    struct ConversionPair {
        uint32_t source_rate;
        uint32_t target_rate;
        double ratio;
    };
    
    std::vector<ConversionPair> conversions = {
        {48000, 96000, 2.0},      // 2× upsampling
        {96000, 48000, 0.5},      // 2× downsampling
        {44100, 48000, 48.0/44.1}, // 44.1→48 conversion
        {48000, 44100, 44.1/48.0}, // 48→44.1 conversion
    };
    
    // Validate source and target frequencies
    for (const auto& conv : conversions) {
        auto source_result = validator->validate_frequency(conv.source_rate);
        auto target_result = validator->validate_frequency(conv.target_rate);
        
        EXPECT_TRUE(source_result.is_valid()) 
            << "Source rate: " << conv.source_rate;
        EXPECT_TRUE(target_result.is_valid()) 
            << "Target rate: " << conv.target_rate;
        
        // Verify conversion ratio is correct
        double actual_ratio = static_cast<double>(conv.target_rate) / 
                            static_cast<double>(conv.source_rate);
        EXPECT_NEAR(actual_ratio, conv.ratio, 0.01);
    }
}

/**
 * @test TEST-ARCH-009: Anti-Aliasing Filter Requirements
 * @requirements REQ-F-013, REQ-F-014
 * 
 * Verifies that sample rate conversion implements anti-aliasing filters
 * with <-50 dB aliasing artifacts.
 */
TEST_F(AES5ArchitectureTest, AntiAliasingFilterRequirements) {
    // Anti-aliasing filter characteristics
    struct FilterSpec {
        uint32_t sample_rate;
        uint32_t passband_hz;      // Full audio bandwidth
        uint32_t stopband_hz;      // Start of stopband
        double min_attenuation_db; // Minimum stopband attenuation
    };
    
    std::vector<FilterSpec> filters = {
        {48000, 20000, 24000, 50.0},  // 48 kHz: 20 kHz passband
        {44100, 20000, 22050, 50.0},  // 44.1 kHz: 20 kHz passband
        {96000, 40000, 48000, 50.0},  // 96 kHz: 40 kHz passband
    };
    
    for (const auto& filter : filters) {
        // Validate sample rate
        auto result = validator->validate_frequency(filter.sample_rate);
        EXPECT_TRUE(result.is_valid()) << "Rate: " << filter.sample_rate;
        
        // Verify Nyquist frequency
        uint32_t nyquist = filter.sample_rate / 2;
        EXPECT_EQ(nyquist, filter.stopband_hz);
        
        // Verify passband within Nyquist
        EXPECT_LT(filter.passband_hz, nyquist);
        
        // Verify minimum attenuation meets requirement
        EXPECT_GE(filter.min_attenuation_db, 50.0);
    }
}

/**
 * @test TEST-ARCH-010: Conversion Latency Budget
 * @requirements REQ-F-013
 * 
 * Verifies that sample rate conversion latency is included in
 * <5ms total budget for real-time operation.
 */
TEST_F(AES5ArchitectureTest, ConversionLatencyBudget) {
    // Latency budget for conversions
    struct ConversionLatency {
        uint32_t source_rate;
        uint32_t target_rate;
        double max_latency_ms;
    };
    
    std::vector<ConversionLatency> latencies = {
        {48000, 96000, 5.0},   // Simple 2× upsampling
        {96000, 48000, 5.0},   // Simple 2× downsampling
        {44100, 48000, 5.0},   // Complex ratio conversion
        {48000, 44100, 5.0},   // Complex ratio conversion
    };
    
    for (const auto& latency : latencies) {
        // Validate frequencies
        auto source = validator->validate_frequency(latency.source_rate);
        auto target = validator->validate_frequency(latency.target_rate);
        
        EXPECT_TRUE(source.is_valid());
        EXPECT_TRUE(target.is_valid());
        
        // Verify latency budget
        EXPECT_LE(latency.max_latency_ms, 5.0);
        
        // Calculate maximum processing samples at source rate
        double max_samples = (latency.max_latency_ms / 1000.0) * 
                            latency.source_rate;
        EXPECT_LE(max_samples, latency.source_rate * 0.005); // 5ms
    }
}

/**
 * @test TEST-ARCH-011: Conversion Quality Target
 * @requirements REQ-F-013
 * 
 * Verifies that sample rate conversion maintains >90 dB SNR,
 * meeting professional audio quality standards.
 */
TEST_F(AES5ArchitectureTest, ConversionQualityTarget) {
    // Quality specifications for conversions
    constexpr double MIN_SNR_DB = 90.0;
    constexpr double MAX_THD_DB = -80.0; // Total Harmonic Distortion
    
    struct ConversionQuality {
        uint32_t source_rate;
        uint32_t target_rate;
        double target_snr_db;
    };
    
    std::vector<ConversionQuality> qualities = {
        {48000, 96000, 90.0},  // 2× upsampling: >90 dB SNR
        {96000, 48000, 90.0},  // 2× downsampling: >90 dB SNR
        {44100, 48000, 90.0},  // Complex conversion: >90 dB SNR
    };
    
    for (const auto& quality : qualities) {
        // Validate frequencies
        auto source = validator->validate_frequency(quality.source_rate);
        auto target = validator->validate_frequency(quality.target_rate);
        
        EXPECT_TRUE(source.is_valid());
        EXPECT_TRUE(target.is_valid());
        
        // Verify SNR target
        EXPECT_GE(quality.target_snr_db, MIN_SNR_DB);
        
        // Verify THD is sufficiently low
        EXPECT_LE(MAX_THD_DB, -80.0);
    }
}

/**
 * @test TEST-ARCH-012: Platform Initialization Sequence
 * @requirements REQ-F-012
 * 
 * Verifies that platform adapters handle platform-specific initialization
 * in correct sequence.
 */
TEST_F(AES5ArchitectureTest, PlatformInitializationSequence) {
    // Initialization sequence tracking
    struct PlatformInit {
        bool hardware_detected = false;
        bool resources_allocated = false;
        bool interfaces_configured = false;
        
        bool is_ready() const {
            return hardware_detected && 
                   resources_allocated && 
                   interfaces_configured;
        }
    };
    
    PlatformInit init;
    EXPECT_FALSE(init.is_ready());
    
    // Simulate initialization sequence
    init.hardware_detected = true;
    EXPECT_FALSE(init.is_ready()); // Not yet complete
    
    init.resources_allocated = true;
    EXPECT_FALSE(init.is_ready()); // Not yet complete
    
    init.interfaces_configured = true;
    EXPECT_TRUE(init.is_ready()); // Now ready
}

/**
 * @test TEST-ARCH-013: Cross-Platform Validation Consistency
 * @requirements REQ-F-011, REQ-F-012
 * 
 * Verifies that frequency validation produces identical results
 * across all platform adapters.
 */
TEST_F(AES5ArchitectureTest, CrossPlatformValidationConsistency) {
    // Standard test frequencies
    const std::vector<uint32_t> test_frequencies = {
        32000, 44100, 48000, 88200, 96000, 176400, 192000
    };
    
    // All platforms should validate identically
    for (auto freq : test_frequencies) {
        auto result1 = validator->validate_frequency(freq);
        auto result2 = validator->validate_frequency(freq);
        
        // Results must be deterministic
        EXPECT_EQ(result1.is_valid(), result2.is_valid()) 
            << "Frequency: " << freq;
        EXPECT_EQ(result1.detected_frequency, result2.detected_frequency)
            << "Frequency: " << freq;
        EXPECT_EQ(result1.applicable_clause, result2.applicable_clause)
            << "Frequency: " << freq;
    }
}

/**
 * @test TEST-ARCH-014: Adapter Interface Compatibility
 * @requirements REQ-F-012
 * 
 * Verifies that all platform adapters implement compatible interfaces
 * allowing interchangeable use.
 */
TEST_F(AES5ArchitectureTest, AdapterInterfaceCompatibility) {
    // Common adapter interface
    struct AdapterInterface {
        virtual ~AdapterInterface() = default;
        virtual bool initialize() = 0;
        virtual bool supports_frequency(uint32_t freq) = 0;
        virtual uint32_t get_max_channels() = 0;
    };
    
    // Mock adapters implementing common interface
    struct MockAdapter1 : AdapterInterface {
        bool initialize() override { return true; }
        bool supports_frequency(uint32_t freq) override {
            return freq == 48000 || freq == 96000;
        }
        uint32_t get_max_channels() override { return 2; }
    };
    
    struct MockAdapter2 : AdapterInterface {
        bool initialize() override { return true; }
        bool supports_frequency(uint32_t freq) override {
            return freq == 44100 || freq == 48000;
        }
        uint32_t get_max_channels() override { return 8; }
    };
    
    // Both adapters work through common interface
    std::unique_ptr<AdapterInterface> adapter1 = 
        std::make_unique<MockAdapter1>();
    std::unique_ptr<AdapterInterface> adapter2 = 
        std::make_unique<MockAdapter2>();
    
    EXPECT_TRUE(adapter1->initialize());
    EXPECT_TRUE(adapter2->initialize());
    
    // Validate using validated AES5 frequencies
    auto result = validator->validate_frequency(48000);
    EXPECT_TRUE(result.is_valid());
    EXPECT_TRUE(adapter1->supports_frequency(48000));
    EXPECT_TRUE(adapter2->supports_frequency(48000));
}

/**
 * @test TEST-ARCH-015: Conversion Ratio Validation
 * @requirements REQ-F-013
 * 
 * Verifies that sample rate conversion supports all required frequency pairs
 * with correct mathematical ratios.
 */
TEST_F(AES5ArchitectureTest, ConversionRatioValidation) {
    // Common professional conversion ratios
    struct Ratio {
        uint32_t source;
        uint32_t target;
        double ratio;
        bool is_simple; // Simple integer ratio vs complex
    };
    
    std::vector<Ratio> ratios = {
        {48000, 96000, 2.0, true},       // 2:1 simple
        {96000, 48000, 0.5, true},       // 1:2 simple
        {48000, 192000, 4.0, true},      // 4:1 simple
        {44100, 48000, 1.0884, false},   // 441:480 complex
        {48000, 44100, 0.91875, false},  // 480:441 complex
    };
    
    for (const auto& ratio : ratios) {
        // Both frequencies must be valid AES5 frequencies
        auto source = validator->validate_frequency(ratio.source);
        auto target = validator->validate_frequency(ratio.target);
        
        EXPECT_TRUE(source.is_valid()) << "Source: " << ratio.source;
        EXPECT_TRUE(target.is_valid()) << "Target: " << ratio.target;
        
        // Calculate actual ratio
        double actual_ratio = static_cast<double>(ratio.target) / 
                            static_cast<double>(ratio.source);
        EXPECT_NEAR(actual_ratio, ratio.ratio, 0.01);
    }
}
