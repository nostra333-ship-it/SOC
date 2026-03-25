#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <json/json.h>

class Incident {
public:
    std::string id;
    std::string description;
    std::string status;
    std::string operatorName;
    std::string createdAt;
    std::string resolvedAt;

    Incident(std::string id, std::string description, std::string operatorName)
        : id(id), description(description), operatorName(operatorName), status("Open") {
        // Capture current time; here we would use a proper time library
        createdAt = "2026-03-25 12:50:08";
    }

    void resolve() {
        status = "Resolved";
        resolvedAt = "2026-03-25 12:50:08";
    }
};

class IncidentManager {
private:
    std::vector<Incident> incidents;

public:
    void createIncident(std::string description, std::string operatorName) {
        std::string id = std::to_string(incidents.size() + 1);
        Incident newIncident(id, description, operatorName);
        incidents.push_back(newIncident);
    }

    void exportToJSON(const std::string &filename) {
        Json::Value jsonData;
        for (const auto& incident : incidents) {
            Json::Value jsonIncident;
            jsonIncident["id"] = incident.id;
            jsonIncident["description"] = incident.description;
            jsonIncident["status"] = incident.status;
            jsonIncident["operator"] = incident.operatorName;
            jsonIncident["createdAt"] = incident.createdAt;
            jsonIncident["resolvedAt"] = incident.resolvedAt;
            jsonData.append(jsonIncident);
        }
        std::ofstream file(filename);
        file << jsonData;
        file.close();
    }

    void exportToCSV(const std::string &filename) {
        std::ofstream file(filename);
        file << "ID,Description,Status,Operator,Created At,Resolved At\n";
        for (const auto& incident : incidents) {
            file << incident.id << "," << incident.description << "," << incident.status << "," << incident.operatorName << "," << incident.createdAt << "," << incident.resolvedAt << "\n";
        }
        file.close();
    }
};

int main() {
    IncidentManager manager;
    manager.createIncident("Network outage detected.", "Operator A");
    manager.createIncident("Database connection issue.", "Operator B");
    manager.incidents[0].resolve(); // Resolving the first incident
    manager.exportToJSON("incidents.json");
    manager.exportToCSV("incidents.csv");

    return 0;
}