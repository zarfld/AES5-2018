---
specType: architecture
standard: 42010
phase: 03-architecture
version: 1.0.0
author: AI Agent
date: "2025-11-06"
status: accepted
traceability:
  requirements:
    - SYS-PERF-001
    - SYS-REL-001
    - SYS-CONST-001
---

# ADR-002: Static Memory Allocation Strategy

## Metadata
```yaml
adrId: ADR-002
status: accepted
relatedRequirements:
  - SYS-PERF-001 # Performance requirements (<32KB memory, <5ms latency)
  - SYS-REL-001 # Reliability requirements (99.9% uptime, fault tolerance)
  - SYS-CONST-001 # Hardware abstraction constraints (embedded compatibility)
  - SYS-FUNC-001 # Primary frequency processing requirements
relatedComponents:
  - ARC-C-004 # Buffer Manager
  - ARC-C-005 # Performance Monitor  
  - ARC-C-006 # Memory Pool Management
supersedes: []
supersededBy: null
author: AI Agent
date: 2025-11-06
reviewers: []
```

## Context

The AES5-2018 library faces strict memory and real-time constraints:

**Resource Constraints**:
- **Embedded Systems**: <32KB total memory usage (heap + stack)
- **Real-time Processing**: <5ms latency for frequency conversion operations
- **Reliability Target**: 99.9% uptime with no allocation failures
- **Platform Compatibility**: Support systems without dynamic memory allocation (bare metal RTOS)

**Performance Requirements**:
- **Deterministic Behavior**: Predictable memory access patterns for real-time audio
- **Cache Efficiency**: Memory layout optimized for frequent audio buffer operations
- **Low Latency**: Zero allocation overhead during audio processing hot paths

**Reliability Concerns**:
- **Allocation Failures**: Dynamic allocation can fail unpredictably in resource-constrained environments
- **Memory Fragmentation**: Long-running audio applications suffer from heap fragmentation
- **Error Recovery**: Memory allocation failures are difficult to recover from during audio processing

## Decision

We will implement a **Static Memory Allocation Strategy** with pre-allocated buffer pools:

**Memory Management Approach**:
1. **Pre-allocated Buffer Pools**: All memory allocated at library initialization
2. **Zero Dynamic Allocation**: No malloc/new calls during audio processing
3. **Configurable Pool Sizes**: Platform-specific memory configuration
4. **Buffer Recycling**: Efficient reuse of temporary processing buffers

**Implementation Strategy**:
```cpp
// Platform-specific memory configuration
struct aes5_memory_config {
    size_t buffer_count;     // Number of pre-allocated audio buffers
    size_t buffer_size;      // Size per buffer (samples * channels * sample_size)
    size_t stack_reserve;    // Reserved stack space for processing
    bool allow_dynamic;      // Allow fallback to dynamic allocation (desktop only)
};

// Embedded system configuration (32KB total)
constexpr aes5_memory_config embedded_config = {
    .buffer_count = 4,       // 4 processing buffers
    .buffer_size = 2048,     // 2KB per buffer (512 samples * 2ch * 2 bytes)
    .stack_reserve = 8192,   // 8KB stack reserve
    .allow_dynamic = false   // No dynamic allocation permitted
};
```

## Status

**Accepted** (2025-11-06) - This decision provides deterministic performance and reliability required for embedded real-time audio processing while meeting the <32KB memory constraint.

## Rationale

Static allocation addresses the fundamental tension between **resource constraints** and **reliability requirements**:

**Deterministic Performance**: Pre-allocated buffers eliminate allocation overhead from audio processing hot paths, ensuring consistent <5ms latency.

**Embedded Compatibility**: Works on bare metal systems without heap management, supporting the broadest range of deployment platforms.

**Reliability Assurance**: Eliminates allocation failure modes during operation, supporting 99.9% uptime requirement.

**Memory Efficiency**: While potentially less memory-efficient than dynamic allocation, provides predictable memory usage within 32KB constraint.

## Considered Alternatives

| Alternative | Summary | Pros | Cons | Reason Not Chosen |
|------------|---------|------|------|-------------------|
| **Dynamic Allocation** | Standard malloc/new during processing | Memory efficient, flexible | Unpredictable latency, allocation failures | Violates real-time and reliability requirements |
| **Custom Memory Allocator** | Stack-based or pool allocator | Controlled allocation, debugging | Complex implementation, platform-specific | Exceeds implementation complexity budget |
| **Memory Mapping** | mmap/VirtualAlloc for large buffers | OS virtual memory benefits | Platform-specific, complexity | Not available on embedded RTOS platforms |
| **Hybrid Approach** | Static for hot paths, dynamic for initialization | Best of both approaches | Complex memory management | Increases implementation and testing complexity |

## Consequences

### Positive
- **Deterministic Performance**: Zero allocation overhead during audio processing
- **Real-time Compliance**: Guaranteed memory availability for time-critical operations
- **Embedded Compatibility**: Works on systems without heap or with disabled dynamic allocation
- **Reliability**: Eliminates memory allocation as failure mode during operation
- **Cache Efficiency**: Contiguous buffer layout improves CPU cache performance

### Negative / Liabilities
- **Memory Efficiency**: May waste memory with fixed allocations (mitigation: configurable pool sizes)
- **Flexibility Limitation**: Cannot adapt to varying workloads at runtime (mitigation: conservative buffer sizing)
- **Configuration Complexity**: Requires accurate prediction of memory requirements (mitigation: automated calculation tools)

### Neutral / Follow-ups
- Must implement buffer overflow detection and recovery mechanisms
- Need automated tools for calculating optimal buffer pool sizes
- Require platform-specific memory configuration validation
- Must provide fallback strategies for buffer pool exhaustion

## Quality Attribute Impact Matrix

| Quality Attribute | Impact (+/−/0) | Notes |
|-------------------|----------------|-------|
| **Performance** | ++ | Eliminates allocation overhead, predictable memory access |
| **Reliability** | ++ | No allocation failures, deterministic memory usage |
| **Portability** | + | Works on embedded systems without heap management |
| **Maintainability** | - | Requires careful buffer size calculation and management |
| **Scalability** | - | Fixed memory limits constrain maximum workload |
| **Security** | + | Reduces attack surface by eliminating dynamic allocation |

## Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|-----------|
| **Buffer Pool Exhaustion** | Medium | High | Conservative sizing, overflow detection, graceful degradation |
| **Memory Configuration Errors** | Medium | Medium | Automated calculation tools, platform-specific validation |
| **Suboptimal Memory Usage** | High | Low | Configurable pool sizes, profiling tools for optimization |
| **Platform Memory Constraints** | Low | High | Platform capability detection, adaptive configuration |

## Compliance Mapping

| Standard Clause | How Addressed |
|-----------------|---------------|
| **ISO 42010 §5.8 (Rationale)** | Trade-off analysis between memory efficiency and deterministic performance |
| **IEEE 1012 (Verification)** | Memory usage validation through automated testing |
| **AES5-2018 (Performance)** | Static allocation ensures consistent frequency processing performance |

## Implementation Notes

**Phase 1: Buffer Pool Implementation**
```cpp
// Lock-free buffer pool for real-time audio processing
class StaticBufferPool {
private:
    struct Buffer {
        void* data;
        size_t size;
        std::atomic<bool> in_use{false};
    };
    
    std::array<Buffer, MAX_BUFFERS> buffers_;
    size_t buffer_count_;
    
public:
    Buffer* acquire_buffer() noexcept;
    void release_buffer(Buffer* buffer) noexcept;
    bool is_pool_healthy() const noexcept;
};
```

**Phase 2: Memory Configuration System**
```cpp
// Platform-specific memory configuration
class MemoryConfigurator {
public:
    static aes5_memory_config detect_platform_config();
    static bool validate_config(const aes5_memory_config& config);
    static size_t calculate_required_memory(const aes5_memory_config& config);
};
```

**Phase 3: Buffer Management Integration**
```cpp
// Integration with AES5 frequency processing
class FrequencyProcessor {
private:
    StaticBufferPool& buffer_pool_;
    
public:
    aes5_result_t process_frequency_conversion(
        const audio_buffer_t& input,
        audio_buffer_t& output) noexcept;
};
```

## Validation Plan

**Memory Usage Validation**:
1. Measure actual memory usage across all target platforms
2. Validate <32KB constraint compliance under maximum load
3. Test buffer pool exhaustion scenarios and recovery
4. Profile memory access patterns for cache efficiency

**Performance Validation**:
1. Benchmark allocation overhead elimination (static vs. dynamic)
2. Measure latency consistency over extended operation periods
3. Validate real-time constraint compliance under stress testing
4. Compare memory access latency across different buffer layouts

**Reliability Validation**:
1. Long-duration testing (1000+ hours) without memory issues
2. Stress testing with buffer pool at capacity limits
3. Fault injection testing for buffer overflow conditions
4. Platform compatibility testing across embedded and desktop systems

**Configuration Validation**:
1. Automated testing of memory configuration calculation tools
2. Platform-specific constraint validation
3. Buffer size optimization through profiling
4. Cross-platform consistency verification

## References

- System Requirements Specification (SYS-PERF-001, SYS-REL-001)
- "Real-Time Systems Design and Analysis" - Philip A. Laplante
- "Embedded Systems Architecture" - Tammy Noergaard  
- ARM Cortex-M Programming Guidelines for Memory Management
- RTOS Memory Management Best Practices Documentation