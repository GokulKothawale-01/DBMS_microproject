#ifndef INVESTIGATOR_H
#define INVESTIGATOR_H

#include <string>
#include <iostream>

using namespace std;

class Investigator {
private:
    string id;
    string name;
    string contact;

public:
    Investigator(string i, string n, string c);

    string getID() const { return id; }
    string getName() const { return name; }
    string getContact() const { return contact; }

    void displayDetails() const;
    string serialize() const;
};

#endif