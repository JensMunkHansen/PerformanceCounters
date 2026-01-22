/**
 * @file DummyLib.cpp
 * @brief Implementation of dummy library for cross-module timing test.
 */

#include "DummyLib.h"
#include "ScopedTimer.h"

#include <chrono>
#include <thread>

// Track local call count for verification
static int g_localCallCount = 0;

void DummyLibTimedFunction()
{
    // Use the same timer key that the main executable will use
    ScopedTimerNamed("SharedTimerKey");

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::microseconds(100));

    ++g_localCallCount;
}

int DummyLibGetLocalCallCount()
{
    return g_localCallCount;
}
