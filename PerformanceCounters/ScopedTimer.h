/**
 * @file ScopedTimer.h
 * @brief Public API for instrumenting code with timing.
 *
 * This header provides macros for adding timing instrumentation to functions.
 * For querying timing results, include PerformanceCounters.h instead.
 *
 * @section usage_sec Usage
 *
 * Add ScopedTimer() at the start of any function:
 * @code
 * #include "ScopedTimer.h"
 *
 * void MyFunction() {
 *     ScopedTimer();
 *     // ... function body ...
 * }
 * @endcode
 *
 * Or time a specific scope with a custom name:
 * @code
 * void MyFunction() {
 *     {
 *         ScopedTimerNamed("MyFunction::innerLoop");
 *         // ... inner loop ...
 *     }
 * }
 * @endcode
 */

#ifndef SCOPEDTIMER_H
#define SCOPEDTIMER_H

#include "performancecounters_export.h"

#include <memory>

// Include PerformanceCounters.h to ensure the Schwarz counter initializer
// runs in every translation unit that uses ScopedTimer
#include "PerformanceCounters.h"

/**
 * @class FunctionRegistry
 * @brief Minimal public interface for function registration.
 *
 * Only the methods needed for the ScopedTimer macros are exposed here.
 * The full implementation is internal (PIMPL pattern).
 */
class PERFORMANCECOUNTERS_EXPORT FunctionRegistry
{
  public:
    /**
     * @brief Get the singleton instance.
     * @return Reference to the global registry.
     */
    static FunctionRegistry& Instance();

    /**
     * @brief Register a function name and get its ID.
     * @param name Function name (typically from __FUNCTION__).
     * @return ID for the function.
     */
    int RegisterFunction(const char* name);

    /**
     * @brief Find a function ID by name.
     * @param name Function name to search for.
     * @return ID if found, -1 otherwise.
     */
    int FindFunction(const char* name) const;

    /**
     * @brief Get the number of registered functions.
     */
    int GetFunctionCount() const;

  private:
    FunctionRegistry();
    ~FunctionRegistry();
    FunctionRegistry(const FunctionRegistry&) = delete;
    FunctionRegistry& operator=(const FunctionRegistry&) = delete;

    friend class PerformanceCounters;
    friend struct ThreadAccumulator;
    friend struct std::default_delete<FunctionRegistry>;

    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class ScopedTimerHelper
 * @brief RAII timer that measures scope execution time.
 *
 * Records the start time on construction and calculates elapsed time
 * on destruction, accumulating the result in thread-local storage.
 *
 * @par Thread Safety
 * Thread-safe. Each instance operates only on thread-local data.
 *
 * @par Performance
 * - Overhead: ~30-50 ns per timed scope.
 * - Lock-free after registration.
 */
class PERFORMANCECOUNTERS_EXPORT ScopedTimerHelper
{
  public:
    /**
     * @brief Construct a timer and record the start time.
     * @param id The function ID to accumulate timing data for.
     */
    explicit ScopedTimerHelper(int id);

    /**
     * @brief Destructor records elapsed time to thread-local accumulator.
     */
    ~ScopedTimerHelper();

    ScopedTimerHelper(const ScopedTimerHelper&) = delete;
    ScopedTimerHelper& operator=(const ScopedTimerHelper&) = delete;

  private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ----------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------

#ifndef PERFORMANCE_COUNTERS_DISABLE

/**
 * @def ScopedTimer
 * @brief Time the current function using __FUNCTION__.
 *
 * Place at the start of a function to time its entire execution.
 * Uses static local for one-time registration.
 *
 * Define PERFORMANCE_COUNTERS_DISABLE to make this a no-op.
 */
#define ScopedTimer()                                                                              \
    static const int _pc_timer_id_ =                                                               \
      ::FunctionRegistry::Instance().RegisterFunction(__FUNCTION__);                               \
    ::ScopedTimerHelper _pc_timer_(_pc_timer_id_)

/**
 * @def ScopedTimerNamed
 * @brief Time a scope with a custom name.
 *
 * Use for timing specific blocks within a function.
 * @param name Custom name for this timed scope.
 *
 * Define PERFORMANCE_COUNTERS_DISABLE to make this a no-op.
 */
#define ScopedTimerNamed(name)                                                                     \
    static const int _pc_timer_id_ = ::FunctionRegistry::Instance().RegisterFunction(name);        \
    ::ScopedTimerHelper _pc_timer_(_pc_timer_id_)

#else

#define ScopedTimer()          ((void)0)
#define ScopedTimerNamed(name) ((void)0)

#endif // PERFORMANCE_COUNTERS_DISABLE

#endif // SCOPEDTIMER_H
