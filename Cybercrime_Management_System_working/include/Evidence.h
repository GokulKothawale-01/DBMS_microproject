#ifndef EVIDENCE_H
#define EVIDENCE_H

#include <string>
#include <iostream>

using namespace std;

class Evidence {
private:
    string evidenceID;
    string caseID;
    string type;
    string custodyLog;

public:
    Evidence(string eID, string cID, string t, string log);
    
    string getEvidenceID() const { return evidenceID; }
    string getCaseID() const { return caseID; }
    string getType() const { return type; }
    string getCustodyLog() const { return custodyLog; }

    void updateCustody(string newLog);
    void displayDetails() const;
    string serialize() const;
};

#endif