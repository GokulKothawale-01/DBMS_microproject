#include "IncidentManager.h"
#include <iostream>

using namespace std;

int main() {
    IncidentManager manager;

    cout << "\n===== CYBERCRIME CASE MANAGEMENT SYSTEM =====\n";
    cout << "Connect to MySQL? (1=Yes, 0=No): ";
    int useDb = 0;
    cin >> useDb;

    if (useDb == 1) {
        manager.connectToDatabase("localhost", "root", "", "cybercrime_db");
        manager.loadFromDatabase();
    } else {
        manager.loadFromFile();
    }

    int choice;
    do {
        cout << "\n===== MAIN MENU =====\n"
             << "1. Create New Case\n"
             << "2. Add Evidence\n"
             << "3. View All Cases\n"
             << "4. Search Case\n"
             << "5. Generate Case Report\n"
             << "6. Save & Exit\n"
             << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                manager.createCase();
                break;
            case 2:
                manager.addEvidence();
                break;
            case 3:
                manager.displayAllCases();
                break;
            case 4: {
                string id;
                cout << "Enter Case ID to Search: ";
                cin >> id;
                manager.searchCase(id);
                break;
            }
            case 5: {
                string id;
                cout << "Enter Case ID for Report: ";
                cin >> id;
                manager.generateReport(id);
                break;
            }
            case 6:
                manager.saveToFile();
                cout << "Exiting system. Goodbye!\n";
                break;
            default:
                cout << "Invalid Option! Try again.\n";
        }
    } while (choice != 6);

    return 0;
}