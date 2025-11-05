import { describe, it } from 'node:test';
import assert from 'node:assert/strict';
import {
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
} from '../src/index.js';

describe('AES5-2018 Sampling Rate Implementation', () => {
  describe('SamplingRateFamily enumeration', () => {
    it('should define 48k family', () => {
      assert.equal(SamplingRateFamily.FAMILY_48K, '48k');
    });

    it('should define 44.1k family', () => {
      assert.equal(SamplingRateFamily.FAMILY_44K, '44.1k');
    });
  });

  describe('SamplingFrequencies reference table', () => {
    it('should contain 48k family rates', () => {
      const rates48k = SamplingFrequencies[SamplingRateFamily.FAMILY_48K];
      assert.ok(Array.isArray(rates48k));
      assert.equal(rates48k.length, 7);
      
      const rateValues = rates48k.map(r => r.rate);
      assert.deepEqual(rateValues, [8000, 16000, 24000, 32000, 48000, 96000, 192000]);
    });

    it('should contain 44.1k family rates', () => {
      const rates44k = SamplingFrequencies[SamplingRateFamily.FAMILY_44K];
      assert.ok(Array.isArray(rates44k));
      assert.equal(rates44k.length, 5);
      
      const rateValues = rates44k.map(r => r.rate);
      assert.deepEqual(rateValues, [11025, 22050, 44100, 88200, 176400]);
    });

    it('should have descriptions for all rates', () => {
      for (const family of Object.values(SamplingRateFamily)) {
        for (const freq of SamplingFrequencies[family]) {
          assert.ok(freq.description);
          assert.ok(typeof freq.description === 'string');
          assert.ok(freq.description.length > 0);
        }
      }
    });

    it('should have multipliers for all rates', () => {
      for (const family of Object.values(SamplingRateFamily)) {
        for (const freq of SamplingFrequencies[family]) {
          assert.ok(typeof freq.multiplier === 'number');
          assert.ok(freq.multiplier > 0);
        }
      }
    });
  });

  describe('AllSamplingRates', () => {
    it('should contain all rates from both families', () => {
      assert.equal(AllSamplingRates.length, 12);
    });

    it('should be sorted in ascending order', () => {
      for (let i = 1; i < AllSamplingRates.length; i++) {
        assert.ok(AllSamplingRates[i] > AllSamplingRates[i - 1]);
      }
    });

    it('should start with 8000 Hz', () => {
      assert.equal(AllSamplingRates[0], 8000);
    });

    it('should end with 192000 Hz', () => {
      assert.equal(AllSamplingRates[AllSamplingRates.length - 1], 192000);
    });
  });

  describe('getSamplingRateFamily()', () => {
    it('should identify 48k family rates', () => {
      assert.equal(getSamplingRateFamily(8000), SamplingRateFamily.FAMILY_48K);
      assert.equal(getSamplingRateFamily(48000), SamplingRateFamily.FAMILY_48K);
      assert.equal(getSamplingRateFamily(96000), SamplingRateFamily.FAMILY_48K);
      assert.equal(getSamplingRateFamily(192000), SamplingRateFamily.FAMILY_48K);
    });

    it('should identify 44.1k family rates', () => {
      assert.equal(getSamplingRateFamily(11025), SamplingRateFamily.FAMILY_44K);
      assert.equal(getSamplingRateFamily(44100), SamplingRateFamily.FAMILY_44K);
      assert.equal(getSamplingRateFamily(88200), SamplingRateFamily.FAMILY_44K);
    });

    it('should return null for non-preferred rates', () => {
      assert.equal(getSamplingRateFamily(12000), null);
      assert.equal(getSamplingRateFamily(50000), null);
      assert.equal(getSamplingRateFamily(100000), null);
    });
  });

  describe('isPreferredSamplingRate()', () => {
    it('should return true for all preferred rates', () => {
      for (const rate of AllSamplingRates) {
        assert.ok(isPreferredSamplingRate(rate), `${rate} should be a preferred rate`);
      }
    });

    it('should return false for non-preferred rates', () => {
      assert.equal(isPreferredSamplingRate(10000), false);
      assert.equal(isPreferredSamplingRate(40000), false);
      assert.equal(isPreferredSamplingRate(50000), false);
    });
  });

  describe('getSamplingRateInfo()', () => {
    it('should return complete info for 48 kHz', () => {
      const info = getSamplingRateInfo(48000);
      assert.ok(info);
      assert.equal(info.rate, 48000);
      assert.equal(info.family, SamplingRateFamily.FAMILY_48K);
      assert.equal(info.multiplier, 1);
      assert.ok(info.description.includes('Professional audio'));
    });

    it('should return complete info for 44.1 kHz', () => {
      const info = getSamplingRateInfo(44100);
      assert.ok(info);
      assert.equal(info.rate, 44100);
      assert.equal(info.family, SamplingRateFamily.FAMILY_44K);
      assert.equal(info.multiplier, 1);
      assert.ok(info.description.includes('CD'));
    });

    it('should return null for non-preferred rates', () => {
      assert.equal(getSamplingRateInfo(12345), null);
    });
  });

  describe('getConversionInfo()', () => {
    it('should indicate simple conversion within 48k family', () => {
      const info = getConversionInfo(48000, 96000);
      assert.ok(info.possible);
      assert.ok(info.sameFamily);
      assert.equal(info.complexity, 'low');
      assert.equal(info.ratio, 2);
      assert.equal(info.method, 'upsampling');
    });

    it('should indicate simple conversion within 44.1k family', () => {
      const info = getConversionInfo(44100, 88200);
      assert.ok(info.possible);
      assert.ok(info.sameFamily);
      assert.equal(info.complexity, 'low');
      assert.equal(info.ratio, 2);
      assert.equal(info.method, 'upsampling');
    });

    it('should indicate downsampling', () => {
      const info = getConversionInfo(96000, 48000);
      assert.ok(info.possible);
      assert.ok(info.sameFamily);
      assert.equal(info.method, 'downsampling');
    });

    it('should indicate complex conversion between families', () => {
      const info = getConversionInfo(48000, 44100);
      assert.ok(info.possible);
      assert.equal(info.sameFamily, false);
      assert.equal(info.complexity, 'high');
      assert.ok(info.reason.includes('sample rate conversion'));
    });

    it('should indicate no conversion needed for same rate', () => {
      const info = getConversionInfo(48000, 48000);
      assert.ok(info.possible);
      assert.ok(info.sameFamily);
      assert.equal(info.method, 'none');
      assert.equal(info.ratio, 1);
    });

    it('should reject non-preferred rates', () => {
      const info = getConversionInfo(12345, 48000);
      assert.equal(info.possible, false);
      assert.ok(info.reason.includes('not preferred'));
    });
  });

  describe('getRatesInFamily()', () => {
    it('should return all 48k family rates', () => {
      const rates = getRatesInFamily(48000);
      assert.ok(Array.isArray(rates));
      assert.equal(rates.length, 7);
      assert.ok(rates.includes(8000));
      assert.ok(rates.includes(48000));
      assert.ok(rates.includes(192000));
    });

    it('should return all 44.1k family rates', () => {
      const rates = getRatesInFamily(44100);
      assert.ok(Array.isArray(rates));
      assert.equal(rates.length, 5);
      assert.ok(rates.includes(11025));
      assert.ok(rates.includes(44100));
      assert.ok(rates.includes(176400));
    });

    it('should return null for non-preferred rates', () => {
      assert.equal(getRatesInFamily(12345), null);
    });
  });

  describe('findNearestPreferredRate()', () => {
    it('should find exact match', () => {
      const result = findNearestPreferredRate(48000);
      assert.equal(result.rate, 48000);
      assert.equal(result.difference, 0);
    });

    it('should find nearest rate for arbitrary value', () => {
      const result = findNearestPreferredRate(47000);
      assert.equal(result.rate, 48000);
      assert.equal(result.difference, 1000);
    });

    it('should find nearest in specified family', () => {
      const result = findNearestPreferredRate(47000, SamplingRateFamily.FAMILY_44K);
      assert.equal(result.rate, 44100);
      assert.equal(result.family, SamplingRateFamily.FAMILY_44K);
    });

    it('should include full rate info', () => {
      const result = findNearestPreferredRate(10000);
      assert.ok(result.description);
      assert.ok(result.family);
      assert.ok(typeof result.multiplier === 'number');
    });
  });

  describe('validateForContext()', () => {
    it('should recommend 48 kHz for broadcast', () => {
      const result = validateForContext(48000, 'broadcast');
      assert.ok(result.valid);
      assert.equal(result.recommendation, 'recommended');
    });

    it('should accept 32 kHz for broadcast', () => {
      const result = validateForContext(32000, 'broadcast');
      assert.ok(result.valid);
      assert.equal(result.recommendation, 'acceptable');
    });

    it('should recommend 44.1 kHz for music', () => {
      const result = validateForContext(44100, 'music');
      assert.ok(result.valid);
      assert.equal(result.recommendation, 'recommended');
    });

    it('should recommend 8 kHz for speech', () => {
      const result = validateForContext(8000, 'speech');
      assert.ok(result.valid);
      assert.equal(result.recommendation, 'recommended');
    });

    it('should not recommend 8 kHz for music', () => {
      const result = validateForContext(8000, 'music');
      assert.equal(result.valid, false);
      assert.equal(result.recommendation, 'not-recommended');
      assert.ok(Array.isArray(result.suggestedRates));
    });

    it('should recommend 96 kHz for high-resolution', () => {
      const result = validateForContext(96000, 'high-resolution');
      assert.ok(result.valid);
      assert.equal(result.recommendation, 'recommended');
    });

    it('should reject non-preferred rates', () => {
      const result = validateForContext(12345, 'broadcast');
      assert.equal(result.valid, false);
    });

    it('should handle unknown context', () => {
      const result = validateForContext(48000, 'unknown-context');
      assert.ok(result.valid);
      assert.equal(result.recommendation, 'unknown');
    });
  });
});
