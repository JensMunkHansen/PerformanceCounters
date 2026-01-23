/**
 * @file PerformanceCountersPrivate.h
 * @brief Thread-local accumulator for cross-compilation-unit timing.
 *
 * This header exists solely because thread_local variables with extern
 * linkage must be declared in a header to work across compilation units.
 *
 * @internal Not part of public API. Do not include in user code.
 */

#ifndef PERFORMANCECOUNTERS_PRIVATE_H
#define PERFORMANCECOUNTERS_PRIVATE_H

#include "performancecounters_export.h"

#include <cstdint>
#include <vector>

/**
 * @struct LocalCounters
 * @brief Per-function timing data stored in thread-local accumulators.
 *
 * @internal Not part of public API.
 */
struct LocalCounters
{
    int64_t Elapsed = 0;  ///< Accumulated elapsed time in nanoseconds.
    int Calls = 0;        ///< Accumulated call count.
};

/**
 * @struct ThreadAccumulator
 * @brief Thread-local storage for per-function timing data.
 *
 * Accumulates timing data locally to avoid contention, then flushes
 * to global counters periodically.
 *
 * @internal Not part of public API.
 */
struct PERFORMANCECOUNTERS_EXPORT ThreadAccumulator
{
    std::vector<LocalCounters> Counters;  ///< Per-function counters indexed by function ID.

    ThreadAccumulator();
    ~ThreadAccumulator();

    void EnsureCapacity(int size);
    void Flush();
};

/**
 * @brief Thread-local accumulator instance.
 *
 * @internal Not part of public API.
 * @note thread_local cannot have DLL interface on Windows.
 */
extern thread_local ThreadAccumulator TlsAccum;

#endif // PERFORMANCECOUNTERS_PRIVATE_H
