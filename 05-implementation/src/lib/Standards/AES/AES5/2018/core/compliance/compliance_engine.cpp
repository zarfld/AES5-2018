/**
 * @file compliance_engine.cpp
 * @brief AES5-2018 Compliance Verification Engine Implementation
 * @author AI Agent
 * @date 2025-11-06
 * @version 0.1.0
 * 
 * @traceability DES-C-004 → Implementation
 * 
 * GREEN PHASE: Minimal implementation to pass failing tests
 * This implementation provides the minimal functionality required to pass
 * the TDD tests. Will be refactored in subsequent iterations.
 */

#include "compliance_engine.hpp"

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace compliance {

// Static member initialization - compliance lookup table
const std::unordered_map<std::string, std::unordered_set<uint32_t>>
    ComplianceEngine::clause_frequency_map_ = ComplianceEngine::initialize_clause_frequency_map();

// Static member initialization - empty set for error cases
const std::unordered_set<uint32_t> ComplianceEngine::empty_frequency_set_ = {};

bool ComplianceEngine::verify_aes5_clause_compliance(
    uint32_t frequency_hz, 
    const std::string& aes5_clause) const noexcept {
    
    // GREEN PHASE: Minimal implementation to pass tests
    
    // Handle invalid input (zero frequency)
    if (frequency_hz == 0) {
        return false;
    }
    
    // AES5-2018 Section 5.1: Primary sampling frequency (48 kHz)
    if (aes5_clause == "5.1") {
        return frequency_hz == PRIMARY_FREQUENCY_48KHZ;
    }
    
    // AES5-2018 Section 5.2: Other recommended frequencies
    if (aes5_clause == "5.2") {
        return (frequency_hz == OTHER_FREQUENCY_44_1KHZ || 
                frequency_hz == OTHER_FREQUENCY_96KHZ);
    }
    
    // AES5-2018 Section 5.4: Legacy sampling frequencies
    if (aes5_clause == "5.4") {
        return frequency_hz == LEGACY_FREQUENCY_32KHZ;
    }
    
    // AES5-2018 Annex A: All supported frequencies
    if (aes5_clause == "A.1") {
        return (frequency_hz == PRIMARY_FREQUENCY_48KHZ ||
                frequency_hz == OTHER_FREQUENCY_44_1KHZ ||
                frequency_hz == OTHER_FREQUENCY_96KHZ ||
                frequency_hz == LEGACY_FREQUENCY_32KHZ);
    }
    
    // Unknown clause or non-compliant frequency
    return false;
}

const std::unordered_set<uint32_t>& ComplianceEngine::get_supported_frequencies(
    const std::string& aes5_clause) const noexcept {
    
    // GREEN PHASE: Minimal implementation using static lookup
    auto it = clause_frequency_map_.find(aes5_clause);
    if (it != clause_frequency_map_.end()) {
        return it->second;
    }
    
    // Return empty set for unknown clauses
    return empty_frequency_set_;
}

bool ComplianceEngine::is_clause_supported(const std::string& aes5_clause) const noexcept {
    // GREEN PHASE: Simple lookup in static map
    return clause_frequency_map_.find(aes5_clause) != clause_frequency_map_.end();
}

std::unordered_map<std::string, std::unordered_set<uint32_t>>
ComplianceEngine::initialize_clause_frequency_map() noexcept {
    // GREEN PHASE: Initialize static compliance data
    std::unordered_map<std::string, std::unordered_set<uint32_t>> map;
    
    // AES5-2018 Section 5.1: Primary sampling frequency
    map["5.1"] = {PRIMARY_FREQUENCY_48KHZ};
    
    // AES5-2018 Section 5.2: Other recommended frequencies
    map["5.2"] = {OTHER_FREQUENCY_44_1KHZ, OTHER_FREQUENCY_96KHZ};
    
    // AES5-2018 Section 5.4: Legacy sampling frequencies
    map["5.4"] = {LEGACY_FREQUENCY_32KHZ};
    
    // AES5-2018 Annex A: All supported frequencies
    map["A.1"] = {
        PRIMARY_FREQUENCY_48KHZ,
        OTHER_FREQUENCY_44_1KHZ,
        OTHER_FREQUENCY_96KHZ,
        LEGACY_FREQUENCY_32KHZ
    };
    
    return map;
}

} // namespace compliance
} // namespace core
} // namespace _2018
} // namespace AES5
} // namespace AES