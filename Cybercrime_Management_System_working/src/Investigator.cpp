#include "Investigator.h"

Investigator::Investigator(string i, string n, string c)
    : id(i), name(n), contact(c) {}

void Investigator::displayDetails() const {
    cout << "ID: " << id << " | Name: " << name << " | Contact: " << contact << "\n";
}

string Investigator::serialize() const {
    return id + "|" + name + "|" + contact;
}