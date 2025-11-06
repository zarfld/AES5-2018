/**
 * @file rate_category_manager.cpp
 * @brief AES5-2018 Rate Category Manager Implementation
 * @traceability DES-C-003
 * 
 * RED PHASE: Minimal implementation to make TDD tests fail appropriately.
 * This implementation provides stub methods that will fail tests,
 * driving the development of proper AES5-2018 rate category logic.
 */

#include "rate_category_manager.hpp"
#include <algorithm>
#include <chrono>

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace rate_categories {

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

// Private constructor
RateCategoryManager::RateCategoryManager(
    std::unique_ptr<validation::ValidationCore> validation_core) noexcept
    : validation_core_(std::move(validation_core))
    , last_frequency_(0)
    , last_category_(RateCategory::Unknown)
    , last_multiplier_(0.0) {
    // GREEN PHASE: Initialize lookup tables for performance optimization
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

// Internal implementation methods - GREEN PHASE: AES5-2018 Section 5.3 logic
RateCategory RateCategoryManager::classify_rate_category_internal(uint32_t frequency_hz) const noexcept {
    // AES5-2018 Section 5.3 rate category classification
    
    // Quarter rate: 7.75-13.5 kHz
    if (frequency_hz >= QUARTER_RATE_MIN_HZ && frequency_hz <= QUARTER_RATE_MAX_HZ) {
        return RateCategory::Quarter;
    }
    
    // Half rate: 15.5-27 kHz
    if (frequency_hz >= HALF_RATE_MIN_HZ && frequency_hz <= HALF_RATE_MAX_HZ) {
        return RateCategory::Half;
    }
    
    // Basic rate: 31-54 kHz (includes 32k, 44.1k, 48k)
    if (frequency_hz >= BASIC_RATE_MIN_HZ && frequency_hz <= BASIC_RATE_MAX_HZ) {
        return RateCategory::Basic;
    }
    
    // Double rate: 62-108 kHz (includes 88.2k, 96k)
    if (frequency_hz >= DOUBLE_RATE_MIN_HZ && frequency_hz <= DOUBLE_RATE_MAX_HZ) {
        return RateCategory::Double;
    }
    
    // Quadruple rate: 124-216 kHz (includes 176.4k, 192k)
    if (frequency_hz >= QUADRUPLE_RATE_MIN_HZ && frequency_hz <= QUADRUPLE_RATE_MAX_HZ) {
        return RateCategory::Quadruple;
    }
    
    // Octuple rate: 248-432 kHz (includes 352.8k, 384k)
    if (frequency_hz >= OCTUPLE_RATE_MIN_HZ && frequency_hz <= OCTUPLE_RATE_MAX_HZ) {
        return RateCategory::Octuple;
    }
    
    // No matching category
    return RateCategory::Unknown;
}

double RateCategoryManager::calculate_multiplier_internal(uint32_t frequency_hz) const noexcept {
    // Calculate multiplier relative to 48 kHz base frequency
    // Return 0.0 for invalid frequencies (outside any rate category)
    if (frequency_hz == 0) {
        return 0.0;
    }
    
    // Check if frequency is in a valid AES5-2018 rate category
    RateCategory category = classify_rate_category_internal(frequency_hz);
    if (category == RateCategory::Unknown) {
        return 0.0;  // Invalid frequencies should have 0.0 multiplier
    }
    
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