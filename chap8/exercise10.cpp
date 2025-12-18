//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Exercise #10 - Sorting Orders
// This program demonstrates bubble sort and selection sort with step-by-step output

#include <iostream>
using namespace std;

// Function prototypes
void bubbleSort(int[], int);
void selectionSort(int[], int);
void displayArray(const int[], int);

int main() {
    const int SIZE = 8;
    
    // Two identical arrays
    int array1[SIZE] = {64, 34, 25, 12, 22, 11, 90, 88};
    int array2[SIZE] = {64, 34, 25, 12, 22, 11, 90, 88};
    
    cout << "Sorting Orders Demonstration\n";
    cout << "============================\n\n";
    
    // Display and sort first array using bubble sort
    cout << "BUBBLE SORT\n";
    cout << "-----------\n";
    cout << "Original array: ";
    displayArray(array1, SIZE);
    cout << endl;
    
    bubbleSort(array1, SIZE);
    
    cout << "\nFinal sorted array: ";
    displayArray(array1, SIZE);
    cout << "\n\n";
    
    // Display and sort second array using selection sort
    cout << "SELECTION SORT\n";
    cout << "--------------\n";
    cout << "Original array: ";
    displayArray(array2, SIZE);
    cout << endl;
    
    selectionSort(array2, SIZE);
    
    cout << "\nFinal sorted array: ";
    displayArray(array2, SIZE);
    cout << endl;
    
    return 0;
}

// Bubble sort function - ascending order with step-by-step output
void bubbleSort(int array[], int size) {
    int temp;
    bool swapped;
    int pass = 1;
    
    for (int maxElement = size - 1; maxElement > 0; maxElement--) {
        swapped = false;
        
        for (int index = 0; index < maxElement; index++) {
            if (array[index] > array[index + 1]) {
                // Swap elements
                temp = array[index];
                array[index] = array[index + 1];
                array[index + 1] = temp;
                swapped = true;
            }
        }
        
        // Print array after each pass
        cout << "After pass " << pass << ": ";
        displayArray(array, size);
        cout << endl;
        pass++;
        
        // If no swaps were made, the array is sorted
        if (!swapped) {
            break;
        }
    }
}

// Selection sort function - ascending order with step-by-step output
void selectionSort(int array[], int size) {
    int minIndex;
    int minValue;
    int pass = 1;
    
    for (int start = 0; start < size - 1; start++) {
        minIndex = start;
        minValue = array[start];
        
        // Find the minimum value in the remaining array
        for (int index = start + 1; index < size; index++) {
            if (array[index] < minValue) {
                minValue = array[index];
                minIndex = index;
            }
        }
        
        // Swap the minimum value with the value at start
        array[minIndex] = array[start];
        array[start] = minValue;
        
        // Print array after each pass
        cout << "After pass " << pass << ": ";
        displayArray(array, size);
        cout << endl;
        pass++;
    }
}

// Display array function
void displayArray(const int array[], int size) {
    cout << "[";
    for (int i = 0; i < size; i++) {
        cout << array[i];
        if (i < size - 1) {
            cout << ", ";
        }
    }
    cout << "]";
}
