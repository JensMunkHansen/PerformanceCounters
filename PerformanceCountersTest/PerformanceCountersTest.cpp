/**
 * @file PerformanceCountersTest.cpp
 * @brief Tests for PerformanceCounters library.
 *
 * Tests include:
 * - Basic timing functionality
 * - Cross-module timing aggregation (main exe + DummyLib DLL)
 * - Thread-local accumulator functionality
 */

#include "PerformanceCounters.h"
#include "DummyLib.h"
#include "ScopedTimer.h"
#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <thread>

// Function in main executable that uses the same timer key as DummyLib
void MainExeTimedFunction()
{
    ScopedTimerNamed("SharedTimerKey");

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::microseconds(100));
}

TEST_CASE("PerformanceCounters::Timing::Basic", "[timing]")
{
    // Reset counters before test
    auto& pc = PerformanceCounters::GetInstance();
    pc.ResetAllCounters();

    SECTION("ScopedTimer registers and times function")
    {
        {
            ScopedTimerNamed("TestFunction");
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        pc.CollectAll();

        int id = pc.GetFunctionId("TestFunction");
        REQUIRE(id >= 0);
        REQUIRE(pc.GetFunctionCallCount(id) == 1);
        REQUIRE(pc.GetFunctionTotalTime(id) > 0.0);
    }

    SECTION("Multiple calls accumulate")
    {
        pc.ResetAllCounters();

        for (int i = 0; i < 5; ++i)
        {
            ScopedTimerNamed("MultiCallFunction");
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }

        pc.CollectAll();

        int id = pc.GetFunctionId("MultiCallFunction");
        REQUIRE(id >= 0);
        REQUIRE(pc.GetFunctionCallCount(id) == 5);
    }
}

TEST_CASE("PerformanceCounters::Timing::CrossModule", "[timing][cross-module]")
{
    auto& pc = PerformanceCounters::GetInstance();
    pc.ResetAllCounters();

    SECTION("Timing from DLL and main exe aggregate to same counter")
    {
        // Call the timed function from DummyLib (separate DLL)
        const int dllCalls = 3;
        for (int i = 0; i < dllCalls; ++i)
        {
            DummyLibTimedFunction();
        }

        // Call the timed function from main executable
        const int exeCalls = 2;
        for (int i = 0; i < exeCalls; ++i)
        {
            MainExeTimedFunction();
        }

        // Collect all thread-local accumulators
        pc.CollectAll();

        // Verify the DummyLib tracked its calls locally
        REQUIRE(DummyLibGetLocalCallCount() >= dllCalls);

        // The key test: both DLL and main exe should have contributed
        // to the same "SharedTimerKey" counter
        int id = pc.GetFunctionId("SharedTimerKey");
        REQUIRE(id >= 0);

        int totalCalls = pc.GetFunctionCallCount(id);
        INFO("Total calls from SharedTimerKey: " << totalCalls);
        INFO("Expected: " << (dllCalls + exeCalls));

        // This is the critical assertion for cross-module sharing:
        // If Symbol Interposition works, we should see dllCalls + exeCalls
        // If it doesn't work, we'd see only exeCalls (DLL has separate counter)
        REQUIRE(totalCalls == dllCalls + exeCalls);
    }
}

TEST_CASE("PerformanceCounters::API::Core", "[api]")
{
    auto& pc = PerformanceCounters::GetInstance();
    pc.ResetAllCounters();

    SECTION("GetFunctionCount returns registered functions")
    {
        int initialCount = pc.GetFunctionCount();

        {
            ScopedTimerNamed("NewUniqueFunction");
        }
        pc.CollectAll();

        // Should have at least one more function registered
        REQUIRE(pc.GetFunctionCount() >= initialCount + 1);
    }

    SECTION("GetFunctionName returns correct name")
    {
        {
            ScopedTimerNamed("NameTestFunction");
        }
        pc.CollectAll();

        int id = pc.GetFunctionId("NameTestFunction");
        REQUIRE(id >= 0);
        REQUIRE(pc.GetFunctionName(id) == "NameTestFunction");
    }

    SECTION("GetResultsAsString returns non-empty string")
    {
        {
            ScopedTimerNamed("ResultsStringTest");
        }
        pc.CollectAll();

        std::string results = pc.GetResultsAsString();
        REQUIRE(!results.empty());
        REQUIRE(results.find("ResultsStringTest") != std::string::npos);
    }

    SECTION("ResetAllCounters clears counters")
    {
        {
            ScopedTimerNamed("ResetTestFunction");
        }
        pc.CollectAll();

        int id = pc.GetFunctionId("ResetTestFunction");
        REQUIRE(pc.GetFunctionCallCount(id) > 0);

        pc.ResetAllCounters();

        REQUIRE(pc.GetFunctionCallCount(id) == 0);
    }
}

TEST_CASE("PerformanceCounters::Threading::Safety", "[threading]")
{
    auto& pc = PerformanceCounters::GetInstance();
    pc.ResetAllCounters();

    SECTION("Multiple threads can time concurrently")
    {
        const int numThreads = 4;
        const int callsPerThread = 10;

        std::vector<std::thread> threads;
        for (int t = 0; t < numThreads; ++t)
        {
            threads.emplace_back(
              [callsPerThread]()
              {
                  for (int i = 0; i < callsPerThread; ++i)
                  {
                      ScopedTimerNamed("ConcurrentTimerTest");
                      std::this_thread::sleep_for(std::chrono::microseconds(10));
                  }
              });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        pc.CollectAll();

        int id = pc.GetFunctionId("ConcurrentTimerTest");
        REQUIRE(id >= 0);
        REQUIRE(pc.GetFunctionCallCount(id) == numThreads * callsPerThread);
    }
}
