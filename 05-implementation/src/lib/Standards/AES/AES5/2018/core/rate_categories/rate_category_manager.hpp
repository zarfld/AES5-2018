/**
 * @file rate_category_manager.hpp
 * @brief AES5-2018 Rate Category Manager Interface
 * @traceability DES-C-003
 * 
 * Provides rate category classification according to AES5-2018 Section 5.3.
 * Supports Basic, Double, Quadruple, Octuple, Half, and Quarter rate categories
 * with ValidationCore integration for performance monitoring.
 */

#ifndef AES_AES5_2018_CORE_RATE_CATEGORIES_RATE_CATEGORY_MANAGER_H
#define AES_AES5_2018_CORE_RATE_CATEGORIES_RATE_CATEGORY_MANAGER_H

#include <memory>
#include <cstdint>
#include "../validation/validation_core.hpp"

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace rate_categories {

/**
 * @brief AES5-2018 Rate Categories (Section 5.3)
 * @traceability DES-C-003 → AES5-2018 Section 5.3
 */
enum class RateCategory : uint8_t {
    Unknown = 0,      ///< Unknown or invalid rate category
    Quarter = 1,      ///< Quarter rate: 7.75-13.5 kHz
    Half = 2,         ///< Half rate: 15.5-27 kHz
    Basic = 3,        ///< Basic rate: 31-54 kHz (includes 32k, 44.1k, 48k)
    Double = 4,       ///< Double rate: 62-108 kHz (includes 88.2k, 96k)
    Quadruple = 5,    ///< Quadruple rate: 124-216 kHz (includes 176.4k, 192k)
    Octuple = 6       ///< Octuple rate: 248-432 kHz (includes 352.8k, 384k)
};

/**
 * @brief Rate category classification result
 * @traceability DES-C-003 → Rate Classification Result
 */
struct RateCategoryResult {
    RateCategory category;      ///< Classified rate category
    double multiplier;          ///< Rate multiplier relative to 48 kHz base
    uint32_t frequency_hz;      ///< Input frequency in Hz
    bool valid;                 ///< True if frequency fits a valid category
    
    /**
     * @brief Check if classification result is valid
     * @return true if frequency was successfully classified
     */
    bool is_valid() const noexcept {
        return valid && category != RateCategory::Unknown;
    }
    
    /**
     * @brief Get human-readable category name
     * @return Category name string
     */
    const char* get_category_name() const noexcept;
    
    /**
     * @brief Get AES5-2018 section reference for this category
     * @return Section reference string
     */
    const char* get_aes5_section() const noexcept;
};

/**
 * @brief AES5-2018 Rate Category Manager
 * @traceability DES-C-003
 * 
 * Classifies sampling frequencies into AES5-2018 rate categories with
 * high-performance lookup tables and ValidationCore integration.
 * 
 * Thread Safety: All methods are thread-safe and lock-free
 * Exception Safety: All methods provide noexcept guarantee
 * Performance: <10μs per classification, optimized lookup tables
 * Memory: <2KB footprint, static allocation only
 */
class RateCategoryManager {
public:
    /**
     * @brief Create rate category manager with ValidationCore
     * @param validation_core ValidationCore for performance monitoring
     * @return Unique pointer to RateCategoryManager or nullptr on failure
     * @traceability DES-C-003 → create
     */
    static std::unique_ptr<RateCategoryManager> create(
        std::unique_ptr<validation::ValidationCore> validation_core) noexcept;

    /**
     * @brief Destructor
     */
    ~RateCategoryManager() noexcept = default;

    // Delete copy/move operations for unique ownership
    RateCategoryManager(const RateCategoryManager&) = delete;
    RateCategoryManager& operator=(const RateCategoryManager&) = delete;
    RateCategoryManager(RateCategoryManager&&) = delete;
    RateCategoryManager& operator=(RateCategoryManager&&) = delete;

    /**
     * @brief Classify frequency into AES5-2018 rate category
     * @param frequency_hz Sampling frequency in Hz
     * @return Rate category classification result
     * @performance <10μs per call (typically <1μs with lookup tables)
     * @thread_safety Thread-safe, lock-free operation
     * @traceability DES-C-003 → classify_rate_category
     * 
     * Classifies the input frequency according to AES5-2018 Section 5.3:
     * - Quarter: 7.75-13.5 kHz
     * - Half: 15.5-27 kHz  
     * - Basic: 31-54 kHz (32k, 44.1k, 48k)
     * - Double: 62-108 kHz (88.2k, 96k)
     * - Quadruple: 124-216 kHz (176.4k, 192k)
     * - Octuple: 248-432 kHz (352.8k, 384k)
     */
    RateCategoryResult classify_rate_category(uint32_t frequency_hz) const noexcept;

    /**
     * @brief Get performance metrics from ValidationCore
     * @return Reference to validation metrics
     * @thread_safety Thread-safe atomic access
     * @traceability DES-C-003 → get_metrics
     */
    const validation::ValidationMetrics& get_metrics() const noexcept;

    /**
     * @brief Reset performance metrics
     * @thread_safety Thread-safe atomic reset
     * @traceability DES-C-003 → reset_metrics
     */
    void reset_metrics() noexcept;

    /**
     * @brief Check if rate category manager meets real-time constraints
     * @param max_latency_ns Maximum acceptable latency in nanoseconds
     * @return true if average latency is within constraints
     * @traceability DES-C-003 → meets_realtime_constraints
     */
    bool meets_realtime_constraints(uint64_t max_latency_ns = 10000) const noexcept;

    /**
     * @brief Get memory footprint of rate category manager
     * @return Memory usage in bytes
     * @traceability DES-C-003 → get_memory_footprint
     */
    size_t get_memory_footprint() const noexcept;

    /**
     * @brief Get rate category for frequency (convenience method)
     * @param frequency_hz Sampling frequency in Hz
     * @return Rate category enum value
     * @performance <10μs per call
     * @thread_safety Thread-safe
     */
    RateCategory get_rate_category(uint32_t frequency_hz) const noexcept;

    /**
     * @brief Calculate rate multiplier relative to 48 kHz base
     * @param frequency_hz Sampling frequency in Hz
     * @return Rate multiplier (e.g., 2.0 for 96 kHz)
     * @performance <5μs per call
     * @thread_safety Thread-safe
     */
    double calculate_rate_multiplier(uint32_t frequency_hz) const noexcept;

    /**
     * @brief Check if frequency is in valid rate category
     * @param frequency_hz Sampling frequency in Hz
     * @return true if frequency fits any AES5-2018 rate category
     * @performance <5μs per call
     * @thread_safety Thread-safe
     */
    bool is_valid_rate_category(uint32_t frequency_hz) const noexcept;

    /**
     * @brief Get AES5-2018 rate category constants
     * @traceability DES-C-003 → AES5-2018 Constants
     */
    static constexpr uint32_t QUARTER_RATE_MIN_HZ = 7750;   ///< 7.75 kHz minimum
    static constexpr uint32_t QUARTER_RATE_MAX_HZ = 13500;  ///< 13.5 kHz maximum
    static constexpr uint32_t HALF_RATE_MIN_HZ = 15500;     ///< 15.5 kHz minimum
    static constexpr uint32_t HALF_RATE_MAX_HZ = 27000;     ///< 27 kHz maximum
    static constexpr uint32_t BASIC_RATE_MIN_HZ = 31000;    ///< 31 kHz minimum
    static constexpr uint32_t BASIC_RATE_MAX_HZ = 54000;    ///< 54 kHz maximum
    static constexpr uint32_t DOUBLE_RATE_MIN_HZ = 62000;   ///< 62 kHz minimum
    static constexpr uint32_t DOUBLE_RATE_MAX_HZ = 108000;  ///< 108 kHz maximum
    static constexpr uint32_t QUADRUPLE_RATE_MIN_HZ = 124000; ///< 124 kHz minimum
    static constexpr uint32_t QUADRUPLE_RATE_MAX_HZ = 216000; ///< 216 kHz maximum
    static constexpr uint32_t OCTUPLE_RATE_MIN_HZ = 248000;   ///< 248 kHz minimum
    static constexpr uint32_t OCTUPLE_RATE_MAX_HZ = 432000;   ///< 432 kHz maximum

    static constexpr uint32_t BASE_FREQUENCY_HZ = 48000;    ///< 48 kHz base frequency
    static constexpr double DEFAULT_TOLERANCE_PERCENT = 5.0; ///< Default tolerance

private:
    /**
     * @brief Private constructor - use create() factory method
     * @param validation_core ValidationCore for performance monitoring
     */
    explicit RateCategoryManager(
        std::unique_ptr<validation::ValidationCore> validation_core) noexcept;

    // ValidationCore for performance monitoring
    std::unique_ptr<validation::ValidationCore> validation_core_;

    // Static rate category validation function for ValidationCore
    static validation::ValidationResult rate_category_validation_function(
        uint32_t frequency, void* context) noexcept;
    
    // Internal implementation methods
    RateCategory classify_rate_category_internal(uint32_t frequency_hz) const noexcept;
    double calculate_multiplier_internal(uint32_t frequency_hz) const noexcept;
    
    // Performance optimization members
    mutable uint32_t last_frequency_;      ///< Cache last frequency for optimization
    mutable RateCategory last_category_;   ///< Cache last category for optimization
    mutable double last_multiplier_;       ///< Cache last multiplier for optimization
};

/**
 * @brief Rate category utility functions
 * @traceability DES-C-003 → Utility Functions
 */
namespace rate_category_utils {

/**
 * @brief Convert rate category to string
 * @param category Rate category enum
 * @return Human-readable category name
 */
const char* to_string(RateCategory category) noexcept;

/**
 * @brief Get AES5-2018 section reference for category
 * @param category Rate category enum
 * @return AES5-2018 section reference
 */
const char* get_aes5_section(RateCategory category) noexcept;

/**
 * @brief Get frequency range for category
 * @param category Rate category enum
 * @param min_hz Output minimum frequency in Hz
 * @param max_hz Output maximum frequency in Hz
 * @return true if category has defined range
 */
bool get_frequency_range(RateCategory category, 
                        uint32_t& min_hz, uint32_t& max_hz) noexcept;

} // namespace rate_category_utils

} // namespace rate_categories
} // namespace core
} // namespace _2018
} // namespace AES5
} // namespace AES

#endif // AES_AES5_2018_CORE_RATE_CATEGORIES_RATE_CATEGORY_MANAGER_H