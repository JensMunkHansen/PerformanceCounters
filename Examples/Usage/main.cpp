#include <iostream>
#include <thread>
#include <chrono>
#include "PerformanceCounters.h"
#include "ScopedTimer.h"

// Example function that will be timed
void DoSomeWork()
{
    ScopedTimer();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// Another example with a custom timer name
void ProcessData()
{
    ScopedTimerNamed("ProcessData::MainLoop");

    for (int i = 0; i < 5; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

int main()
{
    std::cout << "PerformanceCounters Usage Example\n";
    std::cout << "==================================\n\n";

    // Time some work
    std::cout << "Calling DoSomeWork() 3 times...\n";
    for (int i = 0; i < 3; ++i)
    {
        DoSomeWork();
    }

    std::cout << "Calling ProcessData() once...\n";
    ProcessData();

    // Collect all timing data from thread-local accumulators
    auto& pc = PerformanceCounters::GetInstance();
    pc.CollectAll();

    // Print results
    std::cout << "\n";
    pc.PrintResults();

    // Demonstrate API usage
    std::cout << "API Demo:\n";
    std::cout << "---------\n";
    std::cout << "Registered functions: " << pc.GetFunctionCount() << "\n";

    int id = pc.GetFunctionId("DoSomeWork");
    if (id >= 0)
    {
        std::cout << "DoSomeWork:\n";
        std::cout << "  Call count: " << pc.GetFunctionCallCount(id) << "\n";
        std::cout << "  Total time: " << pc.GetFunctionTotalTime(id) << " s\n";
        std::cout << "  Avg time:   " << pc.GetFunctionAverageTime(id) << " ns\n";
    }

    return 0;
}
