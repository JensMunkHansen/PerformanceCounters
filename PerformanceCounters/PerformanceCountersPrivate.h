/**
 * @file PerformanceCountersPrivate.h
 * @brief Internal implementation details - NOT part of public API.
 *
 * This header contains internal structures with std::atomic members
 * and other implementation details. It is NOT installed and should
 * NOT be included by user code.
 */

#ifndef PERFORMANCECOUNTERS_PRIVATE_H
#define PERFORMANCECOUNTERS_PRIVATE_H

#include "performancecounters_export.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @struct FunctionCounters
 * @brief Global atomic counters for a single function's timing statistics.
 *
 * @internal Not part of public API.
 */
struct FunctionCounters
{
    std::atomic<int64_t> TotalNanoseconds{ 0 };
    std::atomic<int> CallCount{ 0 };
};

/**
 * @struct LocalCounters
 * @brief Per-function timing data stored in thread-local accumulators.
 *
 * @internal Not part of public API.
 */
struct LocalCounters
{
    int64_t Elapsed = 0;
    int Calls = 0;
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
    std::vector<LocalCounters> Counters;

    ThreadAccumulator();
    ~ThreadAccumulator();

    void EnsureCapacity(int size);
    void Flush();
};

/**
 * @brief Thread-local accumulator instance.
 *
 * @internal Not part of public API.
 * Note: thread_local cannot have DLL interface on Windows.
 */
extern thread_local ThreadAccumulator TlsAccum;

// Include the public header to get the FunctionRegistry class declaration
#include "ScopedTimer.h"

/**
 * @struct FunctionRegistry::Impl
 * @brief Internal implementation of FunctionRegistry (PIMPL pattern).
 *
 * Contains all the private data members including mutexes, maps, and atomic counters.
 *
 * @internal Not part of public API.
 */
struct FunctionRegistry::Impl
{
    std::mutex Mutex;
    std::unordered_map<std::string, int> NameToId;
    std::vector<std::string> Names;
    std::vector<std::unique_ptr<FunctionCounters>> Counters;
    std::atomic<int> Count{ 0 };

    std::mutex AccumulatorMutex;
    std::vector<ThreadAccumulator*> Accumulators;
    std::atomic<bool> Destroyed{ false };

    FunctionCounters& GetCounter(int id);
    const std::string& GetName(int id) const;
    std::mutex& GetAccumulatorMutex();
    std::vector<ThreadAccumulator*>& GetAccumulators();
    std::atomic<bool>& GetDestroyed();
};

#ifdef _WIN32
/**
 * @brief Convert Windows performance counter ticks to nanoseconds.
 * @internal Not part of public API.
 */
int64_t TicksToNanoseconds(int64_t ticks);
#endif

/**
 * @class ScopedTimerImpl
 * @brief RAII timer implementation.
 *
 * @internal Not part of public API.
 */
class PERFORMANCECOUNTERS_EXPORT ScopedTimerImpl
{
  public:
    explicit ScopedTimerImpl(int id);
    ~ScopedTimerImpl();

    ScopedTimerImpl(const ScopedTimerImpl&) = delete;
    ScopedTimerImpl& operator=(const ScopedTimerImpl&) = delete;

  private:
    int Id;

#ifdef _WIN32
    LARGE_INTEGER Start;
#else
    std::chrono::steady_clock::time_point Start;
#endif
};

#endif // PERFORMANCECOUNTERS_PRIVATE_H
