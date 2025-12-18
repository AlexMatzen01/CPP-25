//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Exercise #6 - String Selection Sort
// This program sorts an array of strings alphabetically using selection sort

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// Function prototypes
void selectionSort(vector<string>&);
void displayMenu();
vector<string> readNamesFromFile(const string&);
vector<string> getDefaultNames();
void displayNames(const vector<string>&);
void writeNamesToFile(const vector<string>&, const string&);

int main() {
    vector<string> names;
    int choice;
    
    displayMenu();
    cout << "Enter your choice: ";
    cin >> choice;
    cin.ignore(); // Clear the newline character
    
    if (choice == 1) {
        names = readNamesFromFile("data/names.txt");
        if (names.empty()) {
            cout << "Error reading file or file is empty. Using default names.\n";
            names = getDefaultNames();
        } else {
            cout << "Names read from file successfully.\n";
        }
    } else if (choice == 2) {
        names = getDefaultNames();
        cout << "Using default names.\n";
    } else {
        cout << "Invalid choice. Using default names.\n";
        names = getDefaultNames();
    }
    
    cout << "\nOriginal names:\n";
    displayNames(names);
    
    // Sort the names
    selectionSort(names);
    
    cout << "\nSorted names (alphabetically):\n";
    displayNames(names);
    
    // Write sorted names to file
    writeNamesToFile(names, "data/sortednames.txt");
    cout << "\nSorted names have been written to data/sortednames.txt\n";
    
    return 0;
}

// Display menu function
void displayMenu() {
    cout << "String Selection Sort Program\n";
    cout << "==============================\n";
    cout << "1. Read names from file (names.txt)\n";
    cout << "2. Use default names\n";
}

// Selection sort function for strings
void selectionSort(vector<string>& array) {
    int minIndex;
    string minValue;
    
    for (int start = 0; start < array.size() - 1; start++) {
        minIndex = start;
        minValue = array[start];
        
        for (int index = start + 1; index < array.size(); index++) {
            if (array[index] < minValue) {
                minValue = array[index];
                minIndex = index;
            }
        }
        
        // Swap the minimum value with the value at start
        array[minIndex] = array[start];
        array[start] = minValue;
    }
}

// Read names from file
vector<string> readNamesFromFile(const string& filename) {
    vector<string> names;
    ifstream inputFile(filename);
    string name;
    
    if (inputFile) {
        while (getline(inputFile, name)) {
            if (!name.empty()) {
                names.push_back(name);
            }
        }
        inputFile.close();
    }
    
    return names;
}

// Get default names
vector<string> getDefaultNames() {
    vector<string> names = {
        "Venkman, Peter", "Took, Peregrin", "Pinkman, Jesse", "Simpson, Bart",
        "Allen, Tom", "McFly, Marty", "Solo, Han", "Baggins, Bilbo",
        "Griffin, Peter", "White, Walter", "Organa, Leia", "Brown, Emmet",
        "Fudd, Elmer", "Uzumaki, Naruto", "Skywalker, Luke", "Jones, Indiana",
        "Callahan, Harold", "Gamgee, Samwise", "Fring, Gustavo", "Kenobi, Ben"
    };
    return names;
}

// Display names
void displayNames(const vector<string>& names) {
    for (int i = 0; i < names.size(); i++) {
        cout << names[i] << endl;
    }
}

// Write names to file
void writeNamesToFile(const vector<string>& names, const string& filename) {
    ofstream outputFile(filename);
    
    if (outputFile) {
        for (int i = 0; i < names.size(); i++) {
            outputFile << names[i] << endl;
        }
        outputFile.close();
    } else {
        cout << "Error: Could not open file for writing.\n";
    }
}
