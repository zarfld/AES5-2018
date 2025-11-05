/**
 * Basic usage examples for AES5-2018 library
 */
import {
  SamplingRateFamily,
  isPreferredSamplingRate,
  getSamplingRateInfo,
  getConversionInfo,
  getRatesInFamily,
  findNearestPreferredRate,
  validateForContext,
} from '../src/index.js';

console.log('=== AES5-2018 Library Usage Examples ===\n');

// Example 1: Check if rates are preferred
console.log('1. Checking if rates are preferred:');
console.log(`   48000 Hz is preferred: ${isPreferredSamplingRate(48000)}`);
console.log(`   50000 Hz is preferred: ${isPreferredSamplingRate(50000)}`);
console.log();

// Example 2: Get detailed rate information
console.log('2. Getting rate information:');
const info48k = getSamplingRateInfo(48000);
console.log('   48 kHz:', JSON.stringify(info48k, null, 2));
console.log();

const info44k = getSamplingRateInfo(44100);
console.log('   44.1 kHz:', JSON.stringify(info44k, null, 2));
console.log();

// Example 3: Analyze conversions within same family
console.log('3. Conversion analysis (same family):');
const conv1 = getConversionInfo(48000, 96000);
console.log('   48 kHz → 96 kHz:', JSON.stringify(conv1, null, 2));
console.log();

// Example 4: Analyze conversions between families
console.log('4. Conversion analysis (different families):');
const conv2 = getConversionInfo(48000, 44100);
console.log('   48 kHz → 44.1 kHz:', JSON.stringify(conv2, null, 2));
console.log();

// Example 5: Get all rates in a family
console.log('5. Rates in 48 kHz family:');
const rates48k = getRatesInFamily(48000);
console.log('   ', rates48k.join(', ') + ' Hz');
console.log();

console.log('   Rates in 44.1 kHz family:');
const rates44k = getRatesInFamily(44100);
console.log('   ', rates44k.join(', ') + ' Hz');
console.log();

// Example 6: Find nearest preferred rate
console.log('6. Finding nearest preferred rates:');
const nearest1 = findNearestPreferredRate(47000);
console.log('   Nearest to 47000 Hz:', nearest1.rate, 'Hz (diff:', nearest1.difference, 'Hz)');

const nearest2 = findNearestPreferredRate(47000, SamplingRateFamily.FAMILY_44K);
console.log('   Nearest to 47000 Hz in 44.1k family:', nearest2.rate, 'Hz (diff:', nearest2.difference, 'Hz)');
console.log();

// Example 7: Validate for different contexts
console.log('7. Context validation:');
const contexts = ['broadcast', 'music', 'speech', 'high-resolution'];
const testRates = [8000, 44100, 48000, 96000];

for (const context of contexts) {
  console.log(`   ${context}:`);
  for (const rate of testRates) {
    const validation = validateForContext(rate, context);
    console.log(`     ${rate} Hz: ${validation.recommendation}`);
  }
}
console.log();

// Example 8: Professional workflow example
console.log('8. Professional workflow example:');
console.log('   Scenario: Converting CD audio (44.1 kHz) to broadcast (48 kHz)');
const cdToBroadcast = getConversionInfo(44100, 48000);
console.log('   Complexity:', cdToBroadcast.complexity);
console.log('   Reason:', cdToBroadcast.reason);
console.log('   Ratio:', cdToBroadcast.ratio.toFixed(5));
console.log();

console.log('   Scenario: Upsampling professional audio for archival');
const professionalToArchive = getConversionInfo(48000, 192000);
console.log('   Complexity:', professionalToArchive.complexity);
console.log('   Method:', professionalToArchive.method);
console.log('   Ratio:', professionalToArchive.ratio + 'x');
