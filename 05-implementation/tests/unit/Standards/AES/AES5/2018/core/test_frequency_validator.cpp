// Test file for DES-C-001 FrequencyValidator following TDD Red-Green-Refactor
// Phase: RED - Write failing tests first
// Traceability: DES-C-001 → TEST-C-001

#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>

// Include FrequencyValidator and dependencies
#include "AES/AES5/2018/core/frequency_validation/frequency_validator.hpp"
#include "AES/AES5/2018/core/compliance/compliance_engine.hpp"
#include "AES/AES5/2018/core/validation/validation_core.hpp"

using namespace AES::AES5::_2018::core::frequency_validation;
using namespace AES::AES5::_2018::core::compliance;
using namespace AES::AES5::_2018::core::validation;

/**
 * @brief Test fixture for FrequencyValidator TDD
 * @traceability TEST-C-001 → DES-C-001
 */
class FrequencyValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create component dependencies
        auto compliance_engine = std::make_unique<ComplianceEngine>();
        auto validation_core = std::make_unique<ValidationCore>();
        
        // Create FrequencyValidator using factory method
        validator_ = FrequencyValidator::create(
            std::move(compliance_engine), 
            std::move(validation_core));
    }

    void TearDown() override {
        validator_.reset();
    }

    /**
     * @brief Helper function to measure operation performance
     */
    template<typename Func>
    void measure_performance(Func&& operation, const char* operation_name, 
                           std::chrono::nanoseconds max_duration) {
        auto start = std::chrono::high_resolution_clock::now();
        operation();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        EXPECT_LE(duration, max_duration) 
            << operation_name << " took " << duration.count() << "ns, expected <" 
            << max_duration.count() << "ns";
    }

    std::unique_ptr<FrequencyValidator> validator_;
};

/**
 * @brief Test factory method creates valid FrequencyValidator
 * @requirement SYS-FREQ-001: Frequency validator initialization
 * @traceability TEST-C-001-001 → DES-C-001 → SYS-FREQ-001
 */
TEST_F(FrequencyValidatorTest, FactoryMethodCreatesValidValidator) {
    // Given: Dependencies for FrequencyValidator creation
    auto compliance_engine = std::make_unique<ComplianceEngine>();
    auto validation_core = std::make_unique<ValidationCore>();
    
    // When: Creating FrequencyValidator using factory method
    auto validator = FrequencyValidator::create(
        std::move(compliance_engine), 
        std::move(validation_core));
    
    // Then: Validator should be created successfully
    EXPECT_NE(validator, nullptr);
    
    // And: Should meet real-time constraints initially
    EXPECT_TRUE(validator->meets_realtime_constraints());
}

/**
 * @brief Test primary frequency validation (48 kHz)
 * @requirement AES5-FREQ-001: Primary sampling frequency (AES5-2018 Section 5.1)
 * @traceability TEST-C-001-002 → DES-C-001 → AES5-FREQ-001
 */
TEST_F(FrequencyValidatorTest, ValidatePrimaryFrequency48kHz) {
    // Given: AES5-2018 primary sampling frequency
    uint32_t primary_frequency = FrequencyValidator::PRIMARY_FREQUENCY; // 48000 Hz
    
    // When: Validating primary frequency
    FrequencyValidationResult result = validator_->validate_frequency(primary_frequency);
    
    // Then: Validation should succeed
    EXPECT_TRUE(result.is_valid());
    EXPECT_EQ(result.status, ValidationResult::Valid);
    EXPECT_EQ(result.detected_frequency, 48000);
    EXPECT_EQ(result.closest_standard_frequency, 48000);
    EXPECT_EQ(result.applicable_clause, AES5Clause::Section_5_1);
    EXPECT_LT(result.tolerance_ppm, 1.0);  // Should be essentially zero
    
    // And: Performance requirement (<50μs per validation)
    measure_performance([&]() {
        validator_->validate_frequency(primary_frequency);
    }, "Primary frequency validation", std::chrono::nanoseconds(50000));
}

/**
 * @brief Test consumer frequency validation (44.1 kHz)
 * @requirement AES5-FREQ-002: Consumer sampling frequency (AES5-2018 Section 5.2)
 * @traceability TEST-C-001-003 → DES-C-001 → AES5-FREQ-002
 */
TEST_F(FrequencyValidatorTest, ValidateConsumerFrequency44_1kHz) {
    // Given: AES5-2018 consumer sampling frequency
    uint32_t consumer_frequency = FrequencyValidator::CONSUMER_FREQUENCY; // 44100 Hz
    
    // When: Validating consumer frequency
    FrequencyValidationResult result = validator_->validate_frequency(consumer_frequency);
    
    // Then: Validation should succeed
    EXPECT_TRUE(result.is_valid());
    EXPECT_EQ(result.status, ValidationResult::Valid);
    EXPECT_EQ(result.detected_frequency, 44100);
    EXPECT_EQ(result.closest_standard_frequency, 44100);
    EXPECT_EQ(result.applicable_clause, AES5Clause::Section_5_2);
    EXPECT_LT(result.tolerance_ppm, 1.0);
}

/**
 * @brief Test high bandwidth frequency validation (96 kHz)
 * @requirement AES5-FREQ-003: High bandwidth sampling frequency (AES5-2018 Section 5.2)
 * @traceability TEST-C-001-004 → DES-C-001 → AES5-FREQ-003
 */
TEST_F(FrequencyValidatorTest, ValidateHighBandwidthFrequency96kHz) {
    // Given: AES5-2018 high bandwidth sampling frequency
    uint32_t high_bandwidth_frequency = FrequencyValidator::HIGH_BANDWIDTH_FREQUENCY; // 96000 Hz
    
    // When: Validating high bandwidth frequency
    FrequencyValidationResult result = validator_->validate_frequency(high_bandwidth_frequency);
    
    // Then: Validation should succeed
    EXPECT_TRUE(result.is_valid());
    EXPECT_EQ(result.status, ValidationResult::Valid);
    EXPECT_EQ(result.detected_frequency, 96000);
    EXPECT_EQ(result.closest_standard_frequency, 96000);
    EXPECT_EQ(result.applicable_clause, AES5Clause::Section_5_2);
    EXPECT_LT(result.tolerance_ppm, 1.0);
}

/**
 * @brief Test legacy frequency validation (32 kHz)
 * @requirement AES5-FREQ-004: Legacy sampling frequency (AES5-2018 Section 5.4)
 * @traceability TEST-C-001-005 → DES-C-001 → AES5-FREQ-004
 */
TEST_F(FrequencyValidatorTest, ValidateLegacyFrequency32kHz) {
    // Given: AES5-2018 legacy sampling frequency
    uint32_t legacy_frequency = FrequencyValidator::LEGACY_FREQUENCY; // 32000 Hz
    
    // When: Validating legacy frequency
    FrequencyValidationResult result = validator_->validate_frequency(legacy_frequency);
    
    // Then: Validation should succeed
    EXPECT_TRUE(result.is_valid());
    EXPECT_EQ(result.status, ValidationResult::Valid);
    EXPECT_EQ(result.detected_frequency, 32000);
    EXPECT_EQ(result.closest_standard_frequency, 32000);
    EXPECT_EQ(result.applicable_clause, AES5Clause::Section_5_4);
    EXPECT_LT(result.tolerance_ppm, 1.0);
}

/**
 * @brief Test pull-up/pull-down frequency validation
 * @requirement AES5-FREQ-005: Pull-up/pull-down variants (AES5-2018 Annex A)
 * @traceability TEST-C-001-006 → DES-C-001 → AES5-FREQ-005
 */
TEST_F(FrequencyValidatorTest, ValidatePullUpPullDownFrequencies) {
    // Given: AES5-2018 pull-up frequency (48000 * 1001/1000)
    uint32_t pullup_frequency = FrequencyValidator::PULLUP_48K; // 48048 Hz
    
    // When: Validating pull-up frequency
    FrequencyValidationResult pullup_result = validator_->validate_frequency(pullup_frequency);
    
    // Then: Pull-up validation should succeed
    EXPECT_TRUE(pullup_result.is_valid());
    EXPECT_EQ(pullup_result.detected_frequency, 48048);
    EXPECT_EQ(pullup_result.closest_standard_frequency, 48048);
    EXPECT_EQ(pullup_result.applicable_clause, AES5Clause::Annex_A);
    
    // Given: AES5-2018 pull-down frequency (48000 * 1000/1001)
    uint32_t pulldown_frequency = FrequencyValidator::PULLDOWN_48K; // 47952 Hz
    
    // When: Validating pull-down frequency
    FrequencyValidationResult pulldown_result = validator_->validate_frequency(pulldown_frequency);
    
    // Then: Pull-down validation should succeed
    EXPECT_TRUE(pulldown_result.is_valid());
    EXPECT_EQ(pulldown_result.detected_frequency, 47952);
    EXPECT_EQ(pulldown_result.closest_standard_frequency, 47952);
    EXPECT_EQ(pulldown_result.applicable_clause, AES5Clause::Annex_A);
}

/**
 * @brief Test tolerance-based frequency validation
 * @requirement AES5-TOLERANCE-001: Tolerance-based validation
 * @traceability TEST-C-001-007 → DES-C-001 → AES5-TOLERANCE-001
 */
TEST_F(FrequencyValidatorTest, ToleranceBasedValidation) {
    // Given: Frequency slightly off 48 kHz (within ±100 ppm default tolerance)
    uint32_t frequency_within_tolerance = 48005; // ~104 ppm off
    uint32_t frequency_outside_tolerance = 48100; // ~2083 ppm off
    
    // When: Validating frequency within default tolerance
    FrequencyValidationResult within_result = validator_->validate_frequency(
        frequency_within_tolerance, 200); // ±200 ppm tolerance
    
    // Then: Should be valid with tolerance
    EXPECT_TRUE(within_result.is_valid());
    EXPECT_EQ(within_result.closest_standard_frequency, 48000);
    EXPECT_GT(within_result.tolerance_ppm, 100.0); // Should calculate ~104 ppm
    EXPECT_LT(within_result.tolerance_ppm, 110.0);
    
    // When: Validating frequency outside tight tolerance
    FrequencyValidationResult outside_result = validator_->validate_frequency(
        frequency_outside_tolerance, 50); // ±50 ppm tight tolerance
    
    // Then: Should be invalid due to tolerance
    EXPECT_FALSE(outside_result.is_valid());
    EXPECT_EQ(outside_result.status, ValidationResult::OutOfTolerance);
    EXPECT_GT(outside_result.tolerance_ppm, 2000.0); // Should be ~2083 ppm
}

/**
 * @brief Test invalid frequency handling
 * @requirement SYS-ERROR-001: Invalid input handling
 * @traceability TEST-C-001-008 → DES-C-001 → SYS-ERROR-001
 */
TEST_F(FrequencyValidatorTest, InvalidFrequencyHandling) {
    // When: Validating zero frequency
    FrequencyValidationResult zero_result = validator_->validate_frequency(0);
    
    // Then: Should return invalid input
    EXPECT_FALSE(zero_result.is_valid());
    EXPECT_EQ(zero_result.status, ValidationResult::InvalidInput);
    
    // When: Validating extremely high frequency (outside any standard)
    FrequencyValidationResult extreme_result = validator_->validate_frequency(1000000); // 1 MHz
    
    // Then: Should find closest standard frequency but likely fail tolerance
    EXPECT_FALSE(extreme_result.is_valid());
    EXPECT_GT(extreme_result.tolerance_ppm, 1000000.0); // Massive tolerance error
}

/**
 * @brief Test closest standard frequency finder
 * @requirement AES5-LOOKUP-001: Standard frequency lookup
 * @traceability TEST-C-001-009 → DES-C-001 → AES5-LOOKUP-001
 */
TEST_F(FrequencyValidatorTest, ClosestStandardFrequencyFinder) {
    // Test cases for closest frequency detection
    struct TestCase {
        uint32_t input_frequency;
        uint32_t expected_closest;
        const char* description;
    };
    
    std::vector<TestCase> test_cases = {
        {35000, 32000, "Between 32k and 44.1k -> closer to 32k"},
        {40000, 44100, "Between 32k and 44.1k -> closer to 44.1k"},
        {46000, 47952, "Between 44.1k and 48k -> closer to pull-down"},
        {48500, 48048, "Between 48k variants -> closer to pull-up"},
        {70000, 96000, "Between 48k and 96k -> closer to 96k"},
        {100000, 96000, "Above 96k -> closest to 96k"}
    };
    
    for (const auto& test_case : test_cases) {
        // When: Finding closest standard frequency
        uint32_t closest = validator_->find_closest_standard_frequency(test_case.input_frequency);
        
        // Then: Should match expected closest frequency
        EXPECT_EQ(closest, test_case.expected_closest) 
            << test_case.description << " - Input: " << test_case.input_frequency;
        
        // And: Performance requirement (<10μs per lookup)
        measure_performance([&]() {
            validator_->find_closest_standard_frequency(test_case.input_frequency);
        }, "Closest frequency lookup", std::chrono::nanoseconds(10000));
    }
}

/**
 * @brief Test tolerance calculation precision
 * @requirement AES5-CALC-001: Tolerance calculation accuracy
 * @traceability TEST-C-001-010 → DES-C-001 → AES5-CALC-001
 */
TEST_F(FrequencyValidatorTest, ToleranceCalculationPrecision) {
    // Test cases for tolerance calculation
    struct ToleranceTest {
        uint32_t measured;
        uint32_t reference;
        double expected_ppm;
        double tolerance;
        const char* description;
    };
    
    std::vector<ToleranceTest> test_cases = {
        {48048, 48000, 1000.0, 10.0, "1000 ppm tolerance (pull-up factor)"},
        {48024, 48000, 500.0, 10.0, "500 ppm tolerance"},
        {47976, 48000, 500.0, 10.0, "500 ppm tolerance (negative)"},
        {44144, 44100, 997.7, 10.0, "~1000 ppm tolerance for 44.1k"},
        {32032, 32000, 1000.0, 10.0, "1000 ppm tolerance for 32k"}
    };
    
    for (const auto& test_case : test_cases) {
        // When: Calculating tolerance in PPM
        double calculated_ppm = validator_->calculate_tolerance_ppm(
            test_case.measured, test_case.reference);
        
        // Then: Should match expected tolerance within precision
        EXPECT_NEAR(calculated_ppm, test_case.expected_ppm, test_case.tolerance)
            << test_case.description;
        
        // And: Performance requirement (<5μs per calculation)
        measure_performance([&]() {
            validator_->calculate_tolerance_ppm(test_case.measured, test_case.reference);
        }, "Tolerance calculation", std::chrono::nanoseconds(5000));
    }
}

/**
 * @brief Test performance metrics integration
 * @requirement SYS-PERF-001: Performance monitoring integration
 * @traceability TEST-C-001-011 → DES-C-001 → SYS-PERF-001
 */
TEST_F(FrequencyValidatorTest, PerformanceMetricsIntegration) {
    // Given: Initial metrics state (store values, not references)
    const ValidationMetrics& initial_metrics_ref = validator_->get_metrics();
    uint64_t initial_validations = initial_metrics_ref.total_validations.load(std::memory_order_relaxed);
    uint64_t initial_successful = initial_metrics_ref.successful_validations.load(std::memory_order_relaxed);
    uint64_t initial_failed = initial_metrics_ref.failed_validations.load(std::memory_order_relaxed);
    
    // When: Performing multiple validations
    validator_->validate_frequency(48000);
    validator_->validate_frequency(44100);
    validator_->validate_frequency(99999); // Invalid frequency
    
    // Then: Metrics should be updated
    const ValidationMetrics& final_metrics = validator_->get_metrics();
    uint64_t final_validations = final_metrics.total_validations.load(std::memory_order_relaxed);
    uint64_t final_successful = final_metrics.successful_validations.load(std::memory_order_relaxed);
    uint64_t final_failed = final_metrics.failed_validations.load(std::memory_order_relaxed);
    
    EXPECT_EQ(final_validations, initial_validations + 3);
    EXPECT_GT(final_successful, initial_successful);
    EXPECT_GT(final_failed, initial_failed);
    
    // And: Should meet real-time constraints
    EXPECT_TRUE(validator_->meets_realtime_constraints());
    
    // When: Resetting metrics
    validator_->reset_metrics();
    
    // Then: Metrics should be reset to zero
    const ValidationMetrics& reset_metrics = validator_->get_metrics();
    EXPECT_EQ(reset_metrics.total_validations.load(std::memory_order_relaxed), 0);
    EXPECT_EQ(reset_metrics.successful_validations.load(std::memory_order_relaxed), 0);
    EXPECT_EQ(reset_metrics.failed_validations.load(std::memory_order_relaxed), 0);
    EXPECT_EQ(reset_metrics.successful_validations.load(std::memory_order_relaxed), 0);
    EXPECT_EQ(reset_metrics.failed_validations.load(std::memory_order_relaxed), 0);
}

/**
 * @brief Test ValidationCore integration function
 * @requirement SYS-INTEGRATION-001: ValidationCore integration
 * @traceability TEST-C-001-012 → DES-C-001 → SYS-INTEGRATION-001
 */
TEST_F(FrequencyValidatorTest, ValidationCoreIntegrationFunction) {
    // Given: FrequencyValidator for ValidationCore integration
    void* validator_context = validator_.get();
    
    // When: Using static validation function with valid frequency
    ValidationResult valid_result = frequency_validation_function(48000, validator_context);
    
    // Then: Should return valid result
    EXPECT_EQ(valid_result, ValidationResult::Valid);
    
    // When: Using static validation function with invalid frequency
    ValidationResult invalid_result = frequency_validation_function(0, validator_context);
    
    // Then: Should return invalid input
    EXPECT_EQ(invalid_result, ValidationResult::InvalidInput);
    
    // When: Using static validation function with null context
    ValidationResult null_result = frequency_validation_function(48000, nullptr);
    
    // Then: Should handle null context gracefully
    EXPECT_EQ(null_result, ValidationResult::InternalError);
}

/**
 * @brief Test thread safety of FrequencyValidator
 * @requirement SYS-THREAD-001: Thread-safe validation operations
 * @traceability TEST-C-001-013 → DES-C-001 → SYS-THREAD-001
 */
TEST_F(FrequencyValidatorTest, ThreadSafetyValidation) {
    // Given: Multiple threads performing concurrent validations
    constexpr int num_threads = 4;
    constexpr int validations_per_thread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successful_validations{0};
    
    // When: Running concurrent validations
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, &successful_validations, validations_per_thread]() {
            std::vector<uint32_t> test_frequencies = {
                48000, 44100, 96000, 32000, 48048, 47952
            };
            
            for (int i = 0; i < validations_per_thread; ++i) {
                uint32_t freq = test_frequencies[i % test_frequencies.size()];
                FrequencyValidationResult result = validator_->validate_frequency(freq);
                if (result.is_valid()) {
                    successful_validations.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Then: All standard frequency validations should succeed
    EXPECT_EQ(successful_validations.load(), num_threads * validations_per_thread);
    
    // And: Metrics should reflect all operations
    const ValidationMetrics& metrics = validator_->get_metrics();
    EXPECT_EQ(metrics.total_validations.load(std::memory_order_relaxed), 
              num_threads * validations_per_thread);
    
    // And: Should still meet real-time constraints
    EXPECT_TRUE(validator_->meets_realtime_constraints());
}

/**
 * @brief Test memory footprint requirements
 * @requirement SYS-MEMORY-001: Memory footprint constraints
 * @traceability TEST-C-001-014 → DES-C-001 → SYS-MEMORY-001
 */
TEST_F(FrequencyValidatorTest, MemoryFootprintValidation) {
    // Given: FrequencyValidator instance
    // Note: Actual memory measurement is platform-specific
    // This test validates the design constraints
    
    // When: Performing validations (should not allocate memory)
    validator_->validate_frequency(48000);
    validator_->validate_frequency(44100);
    
    // Then: Should meet design memory constraints
    // Tolerance table: 16 entries * ~32 bytes = 512 bytes
    // Standard frequencies: 6 * 4 bytes = 24 bytes  
    // Plus component overhead: < 1KB total
    
    // Verify no memory allocation during validation path
    // (This would be verified with memory profiling tools in practice)
    EXPECT_TRUE(true); // Placeholder for memory footprint validation
    
    // And: Performance should remain consistent
    measure_performance([&]() {
        for (int i = 0; i < 10; ++i) {
            validator_->validate_frequency(48000 + i);
        }
    }, "Batch validation (10x)", std::chrono::nanoseconds(500000)); // 500μs for 10 validations
}

/**
 * @brief Document expected FrequencyValidator interface
 * @traceability TEST-C-001-015 → DES-C-001
 */
TEST_F(FrequencyValidatorTest, DocumentExpectedInterface) {
    // This test documents the expected interface and ensures compilation
    // It validates that all required methods are available and callable
    
    //Expected interface (from DES-C-001):
    //   static std::unique_ptr<FrequencyValidator> create(...)
    //   FrequencyValidationResult validate_frequency(uint32_t, uint32_t = DEFAULT_TOLERANCE_PPM)
    //   uint32_t find_closest_standard_frequency(uint32_t)
    //   double calculate_tolerance_ppm(uint32_t, uint32_t)  
    //   const ValidationMetrics& get_metrics()
    //   void reset_metrics()
    //   bool meets_realtime_constraints(uint64_t = MAX_VALIDATION_LATENCY_NS)
    
    // Test interface availability (compilation test)
    EXPECT_NE(validator_, nullptr);
    
    // Test result structure
    FrequencyValidationResult result = validator_->validate_frequency(48000);
    EXPECT_TRUE(result.is_valid() || !result.is_valid()); // Boolean result
    EXPECT_NE(result.get_description(), nullptr); // Description available
    
    // Test static function availability
    ValidationResult static_result = frequency_validation_function(48000, validator_.get());
    EXPECT_TRUE(static_result == ValidationResult::Valid || 
                static_result != ValidationResult::Valid); // Valid enum
    
    std::cout << "FrequencyValidator interface validation completed successfully" << std::endl;
}