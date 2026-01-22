/**
 * @file PerformanceCounters.h
 * @brief Public API for per-function timing results.
 *
 * This header provides the public interface for querying timing results.
 * For instrumenting code with timers, include ScopedTimer.h instead.
 *
 * @section singleton_sec Symbol Interposition Singleton
 *
 * This implementation uses the **Symbol Interposition Singleton** pattern
 * combined with the **Schwarz Counter** (Nifty Counter) idiom to ensure
 * correct cross-module sharing of timing data.
 *
 * @section usage_sec Usage
 *
 * Retrieve timing results:
 * @code
 * #include "PerformanceCounters.h"
 * auto& timing = PerformanceCounters::GetInstance();
 * timing.CollectAll();
 * std::cout << timing.GetResultsAsString();
 * @endcode
 *
 * For instrumenting code, see ScopedTimer.h.
 */

#ifndef PERFORMANCECOUNTERS_H
#define PERFORMANCECOUNTERS_H

#include "performancecounters_export.h"

#include <string>

// Forward declaration - internal class
class FunctionRegistry;

/**
 * @class PerformanceCounters
 * @brief Singleton interface for querying function timing results.
 *
 * Multiple modules share the same underlying registry via symbol interposition.
 * Use GetInstance() to access the singleton.
 *
 * @par Thread Safety
 * - CollectAll(): Thread-safe, call after work completes.
 * - GetResultsAsString(): Thread-safe for reading.
 * - ResetAllCounters(): Thread-safe, call when no timing active.
 */
class PERFORMANCECOUNTERS_EXPORT PerformanceCounters
{
  public:
    /**
     * @brief Return the singleton instance.
     * @return Reference to the singleton.
     */
    static PerformanceCounters& GetInstance();

    /**
     * @brief Flush all thread-local accumulators to global counters.
     *
     * Call this after all timed work has completed or from a synchronization
     * point where worker threads are idle.
     */
    void CollectAll();

    /**
     * @brief Get timing results as a formatted string.
     * @return Formatted timing results for all registered functions.
     *
     * Call CollectAll() first for accurate results.
     */
    std::string GetResultsAsString();

    /**
     * @brief Get timing results into a caller-provided buffer (for C interop).
     * @param buffer Output buffer, or nullptr to query required size.
     * @param bufferSize Size of the buffer in bytes.
     * @return Bytes written, required size if buffer is nullptr, or -1 if too small.
     */
    int GetResults(char* buffer, int bufferSize);

    /**
     * @brief Reset all global counters to zero.
     *
     * Call CollectAll() first if you want to capture pending data before reset.
     */
    void ResetAllCounters();

    /**
     * @brief Print timing results to stdout.
     */
    void PrintResults();

    /**
     * @brief Get the number of registered functions.
     */
    int GetFunctionCount();

    /**
     * @brief Get the name of a registered function.
     * @param id The function ID (0 to GetFunctionCount()-1).
     * @return The function name, or empty string if ID is invalid.
     */
    std::string GetFunctionName(int id);

    /**
     * @brief Find a function ID by name.
     * @param name The function name to search for.
     * @return The function ID, or -1 if not found.
     */
    int GetFunctionId(const char* name);

    /**
     * @brief Get the total call count for a function.
     * @param id The function ID.
     * @return Total number of calls, or 0 if ID is invalid.
     */
    int GetFunctionCallCount(int id);
    int GetFunctionCallCount(const char* name);

    /**
     * @brief Get the total elapsed time for a function in seconds.
     * @param id The function ID.
     * @return Total time in seconds, or 0.0 if ID is invalid.
     */
    double GetFunctionTotalTime(int id);
    double GetFunctionTotalTime(const char* name);

    /**
     * @brief Get the average time per call for a function in nanoseconds.
     * @param id The function ID.
     * @return Average time in nanoseconds, or 0 if ID is invalid or no calls.
     */
    double GetFunctionAverageTime(int id);
    double GetFunctionAverageTime(const char* name);

    ~PerformanceCounters();

  protected:
    PerformanceCounters();

    /// Singleton management functions.
    static void ClassInitialize();
    static void ClassFinalize();

    friend class PerformanceCountersInitialize;

  private:
    PerformanceCounters(const PerformanceCounters&) = delete;
    void operator=(const PerformanceCounters&) = delete;

    friend class FunctionRegistry;
    FunctionRegistry& GetRegistry();
    FunctionRegistry* Registry;
};

/**
 * @brief Schwarz counter ensuring initialization before first use.
 *
 * Each translation unit including this header gets a static instance.
 * Constructor increments count and calls ClassInitialize() when count
 * reaches 1. Destructor decrements and calls ClassFinalize() when 0.
 */
class PERFORMANCECOUNTERS_EXPORT PerformanceCountersInitialize
{
  public:
    PerformanceCountersInitialize();
    ~PerformanceCountersInitialize();
};

/// Static instance ensures initialization before first use.
static PerformanceCountersInitialize PerformanceCountersInitializer;

#endif // PERFORMANCECOUNTERS_H
