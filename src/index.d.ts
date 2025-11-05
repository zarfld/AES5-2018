/**
 * TypeScript type definitions for AES5-2018
 */

/**
 * Sampling rate family identifiers
 */
export type SamplingRateFamilyType = '48k' | '44.1k';

/**
 * Enumeration of sampling rate families
 */
export const SamplingRateFamily: {
  readonly FAMILY_48K: '48k';
  readonly FAMILY_44K: '44.1k';
};

/**
 * Information about a specific sampling frequency
 */
export interface SamplingFrequencyInfo {
  /** Sampling rate in Hz */
  rate: number;
  /** Description of typical use cases */
  description: string;
  /** Multiplier relative to base rate (48kHz or 44.1kHz) */
  multiplier: number;
}

/**
 * Information about a sampling frequency including its family
 */
export interface SamplingRateInfoWithFamily extends SamplingFrequencyInfo {
  /** Family identifier */
  family: SamplingRateFamilyType;
}

/**
 * Reference table of all preferred sampling frequencies
 */
export const SamplingFrequencies: {
  readonly '48k': ReadonlyArray<SamplingFrequencyInfo>;
  readonly '44.1k': ReadonlyArray<SamplingFrequencyInfo>;
};

/**
 * Flat array of all preferred sampling rates in Hz, sorted
 */
export const AllSamplingRates: ReadonlyArray<number>;

/**
 * Information about rate conversion
 */
export interface ConversionInfo {
  /** Whether conversion is possible */
  possible: boolean;
  /** Whether rates are in the same family (only if possible) */
  sameFamily?: boolean;
  /** Conversion complexity: 'low' for same family, 'high' for different families, null if not possible */
  complexity: 'low' | 'high' | null;
  /** Explanation of conversion requirements */
  reason: string;
  /** Conversion ratio (toRate / fromRate, only if possible) */
  ratio?: number;
  /** Conversion method: 'upsampling', 'downsampling', 'none' (only if same family) */
  method?: 'upsampling' | 'downsampling' | 'none';
}

/**
 * Result of finding nearest preferred rate
 */
export interface NearestRateResult extends SamplingRateInfoWithFamily {
  /** Absolute difference from requested rate in Hz */
  difference: number;
}

/**
 * Context validation result
 */
export interface ContextValidationResult {
  /** Whether the rate is valid for the context */
  valid: boolean;
  /** Recommendation level */
  recommendation?: 'recommended' | 'acceptable' | 'not-recommended' | 'unknown';
  /** Explanation */
  reason: string;
  /** Suggested rates if not recommended */
  suggestedRates?: number[];
}

/**
 * Application contexts for validation
 */
export type ApplicationContext = 'broadcast' | 'music' | 'speech' | 'high-resolution';

/**
 * Get the family of a given sampling rate
 */
export function getSamplingRateFamily(rate: number): SamplingRateFamilyType | null;

/**
 * Check if a sampling rate is a preferred rate according to AES5-2018
 */
export function isPreferredSamplingRate(rate: number): boolean;

/**
 * Get information about a sampling rate
 */
export function getSamplingRateInfo(rate: number): SamplingRateInfoWithFamily | null;

/**
 * Analyze conversion between sampling rates
 */
export function getConversionInfo(fromRate: number, toRate: number): ConversionInfo;

/**
 * Get all rates in the same family as the given rate
 */
export function getRatesInFamily(rate: number): number[] | null;

/**
 * Find the nearest preferred sampling rate
 */
export function findNearestPreferredRate(
  rate: number,
  preferredFamily?: SamplingRateFamilyType | null
): NearestRateResult;

/**
 * Validate if a rate is suitable for a specific application context
 */
export function validateForContext(
  rate: number,
  context: ApplicationContext | string
): ContextValidationResult;

/**
 * Default export containing all exports
 */
declare const _default: {
  SamplingRateFamily: typeof SamplingRateFamily;
  SamplingFrequencies: typeof SamplingFrequencies;
  AllSamplingRates: typeof AllSamplingRates;
  getSamplingRateFamily: typeof getSamplingRateFamily;
  isPreferredSamplingRate: typeof isPreferredSamplingRate;
  getSamplingRateInfo: typeof getSamplingRateInfo;
  getConversionInfo: typeof getConversionInfo;
  getRatesInFamily: typeof getRatesInFamily;
  findNearestPreferredRate: typeof findNearestPreferredRate;
  validateForContext: typeof validateForContext;
};

export default _default;
