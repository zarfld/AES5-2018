/**
 * AES5-2018 Implementation
 * AES recommended practice for professional digital audio —
 * Preferred sampling frequencies for applications employing pulse-code modulation
 */

/**
 * Enumeration of sampling rate families
 * @readonly
 * @enum {string}
 */
export const SamplingRateFamily = {
  /** 48 kHz family - preferred for professional audio, broadcast, and video */
  FAMILY_48K: '48k',
  /** 44.1 kHz family - used for CD audio and music production */
  FAMILY_44K: '44.1k',
};

/**
 * Reference table of all preferred sampling frequencies defined by AES5-2018
 * Organized by hierarchical families
 * 
 * The 48 kHz family includes rates that are multiples of 8 kHz
 * The 44.1 kHz family includes rates that are multiples of 11.025 kHz
 */
export const SamplingFrequencies = {
  [SamplingRateFamily.FAMILY_48K]: [
    { rate: 8000, description: 'Speech/telephony applications', multiplier: 1/6 },
    { rate: 16000, description: 'Wideband speech/telephony', multiplier: 1/3 },
    { rate: 24000, description: 'Archive/broadcast applications', multiplier: 1/2 },
    { rate: 32000, description: 'Digital audio broadcasting', multiplier: 2/3 },
    { rate: 48000, description: 'Professional audio standard (recommended)', multiplier: 1 },
    { rate: 96000, description: 'High-resolution audio', multiplier: 2 },
    { rate: 192000, description: 'Ultra-high resolution audio', multiplier: 4 },
  ],
  [SamplingRateFamily.FAMILY_44K]: [
    { rate: 11025, description: 'Low-quality consumer audio', multiplier: 1/4 },
    { rate: 22050, description: 'Consumer multimedia', multiplier: 1/2 },
    { rate: 44100, description: 'CD audio standard', multiplier: 1 },
    { rate: 88200, description: 'High-resolution music production', multiplier: 2 },
    { rate: 176400, description: 'Ultra-high resolution music', multiplier: 4 },
  ],
};

/**
 * Flat array of all preferred sampling rates
 */
export const AllSamplingRates = [
  ...SamplingFrequencies[SamplingRateFamily.FAMILY_48K].map(f => f.rate),
  ...SamplingFrequencies[SamplingRateFamily.FAMILY_44K].map(f => f.rate),
].sort((a, b) => a - b);

/**
 * Get the family of a given sampling rate
 * @param {number} rate - Sampling rate in Hz
 * @returns {string|null} The family identifier or null if not a preferred rate
 */
export function getSamplingRateFamily(rate) {
  if (SamplingFrequencies[SamplingRateFamily.FAMILY_48K].some(f => f.rate === rate)) {
    return SamplingRateFamily.FAMILY_48K;
  }
  if (SamplingFrequencies[SamplingRateFamily.FAMILY_44K].some(f => f.rate === rate)) {
    return SamplingRateFamily.FAMILY_44K;
  }
  return null;
}

/**
 * Check if a sampling rate is a preferred rate according to AES5-2018
 * @param {number} rate - Sampling rate in Hz
 * @returns {boolean} True if the rate is a preferred sampling rate
 */
export function isPreferredSamplingRate(rate) {
  return AllSamplingRates.includes(rate);
}

/**
 * Get information about a sampling rate
 * @param {number} rate - Sampling rate in Hz
 * @returns {Object|null} Information object or null if not a preferred rate
 */
export function getSamplingRateInfo(rate) {
  for (const family of Object.values(SamplingRateFamily)) {
    const info = SamplingFrequencies[family].find(f => f.rate === rate);
    if (info) {
      return {
        ...info,
        family,
      };
    }
  }
  return null;
}

/**
 * Convert between sampling rates within the same family
 * @param {number} fromRate - Source sampling rate in Hz
 * @param {number} toRate - Target sampling rate in Hz
 * @returns {Object} Conversion information
 */
export function getConversionInfo(fromRate, toRate) {
  const fromFamily = getSamplingRateFamily(fromRate);
  const toFamily = getSamplingRateFamily(toRate);
  
  if (!fromFamily || !toFamily) {
    return {
      possible: false,
      reason: 'One or both rates are not preferred sampling rates',
      complexity: null,
    };
  }
  
  if (fromFamily !== toFamily) {
    return {
      possible: true,
      sameFamily: false,
      complexity: 'high',
      reason: 'Conversion between different families requires sample rate conversion (SRC)',
      ratio: toRate / fromRate,
    };
  }
  
  return {
    possible: true,
    sameFamily: true,
    complexity: 'low',
    reason: 'Conversion within the same family is simpler',
    ratio: toRate / fromRate,
    method: toRate > fromRate ? 'upsampling' : toRate < fromRate ? 'downsampling' : 'none',
  };
}

/**
 * Get all rates in the same family as the given rate
 * @param {number} rate - Sampling rate in Hz
 * @returns {number[]|null} Array of rates in the same family, or null if rate is not preferred
 */
export function getRatesInFamily(rate) {
  const family = getSamplingRateFamily(rate);
  if (!family) return null;
  
  return SamplingFrequencies[family].map(f => f.rate);
}

/**
 * Find the nearest preferred sampling rate
 * @param {number} rate - Arbitrary sampling rate in Hz
 * @param {string|null} preferredFamily - Optional family to restrict search to
 * @returns {Object} Information about the nearest preferred rate
 */
export function findNearestPreferredRate(rate, preferredFamily = null) {
  let candidates = AllSamplingRates;
  
  if (preferredFamily) {
    candidates = SamplingFrequencies[preferredFamily]?.map(f => f.rate) || AllSamplingRates;
  }
  
  let nearest = candidates[0];
  let minDiff = Math.abs(rate - nearest);
  
  for (const candidate of candidates) {
    const diff = Math.abs(rate - candidate);
    if (diff < minDiff) {
      minDiff = diff;
      nearest = candidate;
    }
  }
  
  return {
    rate: nearest,
    difference: minDiff,
    ...getSamplingRateInfo(nearest),
  };
}

/**
 * Validate if a rate is suitable for a specific application context
 * @param {number} rate - Sampling rate in Hz
 * @param {string} context - Application context (e.g., 'broadcast', 'music', 'speech')
 * @returns {Object} Validation result
 */
export function validateForContext(rate, context) {
  const info = getSamplingRateInfo(rate);
  
  if (!info) {
    return {
      valid: false,
      reason: 'Not a preferred sampling rate according to AES5-2018',
    };
  }
  
  const contextRules = {
    broadcast: {
      recommended: [48000, 96000],
      acceptable: [32000, 192000],
    },
    music: {
      recommended: [44100, 48000, 88200, 96000],
      acceptable: [176400, 192000],
    },
    speech: {
      recommended: [8000, 16000],
      acceptable: [24000, 32000],
    },
    'high-resolution': {
      recommended: [96000, 192000, 88200, 176400],
      acceptable: [48000, 44100],
    },
  };
  
  const rules = contextRules[context];
  if (!rules) {
    return {
      valid: true,
      recommendation: 'unknown',
      reason: 'Unknown context, but rate is a preferred sampling rate',
    };
  }
  
  if (rules.recommended.includes(rate)) {
    return {
      valid: true,
      recommendation: 'recommended',
      reason: `Recommended rate for ${context} applications`,
    };
  }
  
  if (rules.acceptable.includes(rate)) {
    return {
      valid: true,
      recommendation: 'acceptable',
      reason: `Acceptable rate for ${context} applications`,
    };
  }
  
  return {
    valid: false,
    recommendation: 'not-recommended',
    reason: `Not typically used for ${context} applications`,
    suggestedRates: rules.recommended,
  };
}

export default {
  SamplingRateFamily,
  SamplingFrequencies,
  AllSamplingRates,
  getSamplingRateFamily,
  isPreferredSamplingRate,
  getSamplingRateInfo,
  getConversionInfo,
  getRatesInFamily,
  findNearestPreferredRate,
  validateForContext,
};
