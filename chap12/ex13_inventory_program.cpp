/*
#InventoryProgram accepts no arguments
#it stores, displays, and updates inventory records in a binary file
#it outputs record details to the console and persists changes to disk
*/

#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>

using namespace std;

namespace {
const char INVENTORY_FILE[] = "inventory.dat";
const int DESCRIPTION_SIZE = 50;

struct Date {
    int month;
    int day;
    int year;
};

struct InventoryRecord {
    char description[DESCRIPTION_SIZE + 1];
    int quantityOnHand;
    double wholesaleCost;
    double retailCost;
    Date dateAdded;
};

void clearInputState() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool isLeapYear(int year) {
    if (year % 400 == 0) {
        return true;
    }
    if (year % 100 == 0) {
        return false;
    }
    return year % 4 == 0;
}

int daysInMonth(int month, int year) {
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            return isLeapYear(year) ? 29 : 28;
        default:
            return 0;
    }
}

bool isReasonableDate(const Date& dateValue) {
    if (dateValue.year < 1900 || dateValue.year > 2100) {
        return false;
    }

    if (dateValue.month < 1 || dateValue.month > 12) {
        return false;
    }

    const int maxDay = daysInMonth(dateValue.month, dateValue.year);
    return dateValue.day >= 1 && dateValue.day <= maxDay;
}

int readInt(const string& prompt, int minValue) {
    int value = 0;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minValue) {
            clearInputState();
            return value;
        }

        cout << "Invalid input. Enter an integer >= " << minValue << ".\n";
        clearInputState();
    }
}

double readDouble(const string& prompt, double minValue) {
    double value = 0.0;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minValue) {
            clearInputState();
            return value;
        }

        cout << "Invalid input. Enter a number >= " << minValue << ".\n";
        clearInputState();
    }
}

Date readDate() {
    Date dateValue{};

    while (true) {
        dateValue.month = readInt("Enter month (1-12): ", 1);
        dateValue.day = readInt("Enter day: ", 1);
        dateValue.year = readInt("Enter year (1900-2100): ", 1900);

        if (isReasonableDate(dateValue)) {
            return dateValue;
        }

        cout << "Invalid date entered. Please try again.\n";
    }
}

void fillRecordFromUser(InventoryRecord& record) {
    string descriptionInput;

    cout << "Enter item description (up to " << DESCRIPTION_SIZE << " characters): ";
    getline(cin, descriptionInput);
    if (descriptionInput.size() > static_cast<size_t>(DESCRIPTION_SIZE)) {
        descriptionInput.resize(DESCRIPTION_SIZE);
    }

    memset(record.description, 0, sizeof(record.description));
    strncpy(record.description, descriptionInput.c_str(), DESCRIPTION_SIZE);

    record.quantityOnHand = readInt("Enter quantity on hand (>= 0): ", 0);
    record.wholesaleCost = readDouble("Enter wholesale cost (>= 0): ", 0.0);
    record.retailCost = readDouble("Enter retail cost (>= 0): ", 0.0);
    record.dateAdded = readDate();
}

void printRecord(const InventoryRecord& record, size_t recordNumber) {
    cout << "\nRecord #" << recordNumber << "\n";
    cout << "Description      : " << record.description << "\n";
    cout << "Quantity on Hand : " << record.quantityOnHand << "\n";
    cout << "Wholesale Cost   : $" << record.wholesaleCost << "\n";
    cout << "Retail Cost      : $" << record.retailCost << "\n";
    cout << "Date Added       : "
         << record.dateAdded.month << "/"
         << record.dateAdded.day << "/"
         << record.dateAdded.year << "\n";
}

size_t recordCountInFile() {
    ifstream inFile(INVENTORY_FILE, ios::binary);
    if (!inFile) {
        return 0;
    }

    inFile.seekg(0, ios::end);
    const streamoff bytes = inFile.tellg();
    if (bytes < 0) {
        return 0;
    }

    return static_cast<size_t>(bytes / static_cast<streamoff>(sizeof(InventoryRecord)));
}

void addRecord() {
    InventoryRecord newRecord{};
    fillRecordFromUser(newRecord);

    ofstream outFile(INVENTORY_FILE, ios::binary | ios::app);
    if (!outFile) {
        cout << "Unable to open inventory file for writing.\n";
        return;
    }

    outFile.write(reinterpret_cast<const char*>(&newRecord), sizeof(InventoryRecord));
    if (!outFile) {
        cout << "Failed to write record.\n";
        return;
    }

    cout << "Record added successfully.\n";
}

void displayRecord() {
    const size_t totalRecords = recordCountInFile();
    if (totalRecords == 0) {
        cout << "No records found.\n";
        return;
    }

    cout << "There are " << totalRecords << " record(s).\n";
    const int recordNumber = readInt("Enter record number to display: ", 1);
    if (static_cast<size_t>(recordNumber) > totalRecords) {
        cout << "Record number out of range.\n";
        return;
    }

    ifstream inFile(INVENTORY_FILE, ios::binary);
    if (!inFile) {
        cout << "Unable to open inventory file for reading.\n";
        return;
    }

    InventoryRecord record{};
    const streamoff offset = static_cast<streamoff>(recordNumber - 1) * static_cast<streamoff>(sizeof(InventoryRecord));
    inFile.seekg(offset, ios::beg);
    inFile.read(reinterpret_cast<char*>(&record), sizeof(InventoryRecord));

    if (!inFile) {
        cout << "Failed to read record.\n";
        return;
    }

    printRecord(record, static_cast<size_t>(recordNumber));
}

void changeRecord() {
    const size_t totalRecords = recordCountInFile();
    if (totalRecords == 0) {
        cout << "No records found.\n";
        return;
    }

    cout << "There are " << totalRecords << " record(s).\n";
    const int recordNumber = readInt("Enter record number to change: ", 1);
    if (static_cast<size_t>(recordNumber) > totalRecords) {
        cout << "Record number out of range.\n";
        return;
    }

    fstream ioFile(INVENTORY_FILE, ios::binary | ios::in | ios::out);
    if (!ioFile) {
        cout << "Unable to open inventory file for update.\n";
        return;
    }

    InventoryRecord updatedRecord{};
    fillRecordFromUser(updatedRecord);

    const streamoff offset = static_cast<streamoff>(recordNumber - 1) * static_cast<streamoff>(sizeof(InventoryRecord));
    ioFile.seekp(offset, ios::beg);
    ioFile.write(reinterpret_cast<const char*>(&updatedRecord), sizeof(InventoryRecord));

    if (!ioFile) {
        cout << "Failed to update record.\n";
        return;
    }

    cout << "Record updated successfully.\n";
}

int readMenuChoice() {
    while (true) {
        cout << "\nInventory Program\n";
        cout << "1. Add new record\n";
        cout << "2. Display a record\n";
        cout << "3. Change a record\n";
        cout << "4. Exit\n";
        cout << "Choose an option (1-4): ";

        int choice = 0;
        if (cin >> choice && choice >= 1 && choice <= 4) {
            clearInputState();
            return choice;
        }

        cout << "Invalid choice. Please enter a number from 1 to 4.\n";
        clearInputState();
    }
}
}  // namespace

int main() {
    while (true) {
        const int choice = readMenuChoice();

        if (choice == 1) {
            addRecord();
        } else if (choice == 2) {
            displayRecord();
        } else if (choice == 3) {
            changeRecord();
        } else {
            cout << "Program ended.\n";
            break;
        }
    }

    return 0;
}
