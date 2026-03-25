#ifndef INCIDENT_MANAGER_H
#define INCIDENT_MANAGER_H

#include <string>
#include <vector>

class Incident {
public:
    std::string id;
    std::string description;
    // Other relevant fields
};

class IncidentManager {
public:
    IncidentManager();
    void reportIncident(const Incident& incident);
    std::vector<Incident> getIncidents() const;
    
private:
    std::vector<Incident> incidents;
};

#endif // INCIDENT_MANAGER_H
