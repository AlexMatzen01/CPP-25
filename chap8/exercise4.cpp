//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Exercise #4 - Charge Account Modification
// This program validates charge account numbers using binary search
// The array is sorted using selection sort before searching

#include <iostream>
#include <vector>
using namespace std;

// Function prototypes
void selectionSort(vector<int>&);
bool binarySearch(const vector<int>&, int);

int main() {
    // Initialize array with valid account numbers
    vector<int> validAccounts = {
        5658845, 4520125, 7895122, 8777541, 8451277, 1302850,
        8080152, 4562555, 5552012, 5050552, 7825877, 1250255,
        1005231, 6545231, 3852085, 7576651, 7881200, 4581002
    };
    
    // Sort the array using selection sort
    selectionSort(validAccounts);
    
    int accountNumber;
    
    cout << "Charge Account Validation Program (Binary Search)\n";
    cout << "Enter 1111111 to exit\n\n";
    
    do {
        cout << "Enter charge account number: ";
        cin >> accountNumber;
        
        if (accountNumber == 1111111) {
            cout << "Exiting program...\n";
            break;
        }
        
        if (binarySearch(validAccounts, accountNumber)) {
            cout << "The account number " << accountNumber << " is VALID.\n\n";
        } else {
            cout << "The account number " << accountNumber << " is INVALID.\n\n";
        }
        
    } while (accountNumber != 1111111);
    
    return 0;
}

// Selection sort function - sorts array in ascending order
void selectionSort(vector<int>& array) {
    int minIndex;
    int minValue;
    
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

// Binary search function
// Returns true if value is found in the sorted array, false otherwise
bool binarySearch(const vector<int>& array, int value) {
    int first = 0;
    int last = array.size() - 1;
    int middle;
    bool found = false;
    
    while (!found && first <= last) {
        middle = (first + last) / 2;
        
        if (array[middle] == value) {
            found = true;
        } else if (array[middle] > value) {
            last = middle - 1;
        } else {
            first = middle + 1;
        }
    }
    
    return found;
}
