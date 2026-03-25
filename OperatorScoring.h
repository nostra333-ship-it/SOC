# OperatorScoring.h

#ifndef OPERATORS_SCORING_H
#define OPERATORS_SCORING_H

#include <string>
#include <vector>

class OperatorScoring {
public:
    OperatorScoring();
    ~OperatorScoring();

    void trackSession(const std::string &sessionId);
    void addPerformanceMetric(const std::string &metric, double value);
    double calculateScore();
    std::vector<double> getMetrics() const;

private:
    std::string currentSessionId;
    std::vector<std::pair<std::string, double>> performanceMetrics;
};

#endif // OPERATORS_SCORING_H
