/**
 * @file frequency_validator.hpp
 * @brief AES5-2018 Sampling Frequency Validator - Standards Compliant Implementation
 * @traceability DES-C-001
 * 
 * This module implements comprehensive AES5-2018 sampling frequency validation
 * with high-precision tolerance tables and real-time performance optimization.
 * 
 * Key Features:
 * - Primary sampling frequency validation (48 kHz AES5-2018 Section 5.1)
 * - Other sampling frequencies (44.1 kHz, 96 kHz AES5-2018 Section 5.2)
 * - Legacy sampling frequency support (32 kHz AES5-2018 Section 5.4)
 * - Pull-up/pull-down variants (1001/1000 factor AES5-2018 Annex A)
 * - Tolerance-based validation with configurable precision
 * - Integration with ComplianceEngine for standards compliance
 * - Integration with ValidationCore for performance monitoring
 * 
 * Performance Requirements:
 * - <50μs per validation call (including tolerance calculation)
 * - <1KB memory footprint for tolerance tables
 * - Thread-safe operations with lock-free implementation
 * - Zero memory allocation in validation path
 * 
 * Thread Safety: All methods are thread-safe and reentrant
 * Exception Safety: All methods provide noexcept guarantee
 * 
 * @version 1.0.0
 * @date 2024-12-28
 * @author AES5-2018 TDD Implementation
 * 
 * @copyright
 * This implementation is based on understanding of AES5-2018 requirements.
 * No copyrighted content from the standard is reproduced.
 * Refer to AES5-2018 specification for authoritative requirements.
 */

#ifndef AES_AES5_2018_CORE_FREQUENCY_VALIDATION_FREQUENCY_VALIDATOR_HPP
#define AES_AES5_2018_CORE_FREQUENCY_VALIDATION_FREQUENCY_VALIDATOR_HPP

#include <cstdint>
#include <array>
#include <memory>

// AES5-2018 Dependencies
#include "../compliance/compliance_engine.hpp"     // ComplianceEngine for standards compliance
#include "../validation/validation_core.hpp"      // ValidationCore for performance monitoring

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace frequency_validation {

/**
 * @brief Frequency validation result with detailed information
 * @traceability DES-C-001 → FrequencyValidationResult
 */
struct FrequencyValidationResult {
    validation::ValidationResult status;      ///< Overall validation status
    uint32_t detected_frequency;            ///< Detected/normalized frequency (Hz)
    uint32_t closest_standard_frequency;    ///< Nearest AES5-2018 standard frequency
    double tolerance_ppm;                   ///< Tolerance in parts per million
    compliance::AES5Clause applicable_clause; ///< AES5-2018 clause that applies
    
    /**
     * @brief Check if validation was successful
     * @return true if frequency is valid according to AES5-2018
     */
    bool is_valid() const noexcept {
        return status == validation::ValidationResult::Valid;
    }
    
    /**
     * @brief Get human-readable description of validation result
     * @return String describing the validation outcome
     */
    const char* get_description() const noexcept;
};

/**
 * @brief Frequency tolerance configuration
 * @traceability DES-C-001 → FrequencyTolerance
 */
struct FrequencyTolerance {
    uint32_t nominal_frequency;     ///< Nominal frequency (Hz)
    uint32_t tolerance_ppm;         ///< Tolerance in parts per million
    uint32_t min_frequency;         ///< Minimum acceptable frequency (Hz)
    uint32_t max_frequency;         ///< Maximum acceptable frequency (Hz)
    
    /**
     * @brief Check if frequency falls within tolerance
     * @param frequency Frequency to check (Hz)
     * @return true if within tolerance
     */
    bool contains(uint32_t frequency) const noexcept {
        return frequency >= min_frequency && frequency <= max_frequency;
    }
};

/**
 * @brief AES5-2018 Sampling Frequency Validator
 * @traceability DES-C-001
 * 
 * High-performance sampling frequency validator implementing AES5-2018
 * recommendations with tolerance tables and comprehensive validation.
 * 
 * This validator provides:
 * - Standards-compliant frequency validation
 * - High-precision tolerance calculations
 * - Performance monitoring and metrics
 * - Thread-safe lock-free operations
 * - Zero-allocation validation path
 * 
 * Integration Components:
 * - ComplianceEngine: Provides AES5-2018 clause validation
 * - ValidationCore: Provides performance monitoring and metrics
 * 
 * Usage Example:
 * @code
 * auto validator = FrequencyValidator::create();
 * FrequencyValidationResult result = validator->validate_frequency(48000);
 * if (result.is_valid()) {
 *     // Frequency is AES5-2018 compliant
 *     process_audio_at_frequency(result.detected_frequency);
 * }
 * @endcode
 */
class FrequencyValidator {
public:
    // AES5-2018 Standard Frequencies (from specification)
    static constexpr uint32_t PRIMARY_FREQUENCY = 48000;      ///< AES5-2018 Section 5.1
    static constexpr uint32_t CONSUMER_FREQUENCY = 44100;     ///< AES5-2018 Section 5.2
    static constexpr uint32_t HIGH_BANDWIDTH_FREQUENCY = 96000; ///< AES5-2018 Section 5.2
    static constexpr uint32_t LEGACY_FREQUENCY = 32000;       ///< AES5-2018 Section 5.4
    
    // Pull-up/Pull-down variants (AES5-2018 Annex A)
    static constexpr uint32_t PULLUP_48K = 48048;            ///< 48000 * 1001/1000
    static constexpr uint32_t PULLDOWN_48K = 47952;          ///< 48000 * 1000/1001
    
    // Default tolerances (conservative values for high precision)
    static constexpr uint32_t DEFAULT_TOLERANCE_PPM = 100;    ///< ±100 ppm default
    static constexpr uint32_t TIGHT_TOLERANCE_PPM = 50;       ///< ±50 ppm tight tolerance
    
    // Performance constants
    static constexpr size_t MAX_TOLERANCE_ENTRIES = 16;       ///< Maximum tolerance table entries
    static constexpr uint64_t MAX_VALIDATION_LATENCY_NS = 50000; ///< 50μs max validation time

    /**
     * @brief Factory method to create FrequencyValidator instance
     * @param compliance_engine ComplianceEngine instance for standards validation
     * @param validation_core ValidationCore instance for performance monitoring
     * @return Unique pointer to configured FrequencyValidator
     * 
     * @traceability DES-C-001 → create
     * 
     * @pre compliance_engine != nullptr
     * @pre validation_core != nullptr
     * @post Returned validator is ready for frequency validation
     * @post Tolerance tables are initialized with AES5-2018 values
     * 
     * Example:
     * @code
     * auto compliance_engine = std::make_unique<ComplianceEngine>();
     * auto validation_core = std::make_unique<ValidationCore>();
     * auto validator = FrequencyValidator::create(
     *     std::move(compliance_engine), 
     *     std::move(validation_core));
     * @endcode
     */
    static std::unique_ptr<FrequencyValidator> create(
        std::unique_ptr<compliance::ComplianceEngine> compliance_engine,
        std::unique_ptr<validation::ValidationCore> validation_core) noexcept;

    /**
     * @brief Default constructor (private - use factory method)
     */
    FrequencyValidator() = delete;
    
    /**
     * @brief Copy constructor (deleted - use factory method)
     */
    FrequencyValidator(const FrequencyValidator&) = delete;
    
    /**
     * @brief Assignment operator (deleted)
     */
    FrequencyValidator& operator=(const FrequencyValidator&) = delete;
    
    /**
     * @brief Destructor
     */
    ~FrequencyValidator() noexcept = default;

    /**
     * @brief Validate sampling frequency against AES5-2018 recommendations
     * @param frequency Sampling frequency to validate (Hz)
     * @param tolerance_ppm Optional custom tolerance in parts per million
     * @return FrequencyValidationResult with detailed validation information
     * 
     * @traceability DES-C-001 → validate_frequency
     * 
     * @exception none (noexcept guarantee for real-time operation)
     * @performance <50μs per validation call including tolerance calculation
     * @thread_safety Thread-safe, lock-free implementation
     * 
     * @pre frequency > 0 (will return InvalidInput for frequency == 0)
     * @post Performance metrics updated in ValidationCore
     * @post Compliance status validated through ComplianceEngine
     * 
     * This method validates frequencies according to:
     * - AES5-2018 Section 5.1: Primary sampling frequency (48 kHz)
     * - AES5-2018 Section 5.2: Other sampling frequencies (44.1 kHz, 96 kHz)
     * - AES5-2018 Section 5.4: Legacy sampling frequencies (32 kHz)
     * - AES5-2018 Annex A: Pull-up/pull-down variants
     * 
     * Example usage:
     * @code
     * // Validate primary frequency
     * auto result = validator->validate_frequency(48000);
     * assert(result.is_valid());
     * assert(result.applicable_clause == AES5Clause::Section_5_1);
     * 
     * // Validate with custom tolerance
     * auto tight_result = validator->validate_frequency(48050, 50); // ±50 ppm
     * @endcode
     */
    FrequencyValidationResult validate_frequency(uint32_t frequency, 
                                               uint32_t tolerance_ppm = DEFAULT_TOLERANCE_PPM) const noexcept;

    /**
     * @brief Find closest AES5-2018 standard frequency
     * @param frequency Input frequency (Hz)
     * @return Closest standard frequency according to AES5-2018
     * 
     * @traceability DES-C-001 → find_closest_standard_frequency
     * 
     * @exception none (noexcept guarantee)
     * @performance <10μs per lookup (using pre-computed tables)
     * @thread_safety Thread-safe, read-only operation
     * 
     * @pre frequency > 0
     * @post Returns valid AES5-2018 frequency
     * 
     * This method finds the nearest frequency from:
     * - 32000 Hz (legacy)
     * - 44100 Hz (consumer)
     * - 47952 Hz (pull-down 48k)
     * - 48000 Hz (primary)
     * - 48048 Hz (pull-up 48k)  
     * - 96000 Hz (high bandwidth)
     */
    uint32_t find_closest_standard_frequency(uint32_t frequency) const noexcept;

    /**
     * @brief Calculate tolerance in parts per million between frequencies
     * @param measured_frequency Measured frequency (Hz)
     * @param reference_frequency Reference frequency (Hz)
     * @return Tolerance in parts per million (absolute value)
     * 
     * @traceability DES-C-001 → calculate_tolerance_ppm
     * 
     * @exception none (noexcept guarantee)
     * @performance <5μs per calculation
     * @thread_safety Thread-safe, stateless calculation
     * 
     * @pre measured_frequency > 0 && reference_frequency > 0
     * @post Returns non-negative tolerance value
     * 
     * Formula: tolerance_ppm = |measured - reference| / reference * 1,000,000
     * 
     * Example:
     * @code
     * double tolerance = validator->calculate_tolerance_ppm(48050, 48000);
     * assert(tolerance == 1041.67); // ~1042 ppm
     * @endcode
     */
    double calculate_tolerance_ppm(uint32_t measured_frequency, 
                                  uint32_t reference_frequency) const noexcept;

    /**
     * @brief Get performance and operational metrics
     * @return Reference to current validation metrics
     * 
     * @traceability DES-C-001 → get_metrics
     * 
     * @exception none (noexcept guarantee)
     * @performance <5μs (atomic read operations)
     * @thread_safety Thread-safe
     * 
     * Provides access to ValidationCore metrics including:
     * - Total validations performed
     * - Success/failure rates
     * - Performance latency statistics
     * - Memory usage information
     */
    const validation::ValidationMetrics& get_metrics() const noexcept;

    /**
     * @brief Reset performance metrics to zero
     * @traceability DES-C-001 → reset_metrics
     * 
     * @exception none (noexcept guarantee)
     * @performance <10μs
     * @thread_safety Thread-safe
     * 
     * Resets all performance counters and statistics.
     * Useful for benchmarking and performance analysis.
     */
    void reset_metrics() noexcept;

    /**
     * @brief Check if validator meets real-time performance constraints
     * @param max_latency_ns Maximum acceptable latency in nanoseconds
     * @return true if all validations meet the latency constraint
     * 
     * @traceability DES-C-001 → meets_realtime_constraints
     * 
     * @exception none (noexcept guarantee)
     * @performance <5μs
     * @thread_safety Thread-safe
     */
    bool meets_realtime_constraints(uint64_t max_latency_ns = MAX_VALIDATION_LATENCY_NS) const noexcept;

    /**
     * @brief Internal validation with performance monitoring (used by static function)
     * @param frequency Frequency to validate
     * @param tolerance_ppm Tolerance in parts per million
     * @return Validation result
     * 
     * @traceability DES-C-001 → validate_frequency_internal
     * 
     * Note: Made public for static validation function access
     */
    FrequencyValidationResult validate_frequency_internal(uint32_t frequency, 
                                                         uint32_t tolerance_ppm) const noexcept;

private:
    /**
     * @brief Private constructor for factory method
     * @param compliance_engine ComplianceEngine for standards validation
     * @param validation_core ValidationCore for performance monitoring
     */
    FrequencyValidator(std::unique_ptr<compliance::ComplianceEngine> compliance_engine,
                      std::unique_ptr<validation::ValidationCore> validation_core) noexcept;

    /**
     * @brief Initialize tolerance tables with AES5-2018 values
     * @traceability DES-C-001 → initialize_tolerance_tables
     */
    void initialize_tolerance_tables() noexcept;
    
    // Friend function for ValidationCore integration
    friend validation::ValidationResult frequency_validation_function(uint32_t frequency, void* context) noexcept;

    // Component Dependencies (injected via constructor)
    std::unique_ptr<compliance::ComplianceEngine> compliance_engine_;  ///< AES5-2018 compliance validation
    std::unique_ptr<validation::ValidationCore> validation_core_;      ///< Performance monitoring

    // Pre-computed tolerance tables for O(1) lookup performance
    std::array<FrequencyTolerance, MAX_TOLERANCE_ENTRIES> tolerance_table_; ///< Standard frequency tolerances
    size_t tolerance_table_size_;                                           ///< Active entries in tolerance table

    // Performance optimization data
    mutable std::array<uint32_t, 10> standard_frequencies_;                 ///< Sorted standard frequencies for binary search
    mutable uint32_t current_tolerance_ppm_;                                 ///< Current tolerance for static function access
};

/**
 * @brief Static validation function for ValidationCore integration
 * @param frequency Frequency value to validate (Hz)
 * @param context FrequencyValidator instance pointer
 * @return ValidationResult for ValidationCore integration
 * 
 * @traceability DES-C-001 → static_validation_function
 * 
 * This function provides a static interface for ValidationCore integration.
 * It allows FrequencyValidator to be used with ValidationCore's
 * performance monitoring and batch validation capabilities.
 * 
 * Usage with ValidationCore:
 * @code
 * ValidationCore core;
 * FrequencyValidator validator = FrequencyValidator::create(...);
 * ValidationResult result = core.validate(48000, frequency_validation_function, &validator);
 * @endcode
 */
validation::ValidationResult frequency_validation_function(uint32_t frequency, void* context) noexcept;

} // namespace frequency_validation
} // namespace core
} // namespace _2018
} // namespace AES5
} // namespace AES

#endif // AES_AES5_2018_CORE_FREQUENCY_VALIDATION_FREQUENCY_VALIDATOR_HPP