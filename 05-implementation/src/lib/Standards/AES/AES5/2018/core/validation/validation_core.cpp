/**
 * @file validation_core.cpp
 * @brief AES5-2018 Real-Time Validation Core Infrastructure Implementation
 * @author AI Agent
 * @date 2025-11-06
 * @version 0.1.0
 * 
 * @traceability DES-C-005 → Implementation
 * 
 * GREEN PHASE: Minimal implementation to pass failing tests
 * This implementation provides the minimal functionality required to pass
 * the TDD tests. Will be refactored in subsequent iterations.
 */

#include "validation_core.hpp"
#include <algorithm>

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace validation {

ValidationCore::ValidationCore() noexcept {
    // GREEN PHASE: Minimal constructor
    // Metrics are automatically initialized to zero via atomic defaults
}

ValidationCore::ValidationCore(const ValidationCore& other) noexcept {
    // GREEN PHASE: Copy constructor - copy configuration, reset metrics
    (void)other;  // Configuration copying not needed in minimal implementation
    // metrics_ is automatically initialized to zero
}

ValidationCore& ValidationCore::operator=(const ValidationCore& other) noexcept {
    // GREEN PHASE: Copy assignment
    if (this != &other) {
        (void)other;  // Configuration copying not needed in minimal implementation
        reset_metrics();
    }
    return *this;
}

ValidationCore::ValidationCore(ValidationCore&& other) noexcept {
    // GREEN PHASE: Move constructor
    (void)other;  // No resources to move in minimal implementation
    // metrics_ is automatically initialized to zero
}

ValidationCore& ValidationCore::operator=(ValidationCore&& other) noexcept {
    // GREEN PHASE: Move assignment
    if (this != &other) {
        (void)other;  // No resources to move in minimal implementation
        reset_metrics();
    }
    return *this;
}

ValidationResult ValidationCore::validate(uint32_t value, 
                                        ValidationFunction validation_function,
                                        void* context) noexcept {
    // REFACTOR PHASE: Optimized validation with minimal overhead
    
    // Handle null function pointer (fast path)
    if (validation_function == nullptr) {
        update_metrics(ValidationResult::InternalError, 0);
        return ValidationResult::InternalError;
    }
    
    // High-performance timing measurement (cache-optimized)
    uint64_t start_time = get_timestamp_ns();
    
    // Perform actual validation (main operation, optimize for inlining)
    ValidationResult result = validation_function(value, context);
    
    // Fast latency calculation (single call, single subtraction)
    uint64_t latency_ns = get_timestamp_ns() - start_time;
    
    // Update metrics with optimized atomic operations
    update_metrics(result, latency_ns);
    
    return result;
}

ValidationResult ValidationCore::batch_validate(const uint32_t* values,
                                              size_t count,
                                              ValidationFunction validation_function,
                                              void* context) noexcept {
    // REFACTOR PHASE: Cache-optimized batch validation
    
    // Handle invalid parameters (fast path)
    if (values == nullptr || count == 0 || validation_function == nullptr) {
        update_metrics(ValidationResult::InternalError, 0);
        return ValidationResult::InternalError;
    }
    
    // Clamp batch size (avoid bounds check in hot loop)
    const size_t batch_count = (count > MAX_BATCH_SIZE) ? MAX_BATCH_SIZE : count;
    
    // Single timing measurement for entire batch (reduces syscall overhead)
    uint64_t start_time = get_timestamp_ns();
    
    // Cache-friendly sequential validation with early exit
    ValidationResult overall_result = ValidationResult::Valid;
    for (size_t i = 0; i < batch_count; ++i) {
        ValidationResult result = validation_function(values[i], context);
        if (result != ValidationResult::Valid) {
            overall_result = result;
            break;  // Early exit optimization for performance
        }
    }
    
    // Fast latency calculation (single subtraction)
    uint64_t latency_ns = get_timestamp_ns() - start_time;
    
    // Update optimized metrics
    update_metrics(overall_result, latency_ns);
    
    return overall_result;
}

const ValidationMetrics& ValidationCore::get_metrics() const noexcept {
    // GREEN PHASE: Return reference to internal metrics
    // Thread-safe access via atomic operations within ValidationMetrics
    return metrics_;
}

void ValidationCore::reset_metrics() noexcept {
    // GREEN PHASE: Reset all metrics to zero
    metrics_.total_validations.store(0, std::memory_order_relaxed);
    metrics_.successful_validations.store(0, std::memory_order_relaxed);
    metrics_.failed_validations.store(0, std::memory_order_relaxed);
    metrics_.max_latency_ns.store(0, std::memory_order_relaxed);
    metrics_.total_latency_ns.store(0, std::memory_order_relaxed);
}

bool ValidationCore::meets_realtime_constraints(uint64_t max_latency_ns) const noexcept {
    // GREEN PHASE: Simple constraint checking
    uint64_t current_max_latency = metrics_.max_latency_ns.load(std::memory_order_relaxed);
    return current_max_latency <= max_latency_ns;
}

void ValidationCore::update_metrics(ValidationResult result, uint64_t latency_ns) noexcept {
    // REFACTOR PHASE: Optimized atomic metrics update
    
    // Batch atomic operations for cache efficiency
    metrics_.total_validations.fetch_add(1, std::memory_order_relaxed);
    metrics_.total_latency_ns.fetch_add(latency_ns, std::memory_order_relaxed);
    
    // Conditional update (optimized for valid results)
    if (result == ValidationResult::Valid) {
        metrics_.successful_validations.fetch_add(1, std::memory_order_relaxed);
    } else {
        metrics_.failed_validations.fetch_add(1, std::memory_order_relaxed);
    }
    
    // Optimized max latency update (lock-free with early exit)
    uint64_t current_max = metrics_.max_latency_ns.load(std::memory_order_relaxed);
    if (latency_ns > current_max) {
        // Only attempt CAS if we might actually update the max
        while (latency_ns > current_max && 
               !metrics_.max_latency_ns.compare_exchange_weak(current_max, latency_ns, 
                                                            std::memory_order_relaxed,
                                                            std::memory_order_relaxed)) {
            // Retry loop with relaxed ordering for performance
        }
    }
}

uint64_t ValidationCore::get_timestamp_ns() noexcept {
    // GREEN PHASE: High-resolution timestamp
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

} // namespace validation
} // namespace core
} // namespace _2018
} // namespace AES5
} // namespace AES