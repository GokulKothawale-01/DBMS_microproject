#ifndef INCIDENTMANAGER_H
#define INCIDENTMANAGER_H

#include "Database.h"
#include "Incident.h"
#include "Evidence.h"
#include "Investigator.h"
#include <vector>

class IncidentManager {
private:
    vector<Incident*> cases;
    vector<Evidence> evidenceList;
    vector<Investigator> investigators;
    Database db;

public:
    ~IncidentManager();

    bool connectToDatabase(
        const string& host = "localhost",
        const string& user = "root",
        const string& password = "",
        const string& dbName = "cybercrime_db"
    );

    void createCase();
    void addEvidence();
    void addInvestigator();
    void searchCase(string caseID) const;
    void generateReport(string caseID) const;
    void displayAllCases() const;

    void saveToFile();
    void loadFromFile();
    void loadFromDatabase();
};

#endif