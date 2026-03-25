#include "PerformanceMonitor.h"
#include <windows.h>
#include <psapi.h>

PerformanceMonitor::PerformanceMonitor()
{
    m_stats = PerformanceStats();
}

PerformanceStats PerformanceMonitor::getStats() const
{
    PerformanceStats stats = m_stats;

    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    stats.memoryUsage = pmc.WorkingSetSize / (1024 * 1024);

    return stats;
}