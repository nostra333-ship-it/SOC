#ifndef PERFORMANCEMONITOR_H
#define PERFORMANCEMONITOR_H

#include <QObject>
#include <atomic>

struct PerformanceStats {
    float fps = 0;
    float renderTime = 0;
    float decodeTime = 0;
    int memoryUsage = 0;
    int droppedFrames = 0;
};

class PerformanceMonitor : public QObject
{
    Q_OBJECT

public:
    PerformanceMonitor();
    PerformanceStats getStats() const;

private:
    mutable PerformanceStats m_stats;
};

#endif