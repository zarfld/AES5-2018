#ifndef AES_AES5_2018_COMMON_INTERFACES_AUDIO_INTERFACE_H
#define AES_AES5_2018_COMMON_INTERFACES_AUDIO_INTERFACE_H

#include <cstddef>
#include <cstdint>

/**
 * @file audio_interface.h
 * @brief Hardware abstraction interface for AES5-2018 sampling frequency management
 * @namespace Common::interfaces
 * 
 * Provides hardware-agnostic audio interface for AES5-2018 Standards layer.
 * This interface enables dependency injection pattern to keep Standards layer
 * independent of any specific audio hardware or operating system.
 * 
 * The Standards layer receives this interface and uses it for all hardware
 * interactions, maintaining complete hardware independence per copilot instructions.
 */

namespace Common {
namespace interfaces {

/**
 * @brief Audio processing callback function type
 * 
 * @param frame_data Pointer to audio frame data
 * @param length Length of frame data in bytes
 * @param user_data User-defined context data
 * @return 0 on success, negative error code on failure
 */
typedef int (*audio_frame_callback_t)(const void* frame_data, size_t length, void* user_data);

/**
 * @brief Timer callback function type for sample clock generation
 * 
 * @param user_data User-defined context data
 */
typedef void (*timer_callback_t)(void* user_data);

/**
 * @brief Audio hardware capabilities bitfield
 */
typedef enum {
    AUDIO_CAP_NONE = 0x00,              /**< No special capabilities */
    AUDIO_CAP_48KHZ_NATIVE = 0x01,      /**< Native 48kHz support (AES5 primary) */
    AUDIO_CAP_44_1KHZ_NATIVE = 0x02,    /**< Native 44.1kHz support */
    AUDIO_CAP_96KHZ_NATIVE = 0x04,      /**< Native 96kHz support (AES5 high-bandwidth) */
    AUDIO_CAP_192KHZ_SAMPLING = 0x08,   /**< Supports 192kHz sampling (AES5 4× rate) */
    AUDIO_CAP_384KHZ_SAMPLING = 0x10,   /**< Supports 384kHz sampling (AES5 8× rate) */
    AUDIO_CAP_REAL_TIME_SRC = 0x20,     /**< Hardware sample rate conversion */
    AUDIO_CAP_LOW_LATENCY = 0x40        /**< Low-latency audio processing */
} audio_capabilities_t;

/**
 * @brief Audio interface structure for hardware abstraction
 * 
 * This structure provides the complete hardware abstraction interface
 * for the AES5-2018 Standards layer. All hardware-specific functionality
 * is accessed through these function pointers, maintaining hardware
 * independence as required by copilot instructions.
 */
typedef struct {
    /**
     * @brief Send audio frame to hardware output
     * 
     * @param frame_data Pointer to audio frame data
     * @param length Length of frame data in bytes
     * @return 0 on success, negative error code on failure
     */
    int (*send_audio_frame)(const void* frame_data, size_t length);

    /**
     * @brief Receive audio frame from hardware input
     * 
     * @param buffer Buffer to receive audio frame data
     * @param length Pointer to buffer length (input: buffer size, output: actual data length)
     * @return 0 on success, negative error code on failure
     */
    int (*receive_audio_frame)(void* buffer, size_t* length);

    /**
     * @brief Get current sample clock timestamp
     * 
     * @return Current sample clock time in nanoseconds
     */
    uint64_t (*get_sample_clock_ns)(void);

    /**
     * @brief Set sample timer for periodic audio processing
     * 
     * @param sample_rate_hz Desired sample rate in Hz
     * @param callback Callback function to invoke at sample intervals
     * @param user_data User context data for callback
     * @return 0 on success, negative error code on failure
     */
    int (*set_sample_timer)(uint32_t sample_rate_hz, timer_callback_t callback, void* user_data);

    /**
     * @brief Get hardware audio capabilities
     * 
     * @return Bitfield of audio_capabilities_t values
     */
    uint32_t (*get_capabilities)(void);

    /**
     * @brief Set hardware sampling frequency
     * 
     * @param sample_rate_hz Desired sampling frequency in Hz
     * @return 0 on success, negative error code on failure
     */
    int (*set_sample_rate)(uint32_t sample_rate_hz);

    /**
     * @brief Get current hardware sampling frequency
     * 
     * @return Current sampling frequency in Hz, 0 on error
     */
    uint32_t (*get_sample_rate)(void);

    /**
     * @brief User-defined context data
     * 
     * Platform-specific implementation can store context here
     */
    void* user_data;

} audio_interface_t;

/**
 * @class AudioInterfaceValidator
 * @brief Validates audio interface implementations for AES5-2018 compatibility
 * 
 * Provides validation functions to ensure that hardware abstraction
 * implementations are suitable for AES5-2018 Standards layer usage.
 */
class AudioInterfaceValidator {
public:
    /**
     * @brief Validate audio interface for AES5-2018 compatibility
     * 
     * @param interface Audio interface to validate
     * @return true if interface is suitable for AES5-2018 usage
     */
    static bool validate_interface(const audio_interface_t* interface);

    /**
     * @brief Check if interface supports AES5-2018 primary frequency
     * 
     * @param interface Audio interface to check
     * @return true if 48kHz is supported natively or via conversion
     */
    static bool supports_primary_frequency(const audio_interface_t* interface);

    /**
     * @brief Test interface basic functionality
     * 
     * @param interface Audio interface to test
     * @return true if basic send/receive operations work
     */
    static bool test_basic_functionality(const audio_interface_t* interface);
};

} // namespace interfaces
} // namespace Common

#endif // AES_AES5_2018_COMMON_INTERFACES_AUDIO_INTERFACE_H