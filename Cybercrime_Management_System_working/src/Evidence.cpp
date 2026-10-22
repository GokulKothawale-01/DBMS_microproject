#include "Evidence.h"

Evidence::Evidence(string eID, string cID, string t, string log)
    : evidenceID(eID), caseID(cID), type(t), custodyLog(log) {}

void Evidence::updateCustody(string newLog) {
    custodyLog += " -> " + newLog;
}

void Evidence::displayDetails() const {
    cout << "Evidence ID: " << evidenceID << " | Case ID: " << caseID 
         << " | Type: " << type << "\nCustody Log: " << custodyLog << "\n";
}

string Evidence::serialize() const {
    return evidenceID + "|" + caseID + "|" + type + "|" + custodyLog;
}