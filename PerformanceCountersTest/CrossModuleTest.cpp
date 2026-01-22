/**
 * @file CrossModuleTest.cpp
 * @brief Standalone test demonstrating cross-module timing aggregation.
 *
 * This test verifies that timing from a DLL and main executable
 * aggregate to the same counter when using the same timer key.
 */

#include "DummyLib.h"
#include "PerformanceCounters.h"
#include "ScopedTimer.h"
#include <cstdlib>
#include <iostream>

// Function in main executable using the SAME key as DummyLib
void MainExeFunction()
{
    ScopedTimerNamed("SharedTimerKey");
    // Just a tiny bit of work
    volatile int x = 0;
    for (int i = 0; i < 1000; ++i)
        x += i;
}

int main()
{
    std::cout << "=== Cross-Module Timing Aggregation Test ===\n\n";

    auto& pc = PerformanceCounters::GetInstance();
    pc.ResetAllCounters();

    // Fixed number of calls from DLL
    const int dllCalls = 5;
    std::cout << "Calling DummyLibTimedFunction() " << dllCalls << " times (from DLL)...\n";
    for (int i = 0; i < dllCalls; ++i)
    {
        DummyLibTimedFunction();
    }

    // Fixed number of calls from main executable
    const int exeCalls = 3;
    std::cout << "Calling MainExeFunction() " << exeCalls << " times (from main exe)...\n";
    for (int i = 0; i < exeCalls; ++i)
    {
        MainExeFunction();
    }

    // Collect timing data
    pc.CollectAll();

    // Get results
    int id = pc.GetFunctionId("SharedTimerKey");
    int totalCalls = pc.GetFunctionCallCount(id);

    std::cout << "\n=== Results ===\n";
    std::cout << "Timer key: \"SharedTimerKey\"\n";
    std::cout << "  Calls from DLL:      " << dllCalls << "\n";
    std::cout << "  Calls from main exe: " << exeCalls << "\n";
    std::cout << "  Expected total:      " << (dllCalls + exeCalls) << "\n";
    std::cout << "  Actual total:        " << totalCalls << "\n";

    if (totalCalls == dllCalls + exeCalls)
    {
        std::cout << "\n✓ SUCCESS: Cross-module timing aggregation works!\n";
        std::cout << "  Both DLL and main exe contributed to the same counter.\n";
        return EXIT_SUCCESS;
    }
    else
    {
        std::cout << "\n✗ FAILURE: Cross-module timing aggregation broken!\n";
        std::cout << "  Expected " << (dllCalls + exeCalls) << " but got " << totalCalls << "\n";
        if (totalCalls == exeCalls)
        {
            std::cout << "  (Only main exe calls counted - DLL has separate singleton)\n";
        }
        return EXIT_FAILURE;
    }
}
