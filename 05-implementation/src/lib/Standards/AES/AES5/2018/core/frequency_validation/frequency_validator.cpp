/**
 * @file frequency_validator.cpp
 * @brief AES5-2018 Sampling Frequency Validator Implementation
 * @traceability DES-C-001
 * 
 * GREEN PHASE: Minimal implementation to make TDD tests pass
 * This implementation provides basic AES5-2018 frequency validation
 * with tolerance checking and ComplianceEngine/ValidationCore integration.
 */

#include "frequency_validator.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace frequency_validation {

// Static frequency table for efficient lookup
static constexpr std::array<uint32_t, 6> STANDARD_FREQUENCIES = {
    32000,  // Legacy (AES5-2018 Section 5.4)
    44100,  // Consumer (AES5-2018 Section 5.2)  
    47952,  // Pull-down 48k (AES5-2018 Annex A)
    48000,  // Primary (AES5-2018 Section 5.1)
    48048,  // Pull-up 48k (AES5-2018 Annex A)
    96000   // High bandwidth (AES5-2018 Section 5.2)
};

// Mapping frequencies to AES5 clauses
static compliance::AES5Clause get_aes5_clause_for_frequency(uint32_t frequency) noexcept {
    switch (frequency) {
        case 48000: return compliance::AES5Clause::Section_5_1;   // Primary
        case 44100: 
        case 96000: return compliance::AES5Clause::Section_5_2;   // Other
        case 32000: return compliance::AES5Clause::Section_5_4;   // Legacy  
        case 47952:
        case 48048: return compliance::AES5Clause::Annex_A;       // Pull-up/down
        default:    return compliance::AES5Clause::Unknown;
    }
}

// FrequencyValidationResult description implementation
const char* FrequencyValidationResult::get_description() const noexcept {
    switch (status) {
        case validation::ValidationResult::Valid:
            return "Frequency is valid according to AES5-2018";
        case validation::ValidationResult::InvalidInput:
            return "Invalid input frequency (must be > 0)";
        case validation::ValidationResult::OutOfTolerance:
            return "Frequency is outside acceptable tolerance";
        case validation::ValidationResult::InternalError:
            return "Internal validation error";
        default:
            return "Unknown validation result";
    }
}

// Private constructor
FrequencyValidator::FrequencyValidator(
    std::unique_ptr<compliance::ComplianceEngine> compliance_engine,
    std::unique_ptr<validation::ValidationCore> validation_core) noexcept
    : compliance_engine_(std::move(compliance_engine))
    , validation_core_(std::move(validation_core))
    , tolerance_table_size_(0) {
    
    // Initialize standard frequencies for binary search
    std::copy(STANDARD_FREQUENCIES.begin(), STANDARD_FREQUENCIES.end(), 
              standard_frequencies_.begin());
    
    // Initialize tolerance tables
    initialize_tolerance_tables();
}

// Factory method
std::unique_ptr<FrequencyValidator> FrequencyValidator::create(
    std::unique_ptr<compliance::ComplianceEngine> compliance_engine,
    std::unique_ptr<validation::ValidationCore> validation_core) noexcept {
    
    // GREEN PHASE: Basic parameter validation
    if (!compliance_engine || !validation_core) {
        return nullptr;
    }
    
    // Use private constructor via make_unique workaround
    return std::unique_ptr<FrequencyValidator>(new FrequencyValidator(
        std::move(compliance_engine), 
        std::move(validation_core)));
}

// Main validation method
FrequencyValidationResult FrequencyValidator::validate_frequency(
    uint32_t frequency, uint32_t tolerance_ppm) const noexcept {
    
    // GREEN PHASE: Basic validation implementation
    
    // Input validation
    if (frequency == 0) {
        FrequencyValidationResult result;
        result.status = validation::ValidationResult::InvalidInput;
        result.detected_frequency = frequency;
        result.closest_standard_frequency = 0;
        result.tolerance_ppm = 0.0;
        result.applicable_clause = compliance::AES5Clause::Unknown;
        return result;
    }
    
    // Use ValidationCore for performance monitoring
    auto validation_result = validation_core_->validate(
        frequency, frequency_validation_function, 
        const_cast<FrequencyValidator*>(this));
    
    // Internal validation logic
    return validate_frequency_internal(frequency, tolerance_ppm);
}

// Internal validation implementation
FrequencyValidationResult FrequencyValidator::validate_frequency_internal(
    uint32_t frequency, uint32_t tolerance_ppm) const noexcept {
    
    FrequencyValidationResult result;
    result.detected_frequency = frequency;
    
    // Find closest standard frequency
    result.closest_standard_frequency = find_closest_standard_frequency(frequency);
    result.applicable_clause = get_aes5_clause_for_frequency(result.closest_standard_frequency);
    
    // Calculate tolerance
    result.tolerance_ppm = calculate_tolerance_ppm(frequency, result.closest_standard_frequency);
    
    // Check if within tolerance
    if (result.tolerance_ppm <= tolerance_ppm) {
        result.status = validation::ValidationResult::Valid;
    } else {
        result.status = validation::ValidationResult::OutOfTolerance;
    }
    
    return result;
}

// Find closest standard frequency
uint32_t FrequencyValidator::find_closest_standard_frequency(uint32_t frequency) const noexcept {
    // GREEN PHASE: Simple linear search implementation
    
    if (frequency == 0) {
        return STANDARD_FREQUENCIES[0]; // Return first standard frequency
    }
    
    uint32_t closest = STANDARD_FREQUENCIES[0];
    uint32_t min_diff = std::abs(static_cast<int64_t>(frequency) - static_cast<int64_t>(closest));
    
    for (const uint32_t std_freq : STANDARD_FREQUENCIES) {
        uint32_t diff = std::abs(static_cast<int64_t>(frequency) - static_cast<int64_t>(std_freq));
        if (diff < min_diff) {
            min_diff = diff;
            closest = std_freq;
        }
    }
    
    return closest;
}

// Calculate tolerance in parts per million
double FrequencyValidator::calculate_tolerance_ppm(
    uint32_t measured_frequency, uint32_t reference_frequency) const noexcept {
    
    // GREEN PHASE: Basic PPM calculation
    
    if (reference_frequency == 0) {
        return std::numeric_limits<double>::max();
    }
    
    if (measured_frequency == reference_frequency) {
        return 0.0;
    }
    
    // PPM = |measured - reference| / reference * 1,000,000
    double difference = std::abs(static_cast<double>(measured_frequency) - 
                                static_cast<double>(reference_frequency));
    double ppm = (difference / static_cast<double>(reference_frequency)) * 1000000.0;
    
    return ppm;
}

// Get metrics from ValidationCore
const validation::ValidationMetrics& FrequencyValidator::get_metrics() const noexcept {
    return validation_core_->get_metrics();
}

// Reset metrics in ValidationCore
void FrequencyValidator::reset_metrics() noexcept {
    validation_core_->reset_metrics();
}

// Check real-time constraints via ValidationCore
bool FrequencyValidator::meets_realtime_constraints(uint64_t max_latency_ns) const noexcept {
    return validation_core_->meets_realtime_constraints(max_latency_ns);
}

// Initialize tolerance tables
void FrequencyValidator::initialize_tolerance_tables() noexcept {
    // GREEN PHASE: Initialize basic tolerance table
    
    tolerance_table_size_ = 0;
    
    // Add standard frequencies with default tolerances
    const std::array<std::pair<uint32_t, uint32_t>, 6> standard_tolerances = {{
        {32000, DEFAULT_TOLERANCE_PPM},
        {44100, DEFAULT_TOLERANCE_PPM}, 
        {47952, DEFAULT_TOLERANCE_PPM},
        {48000, DEFAULT_TOLERANCE_PPM},
        {48048, DEFAULT_TOLERANCE_PPM},
        {96000, DEFAULT_TOLERANCE_PPM}
    }};
    
    for (const auto& [freq, tolerance] : standard_tolerances) {
        if (tolerance_table_size_ < MAX_TOLERANCE_ENTRIES) {
            FrequencyTolerance& entry = tolerance_table_[tolerance_table_size_];
            entry.nominal_frequency = freq;
            entry.tolerance_ppm = tolerance;
            
            // Calculate min/max frequencies based on PPM tolerance
            double tolerance_factor = static_cast<double>(tolerance) / 1000000.0;
            entry.min_frequency = static_cast<uint32_t>(freq * (1.0 - tolerance_factor));
            entry.max_frequency = static_cast<uint32_t>(freq * (1.0 + tolerance_factor));
            
            tolerance_table_size_++;
        }
    }
}

// Static validation function for ValidationCore integration
validation::ValidationResult frequency_validation_function(uint32_t frequency, void* context) noexcept {
    // GREEN PHASE: Basic static validation function
    
    if (context == nullptr) {
        return validation::ValidationResult::InternalError;
    }
    
    FrequencyValidator* validator = static_cast<FrequencyValidator*>(context);
    
    // Use default tolerance for ValidationCore integration
    FrequencyValidationResult result = validator->validate_frequency_internal(
        frequency, FrequencyValidator::DEFAULT_TOLERANCE_PPM);
    
    return result.status;
}

} // namespace frequency_validation
} // namespace core
} // namespace _2018
} // namespace AES5
} // namespace AES