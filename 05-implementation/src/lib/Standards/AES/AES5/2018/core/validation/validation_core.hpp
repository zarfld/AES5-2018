/**
 * @file validation_core.hpp
 * @brief AES5-2018 Real-Time Validation Core Infrastructure
 * @author AI Agent
 * @date 2025-11-06
 * @version 0.1.0
 * 
 * @traceability DES-C-005 → SYS-PERF-001, SYS-REAL-TIME-001
 * 
 * Implements high-performance validation infrastructure with real-time constraints.
 * Provides lock-free validation operations, performance metrics, and error handling
 * suitable for audio processing applications with <5ms latency requirements.
 * 
 * Performance Requirements:
 * - <5ms total validation latency
 * - Lock-free operation for real-time threads  
 * - <100μs per validation call
 * - Thread-safe metric collection
 * - Zero memory allocation in validation path
 */

#ifndef AES_AES5_2018_CORE_VALIDATION_VALIDATION_CORE_HPP
#define AES_AES5_2018_CORE_VALIDATION_VALIDATION_CORE_HPP

#include <cstdint>
#include <atomic>
#include <chrono>
#include <array>

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace validation {

/**
 * @brief Validation result enumeration
 * @traceability DES-C-005 → ValidationResult
 */
enum class ValidationResult : uint8_t {
    Valid = 0,              ///< Validation passed
    InvalidInput = 1,       ///< Input parameters invalid
    OutOfTolerance = 2,     ///< Value outside acceptable tolerance
    PerformanceViolation = 3, ///< Performance constraint violated
    InternalError = 4       ///< Internal validation error
};

/**
 * @brief Performance metrics for validation operations
 * @traceability DES-C-005 → Performance Metrics
 * 
 * Note: Non-copyable due to atomic members for thread safety
 */
struct ValidationMetrics {
    std::atomic<uint64_t> total_validations{0};      ///< Total validation calls
    std::atomic<uint64_t> successful_validations{0}; ///< Successful validations
    std::atomic<uint64_t> failed_validations{0};     ///< Failed validations
    std::atomic<uint64_t> max_latency_ns{0};         ///< Maximum latency in nanoseconds
    std::atomic<uint64_t> total_latency_ns{0};       ///< Total cumulative latency
    
    // Make non-copyable due to atomic members
    ValidationMetrics() = default;
    ValidationMetrics(const ValidationMetrics&) = delete;
    ValidationMetrics& operator=(const ValidationMetrics&) = delete;
    ValidationMetrics(ValidationMetrics&&) = delete;
    ValidationMetrics& operator=(ValidationMetrics&&) = delete;
    
    /**
     * @brief Get average validation latency in nanoseconds
     * @return Average latency, or 0 if no validations performed
     */
    uint64_t get_average_latency_ns() const noexcept {
        uint64_t total = total_validations.load(std::memory_order_relaxed);
        if (total == 0) return 0;
        return total_latency_ns.load(std::memory_order_relaxed) / total;
    }
    
    /**
     * @brief Get validation success rate as percentage
     * @return Success rate (0.0 to 100.0), or 0.0 if no validations
     */
    double get_success_rate() const noexcept {
        uint64_t total = total_validations.load(std::memory_order_relaxed);
        if (total == 0) return 0.0;
        uint64_t successful = successful_validations.load(std::memory_order_relaxed);
        return (double)successful / total * 100.0;
    }
};

/**
 * @brief Real-Time Validation Core Infrastructure
 * @traceability DES-C-005
 * 
 * Provides lock-free, high-performance validation infrastructure optimized for 
 * real-time audio processing. Supports sub-millisecond validation operations
 * with comprehensive performance monitoring.
 * 
 * Thread Safety: All methods are thread-safe and lock-free
 * Exception Safety: All methods provide noexcept guarantee
 * Performance: <100μs per validation call, <5ms total system latency
 * Memory: Zero allocation in validation path, static allocation only
 */
class ValidationCore {
public:
    /**
     * @brief Validation function pointer type
     * @param value Value to validate
     * @param context Optional validation context
     * @return ValidationResult indicating success or failure type
     */
    using ValidationFunction = ValidationResult(*)(uint32_t value, void* context) noexcept;

    /**
     * @brief Default constructor - initializes validation infrastructure
     * @traceability DES-C-005 → Constructor
     */
    ValidationCore() noexcept;

    /**
     * @brief Copy constructor - copies configuration but resets metrics
     * @param other Source ValidationCore to copy from
     */
    ValidationCore(const ValidationCore& other) noexcept;

    /**
     * @brief Copy assignment operator
     * @param other Source ValidationCore to assign from
     * @return Reference to this ValidationCore
     */
    ValidationCore& operator=(const ValidationCore& other) noexcept;

    /**
     * @brief Move constructor
     * @param other Source ValidationCore to move from
     */
    ValidationCore(ValidationCore&& other) noexcept;

    /**
     * @brief Move assignment operator
     * @param other Source ValidationCore to move from
     * @return Reference to this ValidationCore
     */
    ValidationCore& operator=(ValidationCore&& other) noexcept;

    /**
     * @brief Destructor
     */
    ~ValidationCore() noexcept = default;

    /**
     * @brief Perform real-time validation with performance monitoring
     * @param value Value to validate (e.g., sampling frequency)
     * @param validation_function Function to perform actual validation
     * @param context Optional context passed to validation function
     * @return ValidationResult indicating success or failure type
     * 
     * @traceability DES-C-005 → validate
     * 
     * @exception none (noexcept guarantee for real-time operation)
     * @performance <100μs per call including metrics collection
     * @thread_safety Thread-safe, lock-free implementation
     * 
     * @pre validation_function != nullptr
     * @post Performance metrics updated atomically
     * @post Return value accurately reflects validation status
     * 
     * Example usage:
     * @code
     * ValidationCore core;
     * auto result = core.validate(48000, frequency_validator, nullptr);
     * if (result == ValidationResult::Valid) {
     *     // Proceed with validated frequency
     * }
     * @endcode
     */
    ValidationResult validate(uint32_t value, 
                            ValidationFunction validation_function,
                            void* context = nullptr) noexcept;

    /**
     * @brief Batch validate multiple values efficiently  
     * @param values Array of values to validate
     * @param count Number of values in array
     * @param validation_function Function to perform validation
     * @param context Optional context passed to validation function
     * @return ValidationResult::Valid if all pass, first failure otherwise
     * 
     * @traceability DES-C-005 → batch_validate
     * 
     * @exception none (noexcept guarantee)
     * @performance <500μs for up to 16 values
     * @thread_safety Thread-safe
     * 
     * @pre values != nullptr && count > 0 && count <= MAX_BATCH_SIZE
     * @pre validation_function != nullptr
     */
    ValidationResult batch_validate(const uint32_t* values,
                                  size_t count,
                                  ValidationFunction validation_function,
                                  void* context = nullptr) noexcept;

    /**
     * @brief Get current performance metrics
     * @return Copy of current validation metrics
     * 
     * @traceability DES-C-005 → get_metrics
     * 
     * @exception none (noexcept guarantee)
     * @performance <10μs
     * @thread_safety Thread-safe
     */
    const ValidationMetrics& get_metrics() const noexcept;

    /**
     * @brief Reset performance metrics to zero
     * @traceability DES-C-005 → reset_metrics
     * 
     * @exception none (noexcept guarantee)
     * @performance <5μs
     * @thread_safety Thread-safe
     */
    void reset_metrics() noexcept;

    /**
     * @brief Check if validation latency meets real-time constraints
     * @param max_latency_ns Maximum acceptable latency in nanoseconds
     * @return true if all validations meet constraint, false otherwise
     * 
     * @traceability DES-C-005 → meets_realtime_constraints
     * 
     * @exception none (noexcept guarantee)
     * @performance <1μs
     * @thread_safety Thread-safe
     */
    bool meets_realtime_constraints(uint64_t max_latency_ns = 100000) const noexcept; // 100μs default

    /**
     * @brief Get memory footprint of ValidationCore instance
     * @return Size in bytes of this instance
     * 
     * @traceability DES-C-005 → get_memory_footprint
     */
    static constexpr size_t get_memory_footprint() noexcept {
        return sizeof(ValidationCore);
    }

private:
    /// Maximum number of values in batch validation
    static constexpr size_t MAX_BATCH_SIZE = 16;

    /// Performance metrics (atomic for thread safety)
    mutable ValidationMetrics metrics_;

    /**
     * @brief Update metrics after validation operation
     * @param result Validation result
     * @param latency_ns Operation latency in nanoseconds
     */
    void update_metrics(ValidationResult result, uint64_t latency_ns) noexcept;

    /**
     * @brief Get high-resolution timestamp in nanoseconds
     * @return Current time in nanoseconds since epoch
     */
    static uint64_t get_timestamp_ns() noexcept;
};

} // namespace validation
} // namespace core
} // namespace _2018
} // namespace AES5
} // namespace AES

#endif // AES_AES5_2018_CORE_VALIDATION_VALIDATION_CORE_HPP