//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Exercise #1 - Charge Account Validation
// This program validates charge account numbers using linear search

#include <iostream>
#include <vector>
using namespace std;

// Function prototype
bool linearSearch(const vector<int>&, int);

int main() {
    // Initialize array with valid account numbers
    vector<int> validAccounts = {
        5658845, 4520125, 7895122, 8777541, 8451277, 1302850,
        8080152, 4562555, 5552012, 5050552, 7825877, 1250255,
        1005231, 6545231, 3852085, 7576651, 7881200, 4581002
    };
    
    int accountNumber;
    
    cout << "Charge Account Validation Program\n";
    cout << "Enter 1111111 to exit\n\n";
    
    do {
        cout << "Enter charge account number: ";
        cin >> accountNumber;
        
        if (accountNumber == 1111111) {
            cout << "Exiting program...\n";
            break;
        }
        
        if (linearSearch(validAccounts, accountNumber)) {
            cout << "The account number " << accountNumber << " is VALID.\n\n";
        } else {
            cout << "The account number " << accountNumber << " is INVALID.\n\n";
        }
        
    } while (accountNumber != 1111111);
    
    return 0;
}

// Linear search function
// Returns true if value is found in the array, false otherwise
bool linearSearch(const vector<int>& array, int value) {
    for (int i = 0; i < array.size(); i++) {
        if (array[i] == value) {
            return true;
        }
    }
    return false;
}
