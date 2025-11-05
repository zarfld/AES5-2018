/**
 * Advanced usage examples for AES5-2018 library
 * Demonstrates professional audio workflows and integration scenarios
 */
import {
  SamplingRateFamily,
  SamplingFrequencies,
  getConversionInfo,
  getRatesInFamily,
  findNearestPreferredRate,
  validateForContext,
  isPreferredSamplingRate,
} from '../src/index.js';

console.log('=== AES5-2018 Advanced Usage Examples ===\n');

// Example 1: AES67/MILAN Network Audio Setup
console.log('1. AES67/MILAN Network Audio Configuration:');
console.log('   Checking recommended rates for audio-over-IP:');
const aes67Rates = [48000, 96000, 192000];
for (const rate of aes67Rates) {
  const validation = validateForContext(rate, 'broadcast');
  console.log(`   ${rate} Hz: ${validation.recommendation} - ${validation.reason}`);
}
console.log();

// Example 2: Multi-format Production Pipeline
console.log('2. Multi-format Production Pipeline:');
console.log('   Scenario: CD mastering (44.1k) → Streaming (48k) → Archive (96k)');

const cdRate = 44100;
const streamingRate = 48000;
const archiveRate = 96000;

const cdToStreaming = getConversionInfo(cdRate, streamingRate);
console.log(`   CD → Streaming: ${cdToStreaming.complexity} complexity (ratio: ${cdToStreaming.ratio.toFixed(4)})`);

const streamingToArchive = getConversionInfo(streamingRate, archiveRate);
console.log(`   Streaming → Archive: ${streamingToArchive.complexity} complexity (${streamingToArchive.method})`);

const cdToArchive = getConversionInfo(cdRate, archiveRate);
console.log(`   CD → Archive Direct: ${cdToArchive.complexity} complexity (ratio: ${cdToArchive.ratio.toFixed(4)})`);
console.log();

// Example 3: Sample Rate Negotiation
console.log('3. Sample Rate Negotiation Between Devices:');
const deviceA = 44100; // Music player
const deviceB = 48000; // Professional interface

console.log(`   Device A operates at: ${deviceA} Hz`);
console.log(`   Device B operates at: ${deviceB} Hz`);

// Find common family rates
const familyA = getRatesInFamily(deviceA);
const familyB = getRatesInFamily(deviceB);

console.log(`   Device A family rates: ${familyA.join(', ')} Hz`);
console.log(`   Device B family rates: ${familyB.join(', ')} Hz`);
console.log('   → Conversion required (different families)');
console.log();

// Example 4: Format Recommendation Engine
console.log('4. Smart Format Recommendation:');
function recommendRate(context, preferredFamily = null) {
  const contexts = {
    'podcast': { context: 'speech', preferred: 16000 },
    'cd-audio': { context: 'music', preferred: 44100 },
    'broadcast': { context: 'broadcast', preferred: 48000 },
    'hi-res-music': { context: 'high-resolution', preferred: 96000 },
    'ultra-archive': { context: 'high-resolution', preferred: 192000 },
  };
  
  const config = contexts[context];
  if (!config) return null;
  
  const validation = validateForContext(config.preferred, config.context);
  return {
    rate: config.preferred,
    validation: validation.recommendation,
    reason: validation.reason,
  };
}

const formats = ['podcast', 'cd-audio', 'broadcast', 'hi-res-music', 'ultra-archive'];
for (const format of formats) {
  const rec = recommendRate(format);
  console.log(`   ${format}: ${rec.rate} Hz (${rec.validation})`);
}
console.log();

// Example 5: Rate Compatibility Matrix
console.log('5. Rate Compatibility Matrix:');
console.log('   Analyzing conversion complexity between common rates:');

const commonRates = [44100, 48000, 96000, 192000];
console.log('\n   From \\ To  |', commonRates.map(r => `${r/1000}k`.padStart(7)).join('|'));
console.log('   ' + '-'.repeat(60));

for (const from of commonRates) {
  const row = commonRates.map(to => {
    if (from === to) return '   -   ';
    const info = getConversionInfo(from, to);
    return info.complexity === 'low' ? '  LOW  ' : '  HIGH ';
  });
  console.log(`   ${(from/1000).toFixed(1)}k     |${row.join('|')}`);
}
console.log();

// Example 6: Hardware Input Validation
console.log('6. Hardware Input Validation:');
const hardwareInputs = [8000, 44100, 48000, 50000, 88200, 96000];

console.log('   Validating hardware sampling rates:');
for (const rate of hardwareInputs) {
  const isValid = isPreferredSamplingRate(rate);
  if (isValid) {
    console.log(`   ✓ ${rate} Hz: Valid AES5-2018 rate`);
  } else {
    const nearest = findNearestPreferredRate(rate);
    console.log(`   ✗ ${rate} Hz: Not preferred (suggest ${nearest.rate} Hz)`);
  }
}
console.log();

// Example 7: Audio Clock Configuration
console.log('7. Audio Clock Configuration for Synchronization:');
const masterClock = 48000;
console.log(`   Master Clock: ${masterClock} Hz`);
console.log('   Derived rates in same family:');

const derivedRates = getRatesInFamily(masterClock);
for (const rate of derivedRates) {
  const info = getConversionInfo(masterClock, rate);
  if (rate !== masterClock) {
    const multiplier = info.ratio;
    console.log(`   - ${rate} Hz (${multiplier}x, ${info.method})`);
  }
}
console.log();

// Example 8: File Format Transcoding Planner
console.log('8. File Format Transcoding Workflow:');
const transcodingTasks = [
  { from: 44100, to: 48000, desc: 'CD to Broadcast' },
  { from: 48000, to: 96000, desc: 'SD to HD' },
  { from: 96000, to: 192000, desc: 'HD to UHD' },
  { from: 88200, to: 176400, desc: 'Hi-Res to Ultra Hi-Res' },
];

for (const task of transcodingTasks) {
  const info = getConversionInfo(task.from, task.to);
  const quality = info.sameFamily ? '★★★' : '★★☆';
  console.log(`   ${task.desc}:`);
  console.log(`     ${task.from} Hz → ${task.to} Hz`);
  console.log(`     Quality: ${quality} (${info.complexity} complexity)`);
  console.log(`     Method: ${info.method || 'Sample Rate Conversion (SRC)'}`);
  console.log();
}

// Example 9: AES3/AES11 Compatibility Check
console.log('9. AES3/AES11 Digital Interface Compatibility:');
const aes3CommonRates = [32000, 44100, 48000, 88200, 96000, 176400, 192000];
console.log('   AES3-compatible rates:');

for (const rate of aes3CommonRates) {
  const familyRates = getRatesInFamily(rate);
  const family = familyRates.includes(48000) ? '48k family' : '44.1k family';
  console.log(`   - ${rate.toString().padEnd(6)} Hz (${family})`);
}
