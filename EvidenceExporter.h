#ifndef EVIDENCEEXPORTER_H
#define EVIDENCEEXPORTER_H

class EvidenceExporter {
public:
    // Constructor
    EvidenceExporter();

    // Method to export snapshot
    void exportSnapshot(const std::string& snapshotData);

    // Method to export report
    void exportReport(const std::string& reportData);
};

#endif // EVIDENCEEXPORTER_H