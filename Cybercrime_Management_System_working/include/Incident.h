#ifndef INCIDENT_H
#define INCIDENT_H

#include <iostream>
#include <string>

using namespace std;

// Abstract Base Class
class Incident {
protected:
    string caseID;
    string severity;
    string status;
    string description;

public:
    Incident(string id, string sev, string stat, string desc);
    virtual ~Incident() {}

    string getCaseID() const { return caseID; }
    string getSeverity() const { return severity; }
    string getStatus() const { return status; }
    string getDescription() const { return description; }

    void setSeverity(string sev) { severity = sev; }
    void updateStatus(string newStatus) { status = newStatus; }

    // Pure Virtual Functions (Abstraction & Polymorphism)
    virtual void displayDetails() const = 0;
    virtual string getType() const = 0;
    virtual string serialize() const;
};

// Derived Classes
class PhishingIncident : public Incident {
private:
    string targetURL;
public:
    PhishingIncident(string id, string sev, string stat, string desc, string url);
    void displayDetails() const override;
    string getType() const override { return "Phishing"; }
    string serialize() const override;
};

class MalwareIncident : public Incident {
private:
    string malwareHash;
public:
    MalwareIncident(string id, string sev, string stat, string desc, string hash);
    void displayDetails() const override;
    string getType() const override { return "Malware"; }
    string serialize() const override;
};

#endif