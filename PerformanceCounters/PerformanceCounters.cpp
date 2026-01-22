/**
 * @file PerformanceCounters.cpp
 * @brief Implementation of PerformanceCounters and related classes.
 *
 * Singleton pattern uses symbol interposition:
 * - File-scope globals for instance pointer and initialization count
 * - Schwarz counter idiom for initialization ordering
 * - Symbol interposition handles sharing across statically-linked modules
 */

#include "PerformanceCounters.h"
#include "PerformanceCountersPrivate.h"
#include "ScopedTimer.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>

//----------------------------------------------------------------------------
// The PerformanceCounters singleton pointer.
//
// IMPORTANT: This is a file-scope global, NOT a static class member.
// Must NOT be initialized - default initialization to zero is required.
//
// When PerformanceCounters is built as static libraries (BUILD_SHARED_LIBS=OFF),
// each shared library (.so/.dll) that links PerformanceCounters statically gets
// its own copy of this variable. However, because this symbol has GLOBAL DEFAULT
// binding, the dynamic linker performs "symbol interposition" - the first module
// loaded defines the symbol, and all subsequent modules' references resolve to
// that same address. This allows singleton sharing across modules without
// requiring shared libraries.
PerformanceCounters* PerformanceCountersInstance;

//----------------------------------------------------------------------------
// Schwarz counter for initialization ordering.
//
// Must NOT be initialized - default initialization to zero is required.
//
// Unlike PerformanceCountersInstance above, this counter does NOT need GLOBAL
// binding. Each module has its own counter, and that's fine - what matters
// is that ClassInitialize() checks the interposed PerformanceCountersInstance
// pointer before creating a new singleton.
unsigned int PerformanceCountersInitializeCount;

//----------------------------------------------------------------------------
PerformanceCountersInitialize::PerformanceCountersInitialize()
{
    if (++PerformanceCountersInitializeCount == 1)
    {
        PerformanceCounters::ClassInitialize();
    }
}

//----------------------------------------------------------------------------
PerformanceCountersInitialize::~PerformanceCountersInitialize()
{
    if (--PerformanceCountersInitializeCount == 0)
    {
        PerformanceCounters::ClassFinalize();
    }
}

//----------------------------------------------------------------------------
void PerformanceCounters::ClassInitialize()
{
    if (!PerformanceCountersInstance)
    {
        PerformanceCountersInstance = new PerformanceCounters;
    }
}

//----------------------------------------------------------------------------
void PerformanceCounters::ClassFinalize()
{
    delete PerformanceCountersInstance;
    PerformanceCountersInstance = nullptr;
}

//----------------------------------------------------------------------------
PerformanceCounters& PerformanceCounters::GetInstance()
{
    return *PerformanceCountersInstance;
}

//----------------------------------------------------------------------------
PerformanceCounters::PerformanceCounters()
  : Registry(nullptr)
{
}

//----------------------------------------------------------------------------
PerformanceCounters::~PerformanceCounters()
{
    delete this->Registry;
}

//----------------------------------------------------------------------------
FunctionRegistry& PerformanceCounters::GetRegistry()
{
    if (!this->Registry)
    {
        this->Registry = new FunctionRegistry();
    }
    return *this->Registry;
}

//----------------------------------------------------------------------------
void PerformanceCounters::CollectAll()
{
    auto& reg = FunctionRegistry::Instance();
    std::lock_guard<std::mutex> lock(reg.pImpl->GetAccumulatorMutex());
    for (auto* acc : reg.pImpl->GetAccumulators())
    {
        acc->Flush();
    }
}

//----------------------------------------------------------------------------
std::string PerformanceCounters::GetResultsAsString()
{
    auto& reg = FunctionRegistry::Instance();
    int count = reg.GetFunctionCount();

    std::ostringstream oss;
    oss << "\n=== Function Timing Results ===\n\n";

    for (int i = 0; i < count; ++i)
    {
        int calls = reg.pImpl->GetCounter(i).CallCount.load();
        int64_t totalNs = reg.pImpl->GetCounter(i).TotalNanoseconds.load();
        double totalSec = totalNs / 1e9;

        oss << reg.pImpl->GetName(i) << ":\n"
            << "  Total calls:   " << calls << "\n"
            << "  Total time:    " << totalSec << " s\n";

        if (calls > 0)
        {
            oss << "  Avg per call:  " << (totalNs / calls) << " ns\n";
        }
        oss << "\n";
    }

    return oss.str();
}

//----------------------------------------------------------------------------
int PerformanceCounters::GetResults(char* buffer, int bufferSize)
{
    std::string result = this->GetResultsAsString();
    int requiredSize = static_cast<int>(result.size() + 1);

    if (buffer == nullptr || bufferSize == 0)
    {
        return requiredSize;
    }

    if (bufferSize < requiredSize)
    {
        return -1;
    }

    std::memcpy(buffer, result.c_str(), result.size() + 1);
    return static_cast<int>(result.size());
}

//----------------------------------------------------------------------------
void PerformanceCounters::ResetAllCounters()
{
    auto& reg = FunctionRegistry::Instance();
    int count = reg.GetFunctionCount();

    for (int i = 0; i < count; ++i)
    {
        reg.pImpl->GetCounter(i).TotalNanoseconds.store(0, std::memory_order_relaxed);
        reg.pImpl->GetCounter(i).CallCount.store(0, std::memory_order_relaxed);
    }
}

//----------------------------------------------------------------------------
void PerformanceCounters::PrintResults()
{
    std::cout << this->GetResultsAsString();
}

//----------------------------------------------------------------------------
int PerformanceCounters::GetFunctionCount()
{
    return FunctionRegistry::Instance().GetFunctionCount();
}

//----------------------------------------------------------------------------
std::string PerformanceCounters::GetFunctionName(int id)
{
    auto& reg = FunctionRegistry::Instance();
    if (id < 0 || id >= reg.GetFunctionCount())
    {
        return std::string();
    }
    return reg.pImpl->GetName(id);
}

//----------------------------------------------------------------------------
int PerformanceCounters::GetFunctionId(const char* name)
{
    return FunctionRegistry::Instance().FindFunction(name);
}

//----------------------------------------------------------------------------
int PerformanceCounters::GetFunctionCallCount(int id)
{
    auto& reg = FunctionRegistry::Instance();
    if (id < 0 || id >= reg.GetFunctionCount())
    {
        return 0;
    }
    return reg.pImpl->GetCounter(id).CallCount.load();
}

//----------------------------------------------------------------------------
int PerformanceCounters::GetFunctionCallCount(const char* name)
{
    int id = this->GetFunctionId(name);
    return this->GetFunctionCallCount(id);
}

//----------------------------------------------------------------------------
double PerformanceCounters::GetFunctionTotalTime(int id)
{
    auto& reg = FunctionRegistry::Instance();
    if (id < 0 || id >= reg.GetFunctionCount())
    {
        return 0.0;
    }
    int64_t totalNs = reg.pImpl->GetCounter(id).TotalNanoseconds.load();
    return totalNs / 1e9;
}

//----------------------------------------------------------------------------
double PerformanceCounters::GetFunctionTotalTime(const char* name)
{
    int id = this->GetFunctionId(name);
    return this->GetFunctionTotalTime(id);
}

//----------------------------------------------------------------------------
double PerformanceCounters::GetFunctionAverageTime(int id)
{
    auto& reg = FunctionRegistry::Instance();
    if (id < 0 || id >= reg.GetFunctionCount())
    {
        return 0.0;
    }
    int calls = reg.pImpl->GetCounter(id).CallCount.load();
    if (calls == 0)
    {
        return 0.0;
    }
    int64_t totalNs = reg.pImpl->GetCounter(id).TotalNanoseconds.load();
    return static_cast<double>(totalNs) / calls;
}

//----------------------------------------------------------------------------
double PerformanceCounters::GetFunctionAverageTime(const char* name)
{
    int id = this->GetFunctionId(name);
    return this->GetFunctionAverageTime(id);
}

//----------------------------------------------------------------------------
// FunctionRegistry::Impl internal methods
//----------------------------------------------------------------------------

FunctionCounters& FunctionRegistry::Impl::GetCounter(int id)
{
    return *this->Counters[id];
}

const std::string& FunctionRegistry::Impl::GetName(int id) const
{
    return this->Names[id];
}

std::mutex& FunctionRegistry::Impl::GetAccumulatorMutex()
{
    return this->AccumulatorMutex;
}

std::vector<ThreadAccumulator*>& FunctionRegistry::Impl::GetAccumulators()
{
    return this->Accumulators;
}

std::atomic<bool>& FunctionRegistry::Impl::GetDestroyed()
{
    return this->Destroyed;
}

//----------------------------------------------------------------------------
// FunctionRegistry (public interface with PIMPL)
//----------------------------------------------------------------------------

FunctionRegistry& FunctionRegistry::Instance()
{
    return PerformanceCounters::GetInstance().GetRegistry();
}

FunctionRegistry::FunctionRegistry()
  : pImpl(new Impl)
{
}

FunctionRegistry::~FunctionRegistry()
{
    pImpl->Destroyed.store(true, std::memory_order_release);
    delete pImpl;
}

int FunctionRegistry::RegisterFunction(const char* name)
{
    std::lock_guard<std::mutex> lock(pImpl->Mutex);

    auto it = pImpl->NameToId.find(name);
    if (it != pImpl->NameToId.end())
    {
        return it->second;
    }

    int id = static_cast<int>(pImpl->Names.size());
    pImpl->Names.emplace_back(name);
    pImpl->NameToId[name] = id;
    pImpl->Counters.push_back(std::make_unique<FunctionCounters>());
    pImpl->Count.store(id + 1, std::memory_order_release);
    return id;
}

int FunctionRegistry::FindFunction(const char* name) const
{
    std::lock_guard<std::mutex> lock(pImpl->Mutex);
    auto it = pImpl->NameToId.find(name);
    if (it != pImpl->NameToId.end())
    {
        return it->second;
    }
    return -1;
}

int FunctionRegistry::GetFunctionCount() const
{
    return pImpl->Count.load(std::memory_order_acquire);
}

//----------------------------------------------------------------------------
// ThreadAccumulator
//----------------------------------------------------------------------------

thread_local ThreadAccumulator TlsAccum;

ThreadAccumulator::ThreadAccumulator()
{
    auto& reg = FunctionRegistry::Instance();
    std::lock_guard<std::mutex> lock(reg.pImpl->GetAccumulatorMutex());
    reg.pImpl->GetAccumulators().push_back(this);
}

ThreadAccumulator::~ThreadAccumulator()
{
    this->Flush();

    auto& reg = FunctionRegistry::Instance();
    if (!reg.pImpl->GetDestroyed().load(std::memory_order_acquire))
    {
        std::lock_guard<std::mutex> lock(reg.pImpl->GetAccumulatorMutex());
        auto& v = reg.pImpl->GetAccumulators();
        v.erase(std::remove(v.begin(), v.end(), this), v.end());
    }
}

void ThreadAccumulator::EnsureCapacity(int size)
{
    if (static_cast<int>(this->Counters.size()) < size)
    {
        this->Counters.resize(size);
    }
}

void ThreadAccumulator::Flush()
{
    auto& reg = FunctionRegistry::Instance();
    int count = reg.GetFunctionCount();
    for (int i = 0; i < count && i < static_cast<int>(this->Counters.size()); ++i)
    {
        if (this->Counters[i].Elapsed || this->Counters[i].Calls)
        {
            reg.pImpl->GetCounter(i).TotalNanoseconds.fetch_add(
              this->Counters[i].Elapsed, std::memory_order_relaxed);
            reg.pImpl->GetCounter(i).CallCount.fetch_add(
              this->Counters[i].Calls, std::memory_order_relaxed);
            this->Counters[i].Elapsed = 0;
            this->Counters[i].Calls = 0;
        }
    }
}

//----------------------------------------------------------------------------
// ScopedTimerHelper - PIMPL implementation
//----------------------------------------------------------------------------

struct ScopedTimerHelper::Impl
{
    int Id;
#ifdef _WIN32
    LARGE_INTEGER Start;
#else
    std::chrono::steady_clock::time_point Start;
#endif
};

ScopedTimerHelper::ScopedTimerHelper(int id)
  : pImpl(new Impl)
{
    pImpl->Id = id;
    TlsAccum.EnsureCapacity(id + 1);
#ifdef _WIN32
    QueryPerformanceCounter(&pImpl->Start);
#else
    pImpl->Start = std::chrono::steady_clock::now();
#endif
}

ScopedTimerHelper::~ScopedTimerHelper()
{
    auto& local = TlsAccum.Counters[pImpl->Id];
#ifdef _WIN32
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    local.Elapsed += TicksToNanoseconds(end.QuadPart - pImpl->Start.QuadPart);
#else
    auto end = std::chrono::steady_clock::now();
    local.Elapsed +=
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - pImpl->Start).count();
#endif
    local.Calls++;
    delete pImpl;
}

//----------------------------------------------------------------------------
// ScopedTimerImpl (internal, used by private header)
//----------------------------------------------------------------------------

ScopedTimerImpl::ScopedTimerImpl(int id)
  : Id(id)
{
    TlsAccum.EnsureCapacity(id + 1);
#ifdef _WIN32
    QueryPerformanceCounter(&this->Start);
#else
    this->Start = std::chrono::steady_clock::now();
#endif
}

ScopedTimerImpl::~ScopedTimerImpl()
{
    auto& local = TlsAccum.Counters[this->Id];
#ifdef _WIN32
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    local.Elapsed += TicksToNanoseconds(end.QuadPart - this->Start.QuadPart);
#else
    auto end = std::chrono::steady_clock::now();
    local.Elapsed +=
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - this->Start).count();
#endif
    local.Calls++;
}

#ifdef _WIN32
int64_t TicksToNanoseconds(int64_t ticks)
{
    static const int64_t freq = []()
    {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f.QuadPart;
    }();
    const int64_t seconds = ticks / freq;
    const int64_t remainder = ticks % freq;
    return seconds * 1000000000LL + (remainder * 1000000000LL) / freq;
}
#endif
