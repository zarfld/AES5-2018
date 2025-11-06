/**
 * @file benchmark_frequency_validator.cpp
 * @brief Performance benchmark for optimized FrequencyValidator
 * @traceability DES-C-001 REFACTOR PHASE: Performance validation
 * 
 * Benchmarks the optimized FrequencyValidator to verify <50μs validation latency.
 */

#include <chrono>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <memory>
#include <cmath>

#include "src/lib/Standards/AES/AES5/2018/core/frequency_validation/frequency_validator.hpp"
#include "src/lib/Standards/AES/AES5/2018/core/compliance/compliance_engine.hpp"
#include "src/lib/Standards/AES/AES5/2018/core/validation/validation_core.hpp"

using namespace AES::AES5::_2018::core;

// Benchmark configuration
constexpr size_t WARMUP_ITERATIONS = 1000;
constexpr size_t BENCHMARK_ITERATIONS = 10000;
constexpr uint64_t TARGET_LATENCY_NS = 50000; // 50μs in nanoseconds

// Test frequencies (representative of real-world usage)
const std::vector<uint32_t> TEST_FREQUENCIES = {
    48000,  // Primary frequency (most common)
    44100,  // Consumer frequency (common)
    48048,  // Pull-up variant
    47952,  // Pull-down variant
    96000,  // High bandwidth
    32000,  // Legacy
    48005,  // Slightly off primary
    48100   // Test tolerance case
};

struct BenchmarkResult {
    double avg_latency_ns;
    double min_latency_ns;
    double max_latency_ns;
    double std_dev_ns;
    size_t total_samples;
    bool meets_requirement;
};

BenchmarkResult benchmark_validation(frequency_validation::FrequencyValidator* validator) {
    std::vector<double> latencies;
    latencies.reserve(BENCHMARK_ITERATIONS);
    
    // Warmup phase
    std::cout << "Warming up..." << std::endl;
    for (size_t i = 0; i < WARMUP_ITERATIONS; ++i) {
        uint32_t freq = TEST_FREQUENCIES[i % TEST_FREQUENCIES.size()];
        validator->validate_frequency(freq, 25);
    }
    
    // Benchmark phase
    std::cout << "Benchmarking " << BENCHMARK_ITERATIONS << " validation operations..." << std::endl;
    
    for (size_t i = 0; i < BENCHMARK_ITERATIONS; ++i) {
        uint32_t freq = TEST_FREQUENCIES[i % TEST_FREQUENCIES.size()];
        
        auto start = std::chrono::high_resolution_clock::now();
        auto result = validator->validate_frequency(freq, 25);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto latency_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        latencies.push_back(static_cast<double>(latency_ns));
    }
    
    // Calculate statistics
    double sum = std::accumulate(latencies.begin(), latencies.end(), 0.0);
    double avg = sum / latencies.size();
    
    auto min_it = std::min_element(latencies.begin(), latencies.end());
    auto max_it = std::max_element(latencies.begin(), latencies.end());
    
    double variance = 0.0;
    for (double latency : latencies) {
        variance += (latency - avg) * (latency - avg);
    }
    variance /= latencies.size();
    double std_dev = std::sqrt(variance);
    
    return {
        avg,
        *min_it,
        *max_it,
        std_dev,
        latencies.size(),
        avg <= TARGET_LATENCY_NS
    };
}

int main() {
    std::cout << "=== FrequencyValidator Performance Benchmark ===" << std::endl;
    std::cout << "Target latency: <" << TARGET_LATENCY_NS / 1000.0 << "μs" << std::endl;
    std::cout << std::endl;
    
    // Create validator components
    auto compliance_engine = std::make_unique<compliance::ComplianceEngine>();
    auto validation_core = std::make_unique<validation::ValidationCore>();
    auto validator = frequency_validation::FrequencyValidator::create(
        std::move(compliance_engine), std::move(validation_core));
    
    if (!validator) {
        std::cerr << "Failed to create FrequencyValidator" << std::endl;
        return 1;
    }
    
    // Run benchmark
    auto result = benchmark_validation(validator.get());
    
    // Report results
    std::cout << "=== BENCHMARK RESULTS ===" << std::endl;
    std::cout << "Total samples: " << result.total_samples << std::endl;
    std::cout << "Average latency: " << result.avg_latency_ns / 1000.0 << "μs" << std::endl;
    std::cout << "Minimum latency: " << result.min_latency_ns / 1000.0 << "μs" << std::endl;
    std::cout << "Maximum latency: " << result.max_latency_ns / 1000.0 << "μs" << std::endl;
    std::cout << "Std deviation: " << result.std_dev_ns / 1000.0 << "μs" << std::endl;
    std::cout << std::endl;
    
    // Performance assessment
    if (result.meets_requirement) {
        std::cout << "✅ PERFORMANCE REQUIREMENT MET!" << std::endl;
        std::cout << "   Average latency (" << result.avg_latency_ns / 1000.0 
                  << "μs) < Target (" << TARGET_LATENCY_NS / 1000.0 << "μs)" << std::endl;
    } else {
        std::cout << "❌ PERFORMANCE REQUIREMENT NOT MET" << std::endl;
        std::cout << "   Average latency (" << result.avg_latency_ns / 1000.0 
                  << "μs) >= Target (" << TARGET_LATENCY_NS / 1000.0 << "μs)" << std::endl;
    }
    
    // Real-time feasibility
    double latency_95th = result.avg_latency_ns + 2 * result.std_dev_ns;
    std::cout << "95th percentile latency (est): " << latency_95th / 1000.0 << "μs" << std::endl;
    
    if (latency_95th <= TARGET_LATENCY_NS) {
        std::cout << "✅ Real-time feasible (95th percentile < target)" << std::endl;
    } else {
        std::cout << "⚠️  Real-time risk (95th percentile >= target)" << std::endl;
    }
    
    // Throughput calculation
    double validations_per_second = 1e9 / result.avg_latency_ns;
    std::cout << "Estimated throughput: " << static_cast<uint64_t>(validations_per_second) 
              << " validations/second" << std::endl;
    
    return result.meets_requirement ? 0 : 1;
}