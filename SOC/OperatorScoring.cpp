#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class OperatorSession {
public:
    void login(const std::string& operatorId) {
        // Logic to handle login
        std::cout << "Operator " << operatorId << " logged in." << std::endl;
    }

    void logout(const std::string& operatorId) {
        // Logic to handle logout
        std::cout << "Operator " << operatorId << " logged out." << std::endl;
    }
};

class ScoreCalculator {
public:
    double calculateScore(int metrics) {
        // Sample logic for score calculation
        return metrics * 1.5;
    }
};

class ReportExporter {
public:
    void exportReport(const std::vector<double>& scores, const std::string& filename) {
        std::ofstream file;
        file.open(filename);
        for (const auto& score : scores) {
            file << score << std::endl;
        }
        file.close();
        std::cout << "Report exported to " << filename << std::endl;
    }
};

int main() {
    OperatorSession session;
    ScoreCalculator scoreCalc;
    ReportExporter exporter;

    std::string operatorId = "Operator1";
    session.login(operatorId);
    
    // Example: calculate score based on some metrics
    int metrics = 10;  // This should be replaced with actual metric data
    double score = scoreCalc.calculateScore(metrics);
    
    std::vector<double> scores = { score }; // Collect scores
    exporter.exportReport(scores, "score_report.txt");

    session.logout(operatorId);
    return 0;
}