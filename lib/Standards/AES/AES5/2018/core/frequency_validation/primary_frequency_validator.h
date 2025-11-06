#ifndef AES_AES5_2018_CORE_FREQUENCY_VALIDATION_PRIMARY_FREQUENCY_VALIDATOR_H
#define AES_AES5_2018_CORE_FREQUENCY_VALIDATION_PRIMARY_FREQUENCY_VALIDATOR_H

#include <cstdint>
#include "../../../../Common/interfaces/audio_interface.h"

/**
 * @file primary_frequency_validator.h
 * @brief AES5-2018 Primary Frequency Validator (48 kHz)
 * @namespace AES::AES5::_2018::core::frequency_validation
 * 
 * Implements primary sampling frequency validation according to AES5-2018 Section 5.1.
 * The 48 kHz frequency is designated as the primary sampling frequency for professional
 * digital audio applications.
 * 
 * @see AES5-2018, Section 5.1 "Primary sampling frequency"
 */

namespace AES {
namespace AES5 {
namespace _2018 {
namespace core {
namespace frequency_validation {

/**
 * @brief AES5-2018 compliance result for frequency validation
 */
typedef enum {
    AES5_COMPLIANCE_PASS = 0,           /**< Frequency meets AES5-2018 requirements */
    AES5_COMPLIANCE_FAIL = 1,           /**< Frequency violates AES5-2018 specification */
    AES5_COMPLIANCE_WARNING = 2,        /**< Frequency acceptable but not recommended */
    AES5_COMPLIANCE_UNSUPPORTED = 3     /**< Frequency not covered by AES5-2018 */
} aes5_compliance_result_t;

/**
 * @brief AES5-2018 frequency categories per Section 5.3
 */
typedef enum {
    AES5_CATEGORY_PRIMARY = 0,          /**< 48 kHz primary frequency */
    AES5_CATEGORY_OTHER = 1,            /**< 44.1 kHz, 32 kHz, 96 kHz */
    AES5_CATEGORY_MULTIPLE_2X = 2,      /**< 88.2 kHz, 96 kHz (2× rates) */
    AES5_CATEGORY_MULTIPLE_4X = 3,      /**< 176.4 kHz, 192 kHz (4× rates) */
    AES5_CATEGORY_MULTIPLE_8X = 4,      /**< 352.8 kHz, 384 kHz (8× rates) */
    AES5_CATEGORY_LEGACY = 5,           /**< 32 kHz legacy broadcast */
    AES5_CATEGORY_UNKNOWN = 6           /**< Frequency not in AES5-2018 */
} aes5_frequency_category_t;

/**
 * @class PrimaryFrequencyValidator
 * @brief Validates 48 kHz primary frequency compliance per AES5-2018 Section 5.1
 *
 * The primary frequency validator ensures that 48 kHz sampling frequency
 * implementation meets AES5-2018 requirements including:
 * - Frequency accuracy (<0.01% tolerance)
 * - Primary frequency designation
 * - Optimal processing path selection
 * - Default frequency behavior
 */
class PrimaryFrequencyValidator {
public:
    /**
     * @brief Validate sampling frequency against AES5-2018 Section 5.1
     *
     * Validates that the specified sampling frequency complies with AES5-2018
     * primary frequency recommendations. 48 kHz should be treated as the
     * primary frequency with optimized processing paths.
     *
     * @param sampling_frequency_hz Sampling frequency in Hz to validate
     * @param category Output parameter for AES5-2018 frequency category
     * @return AES5_COMPLIANCE_PASS if frequency meets requirements
     *
     * @note Per AES5-2018 Section 5.1, 48 kHz is the recommended primary
     *       frequency for all professional digital audio applications
     */
    static aes5_compliance_result_t validate_sampling_frequency(
        uint32_t sampling_frequency_hz,
        aes5_frequency_category_t* category);

    /**
     * @brief Check if frequency is the AES5-2018 primary frequency
     *
     * @param sampling_frequency_hz Sampling frequency in Hz to check
     * @return true if frequency is 48 kHz (within tolerance)
     */
    static bool is_primary_frequency(uint32_t sampling_frequency_hz);

    /**
     * @brief Get the AES5-2018 primary frequency value
     *
     * @return 48000 Hz (the AES5-2018 designated primary frequency)
     */
    static constexpr uint32_t get_primary_frequency() {
        return 48000;  // AES5-2018 Section 5.1
    }

    /**
     * @brief Get frequency accuracy tolerance for primary frequency
     *
     * @return Maximum allowable frequency deviation in Hz
     */
    static constexpr uint32_t get_primary_frequency_tolerance() {
        return 5;  // <0.01% tolerance for 48 kHz = ±4.8 Hz
    }

private:
    // Constants from AES5-2018 Section 5.1
    static constexpr uint32_t AES5_PRIMARY_FREQUENCY_HZ = 48000;
    static constexpr uint32_t AES5_PRIMARY_TOLERANCE_HZ = 5;
};

} // namespace frequency_validation
} // namespace core
} // namespace _2018
} // namespace AES5
} // namespace AES

#endif // AES_AES5_2018_CORE_FREQUENCY_VALIDATION_PRIMARY_FREQUENCY_VALIDATOR_H