# AES5-2018

Open-source, hardware-agnostic implementation of **AES5-2018** (Preferred Sampling Frequencies for PCM Audio) providing reference tables, enumerations, and conversion utilities for sample-rate management across AES3, AES11, AES31, AES67, and MILAN stacks.

## Overview

This library implements the Audio Engineering Society's recommended practice for professional digital audio sampling frequencies (AES5-2018 r2023). It provides a standardized way to work with preferred sampling rates across professional audio applications.

## Features

- ✅ Complete reference tables for all AES5-2018 preferred sampling frequencies
- ✅ Two hierarchical rate families (48 kHz and 44.1 kHz)
- ✅ Sample rate validation and family detection
- ✅ Conversion complexity analysis
- ✅ Context-aware rate recommendations
- ✅ Hardware-agnostic, pure JavaScript implementation
- ✅ Zero dependencies
- ✅ Full test coverage

## Installation

```bash
npm install aes5-2018
```

## Quick Start

```javascript
import {
  SamplingRateFamily,
  SamplingFrequencies,
  isPreferredSamplingRate,
  getSamplingRateInfo,
  getConversionInfo,
} from 'aes5-2018';

// Check if a rate is preferred
console.log(isPreferredSamplingRate(48000)); // true
console.log(isPreferredSamplingRate(50000)); // false

// Get rate information
const info = getSamplingRateInfo(48000);
console.log(info);
// {
//   rate: 48000,
//   description: 'Professional audio standard (recommended)',
//   multiplier: 1,
//   family: '48k'
// }

// Analyze conversion between rates
const conversion = getConversionInfo(48000, 96000);
console.log(conversion);
// {
//   possible: true,
//   sameFamily: true,
//   complexity: 'low',
//   ratio: 2,
//   method: 'upsampling'
// }
```

## Sampling Rate Families

AES5-2018 organizes sampling rates into two hierarchical families:

### 48 kHz Family
Based on multiples of 8 kHz, preferred for professional audio, broadcast, and video:
- **8,000 Hz** - Speech/telephony applications
- **16,000 Hz** - Wideband speech/telephony
- **24,000 Hz** - Archive/broadcast applications
- **32,000 Hz** - Digital audio broadcasting
- **48,000 Hz** - Professional audio standard (recommended)
- **96,000 Hz** - High-resolution audio
- **192,000 Hz** - Ultra-high resolution audio

### 44.1 kHz Family
Based on multiples of 11.025 kHz, used for CD audio and music production:
- **11,025 Hz** - Low-quality consumer audio
- **22,050 Hz** - Consumer multimedia
- **44,100 Hz** - CD audio standard
- **88,200 Hz** - High-resolution music production
- **176,400 Hz** - Ultra-high resolution music

## API Reference

### Constants

#### `SamplingRateFamily`
Enumeration of rate families:
```javascript
{
  FAMILY_48K: '48k',
  FAMILY_44K: '44.1k'
}
```

#### `SamplingFrequencies`
Reference table of all preferred frequencies organized by family:
```javascript
{
  '48k': [
    { rate: 8000, description: '...', multiplier: 1/6 },
    // ...
  ],
  '44.1k': [
    { rate: 11025, description: '...', multiplier: 1/4 },
    // ...
  ]
}
```

#### `AllSamplingRates`
Flat sorted array of all preferred rates: `[8000, 11025, 16000, ...]`

### Functions

#### `isPreferredSamplingRate(rate)`
Check if a rate is preferred according to AES5-2018.

**Parameters:**
- `rate` (number) - Sampling rate in Hz

**Returns:** `boolean`

```javascript
isPreferredSamplingRate(48000); // true
isPreferredSamplingRate(50000); // false
```

#### `getSamplingRateFamily(rate)`
Get the family of a sampling rate.

**Parameters:**
- `rate` (number) - Sampling rate in Hz

**Returns:** `string | null` - Family identifier or null

```javascript
getSamplingRateFamily(48000); // '48k'
getSamplingRateFamily(44100); // '44.1k'
getSamplingRateFamily(50000); // null
```

#### `getSamplingRateInfo(rate)`
Get detailed information about a sampling rate.

**Parameters:**
- `rate` (number) - Sampling rate in Hz

**Returns:** `Object | null`

```javascript
getSamplingRateInfo(48000);
// {
//   rate: 48000,
//   description: 'Professional audio standard (recommended)',
//   multiplier: 1,
//   family: '48k'
// }
```

#### `getConversionInfo(fromRate, toRate)`
Analyze conversion complexity between two rates.

**Parameters:**
- `fromRate` (number) - Source rate in Hz
- `toRate` (number) - Target rate in Hz

**Returns:** `Object` - Conversion analysis

```javascript
// Within same family (simple)
getConversionInfo(48000, 96000);
// {
//   possible: true,
//   sameFamily: true,
//   complexity: 'low',
//   ratio: 2,
//   method: 'upsampling',
//   reason: 'Conversion within the same family is simpler'
// }

// Between families (complex)
getConversionInfo(48000, 44100);
// {
//   possible: true,
//   sameFamily: false,
//   complexity: 'high',
//   ratio: 0.91875,
//   reason: 'Conversion between different families requires sample rate conversion (SRC)'
// }
```

#### `getRatesInFamily(rate)`
Get all rates in the same family.

**Parameters:**
- `rate` (number) - Sampling rate in Hz

**Returns:** `number[] | null`

```javascript
getRatesInFamily(48000);
// [8000, 16000, 24000, 32000, 48000, 96000, 192000]
```

#### `findNearestPreferredRate(rate, preferredFamily)`
Find the nearest preferred rate to an arbitrary value.

**Parameters:**
- `rate` (number) - Arbitrary sampling rate in Hz
- `preferredFamily` (string, optional) - Restrict search to a specific family

**Returns:** `Object`

```javascript
findNearestPreferredRate(47000);
// {
//   rate: 48000,
//   difference: 1000,
//   description: 'Professional audio standard (recommended)',
//   multiplier: 1,
//   family: '48k'
// }

findNearestPreferredRate(47000, SamplingRateFamily.FAMILY_44K);
// {
//   rate: 44100,
//   difference: 2900,
//   // ...
// }
```

#### `validateForContext(rate, context)`
Validate if a rate is suitable for a specific application context.

**Parameters:**
- `rate` (number) - Sampling rate in Hz
- `context` (string) - Context: 'broadcast', 'music', 'speech', 'high-resolution'

**Returns:** `Object` - Validation result

```javascript
validateForContext(48000, 'broadcast');
// {
//   valid: true,
//   recommendation: 'recommended',
//   reason: 'Recommended rate for broadcast applications'
// }

validateForContext(8000, 'music');
// {
//   valid: false,
//   recommendation: 'not-recommended',
//   reason: 'Not typically used for music applications',
//   suggestedRates: [44100, 48000, 88200, 96000]
// }
```

## Use Cases

### Professional Audio Applications
- **AES3** - Digital audio interface
- **AES11** - Digital audio synchronization
- **AES31** - Network and file transfer of audio
- **AES67** - High-performance streaming audio-over-IP interoperability standard
- **MILAN** - Media and control protocol for AVB/TSN networks

### Application Contexts

#### Broadcast and Video Production
```javascript
validateForContext(48000, 'broadcast'); // Recommended
validateForContext(96000, 'broadcast'); // Recommended
```

#### Music Production
```javascript
validateForContext(44100, 'music'); // Recommended (CD quality)
validateForContext(48000, 'music'); // Recommended (professional)
validateForContext(96000, 'music'); // Recommended (high-res)
```

#### Speech and Telephony
```javascript
validateForContext(8000, 'speech');  // Recommended
validateForContext(16000, 'speech'); // Recommended (wideband)
```

#### High-Resolution Audio
```javascript
validateForContext(96000, 'high-resolution');  // Recommended
validateForContext(192000, 'high-resolution'); // Recommended
```

## Standards Compliance

This implementation follows:
- **AES5-2018 (r2023)** - AES recommended practice for professional digital audio — Preferred sampling frequencies for applications employing pulse-code modulation
- Compatible with AES3, AES11, AES31, AES67, and MILAN standards

## Testing

```bash
npm test
```

The library includes comprehensive tests covering all functions and edge cases.

## License

MIT

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## References

- [AES5-2018 Standard](https://www.aes.org/publications/standards/search.cfm?docID=14)
- [Audio Engineering Society](https://www.aes.org/)

## Author

Maintained by the AES5-2018 open-source community.
