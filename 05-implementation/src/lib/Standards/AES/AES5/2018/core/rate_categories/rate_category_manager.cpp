/**
 * @file rate_category_manager.cpp
 * @brief AES5-2018 Rate Category Manager Implementation
 * @traceability DES-C-003
 * 
 * REFACTOR PHASE: High-performance implementation optimized for <10μs classification latency.
 * Uses O(1) lookup tables, precomputed multipliers, and optimized execution paths
 * to achieve microsecond-level performance suitable for real-time audio processing.
 */

#include "rate_category_manager.hpp"
#include <algorithm>
#include <chrono>

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace rate_categories {

// REFACTOR PHASE: Initialize high-performance lookup tables
const std::array<RateCategory, RateCategoryManager::FREQUENCY_LOOKUP_SIZE> 
    RateCategoryManager::frequency_to_category_lookup_ = []() {
    std::array<RateCategory, FREQUENCY_LOOKUP_SIZE> lookup{};
    
    // Initialize all frequencies as Unknown
    lookup.fill(RateCategory::Unknown);
    
    // Use precise frequency-by-frequency mapping for exact AES5-2018 compliance
    for (size_t i = 0; i < FREQUENCY_LOOKUP_SIZE; ++i) {
        uint32_t frequency_hz = i * 1000;  // Convert index to Hz
        
        // AES5-2018 Section 5.3 rate category classification
        if (frequency_hz >= QUARTER_RATE_MIN_HZ && frequency_hz <= QUARTER_RATE_MAX_HZ) {
            lookup[i] = RateCategory::Quarter;
        }
        else if (frequency_hz >= HALF_RATE_MIN_HZ && frequency_hz <= HALF_RATE_MAX_HZ) {
            lookup[i] = RateCategory::Half;
        }
        else if (frequency_hz >= BASIC_RATE_MIN_HZ && frequency_hz <= BASIC_RATE_MAX_HZ) {
            lookup[i] = RateCategory::Basic;
        }
        else if (frequency_hz >= DOUBLE_RATE_MIN_HZ && frequency_hz <= DOUBLE_RATE_MAX_HZ) {
            lookup[i] = RateCategory::Double;
        }
        else if (frequency_hz >= QUADRUPLE_RATE_MIN_HZ && frequency_hz <= QUADRUPLE_RATE_MAX_HZ) {
            lookup[i] = RateCategory::Quadruple;
        }
        else if (frequency_hz >= OCTUPLE_RATE_MIN_HZ && frequency_hz <= OCTUPLE_RATE_MAX_HZ) {
            lookup[i] = RateCategory::Octuple;
        }
        // else remains RateCategory::Unknown
    }
    
    return lookup;
}();

const std::array<double, RateCategoryManager::FREQUENCY_LOOKUP_SIZE>
    RateCategoryManager::frequency_to_multiplier_lookup_ = []() {
    std::array<double, FREQUENCY_LOOKUP_SIZE> lookup{};
    
    // Precompute multipliers for all frequencies
    for (size_t i = 0; i < FREQUENCY_LOOKUP_SIZE; ++i) {
        uint32_t frequency_hz = i * 1000;  // Convert index to Hz
        if (frequency_hz == 0) {
            lookup[i] = 0.0;
        } else {
            // Only compute multiplier if frequency is in valid range
            RateCategory category = frequency_to_category_lookup_[i];
            if (category != RateCategory::Unknown) {
                lookup[i] = static_cast<double>(frequency_hz) / static_cast<double>(BASE_FREQUENCY_HZ);
            } else {
                lookup[i] = 0.0;  // Invalid frequencies get 0.0 multiplier
            }
        }
    }
    
    return lookup;
}();

// RateCategoryResult implementation
const char* RateCategoryResult::get_category_name() const noexcept {
    switch (category) {
        case RateCategory::Quarter: return "Quarter Rate";
        case RateCategory::Half: return "Half Rate";
        case RateCategory::Basic: return "Basic Rate";
        case RateCategory::Double: return "Double Rate";
        case RateCategory::Quadruple: return "Quadruple Rate";
        case RateCategory::Octuple: return "Octuple Rate";
        default: return "Unknown";
    }
}

const char* RateCategoryResult::get_aes5_section() const noexcept {
    switch (category) {
        case RateCategory::Quarter:
        case RateCategory::Half:
        case RateCategory::Basic:
        case RateCategory::Double:
        case RateCategory::Quadruple:
        case RateCategory::Octuple:
            return "AES5-2018 Section 5.3";
        default:
            return "Unknown";
    }
}

// REFACTOR PHASE: Optimized constructor with O(1) lookup tables
RateCategoryManager::RateCategoryManager(
    std::unique_ptr<validation::ValidationCore> validation_core) noexcept
    : validation_core_(std::move(validation_core))
    , last_frequency_(0)
    , last_category_(RateCategory::Unknown)
    , last_multiplier_(0.0) {
    // Lookup tables are statically initialized - no runtime initialization required
    frequency_cache_.fill(0);
    category_cache_.fill(RateCategory::Unknown);
    multiplier_cache_.fill(0.0);
}

// Factory method
std::unique_ptr<RateCategoryManager> RateCategoryManager::create(
    std::unique_ptr<validation::ValidationCore> validation_core) noexcept {
    
    // RED PHASE: Basic parameter validation
    if (!validation_core) {
        return nullptr;
    }
    
    // Use private constructor via make_unique workaround
    return std::unique_ptr<RateCategoryManager>(new RateCategoryManager(
        std::move(validation_core)));
}

// Main classification method - GREEN PHASE: AES5-2018 Section 5.3 implementation
RateCategoryResult RateCategoryManager::classify_rate_category(uint32_t frequency_hz) const noexcept {
    // Performance optimization: Check cached result first
    if (frequency_hz == last_frequency_) {
        RateCategoryResult result;
        result.frequency_hz = frequency_hz;
        result.category = last_category_;
        result.multiplier = last_multiplier_;
        result.valid = (last_category_ != RateCategory::Unknown);
        return result;
    }
    
    // Always record metrics via ValidationCore for new frequencies
    validation_core_->validate(frequency_hz, rate_category_validation_function, const_cast<RateCategoryManager*>(this));
    
    // Classify rate category using AES5-2018 Section 5.3 ranges
    RateCategory category = classify_rate_category_internal(frequency_hz);
    double multiplier = calculate_multiplier_internal(frequency_hz);
    bool valid = (category != RateCategory::Unknown);
    
    // Cache result for performance optimization
    last_frequency_ = frequency_hz;
    last_category_ = category;
    last_multiplier_ = multiplier;
    
    // Build result structure
    RateCategoryResult result;
    result.frequency_hz = frequency_hz;
    result.category = category;
    result.multiplier = multiplier;
    result.valid = valid;
    
    return result;
}

// Get metrics from ValidationCore
const validation::ValidationMetrics& RateCategoryManager::get_metrics() const noexcept {
    return validation_core_->get_metrics();
}

// Reset metrics in ValidationCore
void RateCategoryManager::reset_metrics() noexcept {
    validation_core_->reset_metrics();
}

// Check real-time constraints via ValidationCore
bool RateCategoryManager::meets_realtime_constraints(uint64_t max_latency_ns) const noexcept {
    return validation_core_->meets_realtime_constraints(max_latency_ns);
}

// Get memory footprint
size_t RateCategoryManager::get_memory_footprint() const noexcept {
    // RED PHASE: Return estimated size
    return sizeof(RateCategoryManager) + 
           (validation_core_ ? validation_core_->get_memory_footprint() : 0);
}

// Convenience methods - RED PHASE: Stub implementations
RateCategory RateCategoryManager::get_rate_category(uint32_t frequency_hz) const noexcept {
    auto result = classify_rate_category(frequency_hz);
    return result.category;
}

double RateCategoryManager::calculate_rate_multiplier(uint32_t frequency_hz) const noexcept {
    auto result = classify_rate_category(frequency_hz);
    return result.multiplier;
}

bool RateCategoryManager::is_valid_rate_category(uint32_t frequency_hz) const noexcept {
    auto result = classify_rate_category(frequency_hz);
    return result.is_valid();
}

// Internal implementation methods - REFACTOR PHASE: O(1) optimized AES5-2018 logic
RateCategory RateCategoryManager::classify_rate_category_internal(uint32_t frequency_hz) const noexcept {
    // REFACTOR PHASE: Use O(1) lookup table instead of O(n) range checks
    return classify_frequency_optimized(frequency_hz);
}

double RateCategoryManager::calculate_multiplier_internal(uint32_t frequency_hz) const noexcept {
    // REFACTOR PHASE: Use O(1) precomputed multiplier lookup
    return calculate_multiplier_optimized(frequency_hz);
}

// REFACTOR PHASE: O(1) optimized classification methods
RateCategory RateCategoryManager::classify_frequency_optimized(uint32_t frequency_hz) const noexcept {
    // For fractional kHz frequencies, fall back to range-based classification
    // for perfect AES5-2018 compliance while maintaining O(1) for most cases
    
    // Try O(1) lookup first for integer kHz frequencies
    uint32_t frequency_khz = frequency_hz / 1000;
    if (frequency_khz < FREQUENCY_LOOKUP_SIZE && (frequency_hz % 1000) == 0) {
        // Perfect integer kHz - use O(1) lookup
        return frequency_to_category_lookup_[frequency_khz];
    }
    
    // Fractional kHz or out-of-range - use precise range checks for AES5-2018 compliance
    if (frequency_hz >= QUARTER_RATE_MIN_HZ && frequency_hz <= QUARTER_RATE_MAX_HZ) {
        return RateCategory::Quarter;
    }
    if (frequency_hz >= HALF_RATE_MIN_HZ && frequency_hz <= HALF_RATE_MAX_HZ) {
        return RateCategory::Half;
    }
    if (frequency_hz >= BASIC_RATE_MIN_HZ && frequency_hz <= BASIC_RATE_MAX_HZ) {
        return RateCategory::Basic;
    }
    if (frequency_hz >= DOUBLE_RATE_MIN_HZ && frequency_hz <= DOUBLE_RATE_MAX_HZ) {
        return RateCategory::Double;
    }
    if (frequency_hz >= QUADRUPLE_RATE_MIN_HZ && frequency_hz <= QUADRUPLE_RATE_MAX_HZ) {
        return RateCategory::Quadruple;
    }
    if (frequency_hz >= OCTUPLE_RATE_MIN_HZ && frequency_hz <= OCTUPLE_RATE_MAX_HZ) {
        return RateCategory::Octuple;
    }
    
    return RateCategory::Unknown;
}

double RateCategoryManager::calculate_multiplier_optimized(uint32_t frequency_hz) const noexcept {
    // For zero frequency, return 0.0
    if (frequency_hz == 0) {
        return 0.0;
    }
    
    // Check if frequency is in a valid AES5-2018 rate category first
    RateCategory category = classify_frequency_optimized(frequency_hz);
    if (category == RateCategory::Unknown) {
        return 0.0;  // Invalid frequencies get 0.0 multiplier
    }
    
    // Calculate precise multiplier for valid frequencies
    return static_cast<double>(frequency_hz) / static_cast<double>(BASE_FREQUENCY_HZ);
}

// Static validation function for ValidationCore
validation::ValidationResult RateCategoryManager::rate_category_validation_function(
    uint32_t frequency, void* context) noexcept {
    
    // Cast context to RateCategoryManager for classification
    if (!context) {
        return validation::ValidationResult::InvalidInput;
    }
    
    auto* manager = static_cast<const RateCategoryManager*>(context);
    // Use internal classification to avoid recursion
    auto category = manager->classify_rate_category_internal(frequency);
    
    return (category != RateCategory::Unknown) ? 
        validation::ValidationResult::Valid : 
        validation::ValidationResult::InvalidInput;
}

// Utility functions
namespace rate_category_utils {

const char* to_string(RateCategory category) noexcept {
    switch (category) {
        case RateCategory::Quarter: return "Quarter";
        case RateCategory::Half: return "Half";
        case RateCategory::Basic: return "Basic";
        case RateCategory::Double: return "Double";
        case RateCategory::Quadruple: return "Quadruple";
        case RateCategory::Octuple: return "Octuple";
        default: return "Unknown";
    }
}

const char* get_aes5_section(RateCategory category) noexcept {
    switch (category) {
        case RateCategory::Quarter:
        case RateCategory::Half:
        case RateCategory::Basic:
        case RateCategory::Double:
        case RateCategory::Quadruple:
        case RateCategory::Octuple:
            return "5.3";
        default:
            return "Unknown";
    }
}

bool get_frequency_range(RateCategory category, 
                        uint32_t& min_hz, uint32_t& max_hz) noexcept {
    switch (category) {
        case RateCategory::Quarter:
            min_hz = RateCategoryManager::QUARTER_RATE_MIN_HZ;
            max_hz = RateCategoryManager::QUARTER_RATE_MAX_HZ;
            return true;
        case RateCategory::Half:
            min_hz = RateCategoryManager::HALF_RATE_MIN_HZ;
            max_hz = RateCategoryManager::HALF_RATE_MAX_HZ;
            return true;
        case RateCategory::Basic:
            min_hz = RateCategoryManager::BASIC_RATE_MIN_HZ;
            max_hz = RateCategoryManager::BASIC_RATE_MAX_HZ;
            return true;
        case RateCategory::Double:
            min_hz = RateCategoryManager::DOUBLE_RATE_MIN_HZ;
            max_hz = RateCategoryManager::DOUBLE_RATE_MAX_HZ;
            return true;
        case RateCategory::Quadruple:
            min_hz = RateCategoryManager::QUADRUPLE_RATE_MIN_HZ;
            max_hz = RateCategoryManager::QUADRUPLE_RATE_MAX_HZ;
            return true;
        case RateCategory::Octuple:
            min_hz = RateCategoryManager::OCTUPLE_RATE_MIN_HZ;
            max_hz = RateCategoryManager::OCTUPLE_RATE_MAX_HZ;
            return true;
        default:
            return false;
    }
}

} // namespace rate_category_utils

} // namespace rate_categories
} // namespace core
} // namespace _2018
} // namespace AES5
} // namespace AES