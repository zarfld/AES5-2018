# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-11-05

### Added
- Initial implementation of AES5-2018 standard
- Reference tables for all preferred sampling frequencies
- Two hierarchical rate families (48 kHz and 44.1 kHz)
- `SamplingRateFamily` enumeration for family identifiers
- `SamplingFrequencies` reference table with complete rate information
- `AllSamplingRates` flat array of all preferred rates
- `isPreferredSamplingRate()` function to validate rates
- `getSamplingRateFamily()` function to identify rate families
- `getSamplingRateInfo()` function for detailed rate information
- `getConversionInfo()` function to analyze conversion complexity
- `getRatesInFamily()` function to list family members
- `findNearestPreferredRate()` function for rate approximation
- `validateForContext()` function for context-aware validation
- Comprehensive test suite with 39 tests
- TypeScript type definitions
- Usage examples
- Complete API documentation
- MIT License

### Supported Standards
- AES5-2018 (r2023) - Preferred sampling frequencies
- Compatible with AES3, AES11, AES31, AES67, and MILAN

### Supported Sampling Rates

#### 48 kHz Family
- 8,000 Hz - Speech/telephony
- 16,000 Hz - Wideband speech
- 24,000 Hz - Archive/broadcast
- 32,000 Hz - Digital audio broadcasting
- 48,000 Hz - Professional audio standard
- 96,000 Hz - High-resolution audio
- 192,000 Hz - Ultra-high resolution

#### 44.1 kHz Family
- 11,025 Hz - Low-quality consumer audio
- 22,050 Hz - Consumer multimedia
- 44,100 Hz - CD audio standard
- 88,200 Hz - High-resolution music
- 176,400 Hz - Ultra-high resolution music
