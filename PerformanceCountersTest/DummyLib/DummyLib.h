/**
 * @file DummyLib.h
 * @brief Dummy library for testing cross-module timing aggregation.
 *
 * This library contains a timed function that uses the same timer key
 * as functions in other modules. When properly using Symbol Interposition
 * Singleton, timing from this DLL and the main executable should aggregate
 * to the same counters.
 */

#ifndef DUMMYLIB_H
#define DUMMYLIB_H

#include "dummylib_export.h"

/**
 * @brief A timed function in the dummy library.
 *
 * Uses ScopedTimerNamed("SharedTimerKey") internally.
 * When called, it should add to the same counter that other modules
 * use when they also time with "SharedTimerKey".
 */
DUMMYLIB_EXPORT void DummyLibTimedFunction();

/**
 * @brief Get the number of times DummyLibTimedFunction was called.
 * @return Call count from this library's perspective.
 */
DUMMYLIB_EXPORT int DummyLibGetLocalCallCount();

#endif // DUMMYLIB_H
