#include "Incident.h"

Incident::Incident(string id, string sev, string stat, string desc)
    : caseID(id), severity(sev), status(stat), description(desc) {}

string Incident::serialize() const {
    return caseID + "|" + severity + "|" + status + "|" + description;
}

PhishingIncident::PhishingIncident(string id, string sev, string stat, string desc, string url)
    : Incident(id, sev, stat, desc), targetURL(url) {}

void PhishingIncident::displayDetails() const {
    cout << "\n[Phishing Case Details]\n"
         << "Case ID     : " << caseID << "\n"
         << "Severity    : " << severity << "\n"
         << "Status      : " << status << "\n"
         << "Description : " << description << "\n"
         << "Target URL  : " << targetURL << "\n";
}

string PhishingIncident::serialize() const {
    return "Phishing|" + Incident::serialize() + "|" + targetURL;
}

MalwareIncident::MalwareIncident(string id, string sev, string stat, string desc, string hash)
    : Incident(id, sev, stat, desc), malwareHash(hash) {}

void MalwareIncident::displayDetails() const {
    cout << "\n[Malware Case Details]\n"
         << "Case ID     : " << caseID << "\n"
         << "Severity    : " << severity << "\n"
         << "Status      : " << status << "\n"
         << "Description : " << description << "\n"
         << "Malware Hash: " << malwareHash << "\n";
}

string MalwareIncident::serialize() const {
    return "Malware|" + Incident::serialize() + "|" + malwareHash;
}