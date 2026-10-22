#include "IncidentManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

IncidentManager::~IncidentManager() {
    for (auto inc : cases) {
        delete inc;
    }
}

bool IncidentManager::connectToDatabase(
    const string& host,
    const string& user,
    const string& password,
    const string& dbName
) {
    if (db.connect(host, user, password, dbName)) {
        cout << ">> Connected to MySQL database successfully.\n";
        return true;
    }

    cout << ">> MySQL connection failed. Falling back to local files.\n";
    return false;
}

void IncidentManager::createCase() {
    int typeChoice;
    string id, sev, desc, extra;

    cout << "\nSelect Incident Type (1-Phishing, 2-Malware): ";
    cin >> typeChoice;
    cin.ignore();

    cout << "Enter Case ID (e.g. CYB1001): ";
    getline(cin, id);
    cout << "Enter Severity (Low/Medium/High): ";
    getline(cin, sev);
    cout << "Enter Description: ";
    getline(cin, desc);

    if (typeChoice == 1) {
        cout << "Enter Target URL/Email: ";
        getline(cin, extra);
        cases.push_back(new PhishingIncident(id, sev, "Under Investigation", desc, extra));
    } else if (typeChoice == 2) {
        cout << "Enter Malware Hash/Signature: ";
        getline(cin, extra);
        cases.push_back(new MalwareIncident(id, sev, "Under Investigation", desc, extra));
    } else {
        cout << "Invalid Choice!\n";
        return;
    }
    cout << ">> Case Created Successfully!\n";
}

void IncidentManager::addEvidence() {
    string eID, cID, type, log;
    cin.ignore();
    cout << "Enter Evidence ID: "; getline(cin, eID);
    cout << "Enter Case ID: "; getline(cin, cID);
    cout << "Enter Evidence Type (e.g. Screenshot/Log): "; getline(cin, type);
    cout << "Enter Initial Custody Log: "; getline(cin, log);

    evidenceList.push_back(Evidence(eID, cID, type, log));
    cout << ">> Evidence Added Successfully!\n";
}

void IncidentManager::searchCase(string caseID) const {
    bool found = false;
    for (const auto& c : cases) {
        if (c->getCaseID() == caseID) {
            c->displayDetails();
            found = true;
            break;
        }
    }
    if (!found) cout << "Error: Case ID " << caseID << " not found!\n";
}

void IncidentManager::generateReport(string caseID) const {
    cout << "\n================ CASE REPORT ================";
    searchCase(caseID);
    cout << "\n--- Linked Evidence Items ---";
    bool hasEvidence = false;
    for (const auto& e : evidenceList) {
        if (e.getCaseID() == caseID) {
            e.displayDetails();
            hasEvidence = true;
        }
    }
    if (!hasEvidence) cout << "No evidence registered for this case.\n";
    cout << "=============================================\n";
}

void IncidentManager::displayAllCases() const {
    if (cases.empty()) {
        cout << "No cases registered.\n";
        return;
    }
    for (const auto& c : cases) {
        c->displayDetails();
    }
}

void IncidentManager::saveToFile() {
    ofstream fout("data/cases.txt");
    for (const auto& c : cases) {
        fout << c->serialize() << "\n";
    }
    fout.close();

    ofstream foutE("data/evidence.txt");
    for (const auto& e : evidenceList) {
        foutE << e.serialize() << "\n";
    }
    foutE.close();
    cout << ">> Data Saved to Files Successfully!\n";
}

void IncidentManager::loadFromFile() {
    ifstream fin("data/cases.txt");
    if (!fin) return;

    string line;
    while (getline(fin, line)) {
        stringstream ss(line);
        string type, id, sev, stat, desc, extra;
        getline(ss, type, '|');
        getline(ss, id, '|');
        getline(ss, sev, '|');
        getline(ss, stat, '|');
        getline(ss, desc, '|');
        getline(ss, extra, '|');

        if (type == "Phishing") {
            cases.push_back(new PhishingIncident(id, sev, stat, desc, extra));
        } else if (type == "Malware") {
            cases.push_back(new MalwareIncident(id, sev, stat, desc, extra));
        }
    }
    fin.close();
}

void IncidentManager::loadFromDatabase() {
    if (!db.isConnected()) {
        cout << "No active MySQL connection.\n";
        return;
    }

    auto rows = db.query("SELECT case_id, type, severity, status, description, extra_data FROM incidents");
    for (const auto& row : rows) {
        if (row.size() >= 6) {
            const string id = row[0];
            const string type = row[1];
            const string sev = row[2];
            const string stat = row[3];
            const string desc = row[4];
            const string extra = row[5];

            if (type == "Phishing") {
                cases.push_back(new PhishingIncident(id, sev, stat, desc, extra));
            } else if (type == "Malware") {
                cases.push_back(new MalwareIncident(id, sev, stat, desc, extra));
            }
        }
    }
}