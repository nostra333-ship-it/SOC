#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <json/json.h> // Assuming a JSON library is available

class EvidenceExporter {
public:
    // Method to export a single snapshot
    void exportSnapshot(const std::string& snapshotData, const std::string& format) {
        if (format == "json") {
            exportToJson(snapshotData);
        } else if (format == "csv") {
            exportToCsv(snapshotData);
        } else {
            std::cerr << "Unsupported format specified!\n";
        }
    }

    // Method to generate JSON report
    void exportToJson(const std::string& data) {
        Json::Value jsonData;
        jsonData["snapshot"] = data;

        std::ofstream file("report.json");
        file << jsonData;
        file.close();
        std::cout << "JSON Report generated successfully.\n";
    }

    // Method to generate CSV report
    void exportToCsv(const std::string& data) {
        std::ofstream file("report.csv");
        file << "Snapshot\n";
        file << data << std::endl;
        file.close();
        std::cout << "CSV Report generated successfully.\n";
    }

    // Method for batch export functionality
    void batchExport(const std::vector<std::string>& snapshots, const std::string& format) {
        for (const auto& snapshot : snapshots) {
            exportSnapshot(snapshot, format);
        }
        std::cout << "Batch export completed.\n";
    }
};

int main() {
    EvidenceExporter exporter;
    std::string sampleData = "Sample Data for Export";
    exporter.exportSnapshot(sampleData, "json");
    exporter.exportSnapshot(sampleData, "csv");

    std::vector<std::string> batchData = {"Batch Data 1", "Batch Data 2", "Batch Data 3"};
    exporter.batchExport(batchData, "json");
}
