//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Student Grades Management System
// A program for managing student grades with sorting, searching, and analysis

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
using namespace std;

// Constants
const int MAX_STUDENTS = 100;
const int MAX_GRADES = 10;

// Global arrays to store student data
string studentNames[MAX_STUDENTS];
double studentGrades[MAX_STUDENTS][MAX_GRADES];
int numGradesPerStudent[MAX_STUDENTS];
double studentAverages[MAX_STUDENTS];
int studentCount = 0;

// Function prototypes
void displayMenu();
void inputGrades();
void viewGrades();
void saveGradesToFile();
void loadGradesFromFile();
void analyzeGrades();
void searchStudent();
void sortData();
void calculateAverages();
double calculateAverage(int studentIndex);
void displayStudentData(int index);
void quickSortByName(int left, int right);
int partitionByName(int left, int right);
void mergeSortByAverage(int left, int right);
void merge(int left, int mid, int right);
int binarySearchByName(string name);
void clearInputStream();

int main() {
    int choice;
    
    cout << "Student Grades Management System\n";
    cout << "=================================\n\n";
    
    do {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;
        clearInputStream();
        
        switch (choice) {
            case 1:
                inputGrades();
                break;
            case 2:
                viewGrades();
                break;
            case 3:
                saveGradesToFile();
                break;
            case 4:
                loadGradesFromFile();
                break;
            case 5:
                analyzeGrades();
                break;
            case 6:
                searchStudent();
                break;
            case 7:
                sortData();
                break;
            case 8:
                cout << "\nExiting program. Goodbye!\n";
                break;
            default:
                cout << "\nInvalid choice. Please try again.\n";
        }
        
        if (choice != 8) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        
    } while (choice != 8);
    
    return 0;
}

// Display the main menu
void displayMenu() {
    cout << "\n";
    cout << "1. Input grades\n";
    cout << "2. View grades\n";
    cout << "3. Save Grades to File\n";
    cout << "4. Load grades from File\n";
    cout << "5. Analyze Grades\n";
    cout << "6. Search for a student by Name\n";
    cout << "7. Sort Data\n";
    cout << "8. Exit\n";
}

// Input student grades
void inputGrades() {
    if (studentCount >= MAX_STUDENTS) {
        cout << "\nMaximum number of students reached!\n";
        return;
    }
    
    cout << "\n--- Input Grades ---\n";
    cout << "Enter student name: ";
    getline(cin, studentNames[studentCount]);
    
    int numGrades;
    cout << "How many grades for this student? (1-" << MAX_GRADES << "): ";
    cin >> numGrades;
    clearInputStream();
    
    if (numGrades < 1 || numGrades > MAX_GRADES) {
        cout << "Invalid number of grades. Setting to 1.\n";
        numGrades = 1;
    }
    
    numGradesPerStudent[studentCount] = numGrades;
    
    for (int i = 0; i < numGrades; i++) {
        do {
            cout << "Enter grade " << (i + 1) << " (0-100): ";
            cin >> studentGrades[studentCount][i];
            clearInputStream();
            
            if (studentGrades[studentCount][i] < 0 || studentGrades[studentCount][i] > 100) {
                cout << "Invalid grade. Please enter a value between 0 and 100.\n";
            }
        } while (studentGrades[studentCount][i] < 0 || studentGrades[studentCount][i] > 100);
    }
    
    studentAverages[studentCount] = calculateAverage(studentCount);
    studentCount++;
    
    cout << "\nStudent added successfully!\n";
}

// View all grades
void viewGrades() {
    if (studentCount == 0) {
        cout << "\nNo student data available.\n";
        return;
    }
    
    cout << "\n--- Student Grades ---\n";
    cout << left << setw(20) << "Student" << setw(40) << "Grades" << setw(15) << "Average" << endl;
    cout << string(75, '-') << endl;
    
    for (int i = 0; i < studentCount; i++) {
        cout << left << setw(20) << studentNames[i];
        
        for (int j = 0; j < numGradesPerStudent[i]; j++) {
            cout << fixed << setprecision(1) << studentGrades[i][j];
            if (j < numGradesPerStudent[i] - 1) {
                cout << " ";
            }
        }
        
        cout << setw(40 - numGradesPerStudent[i] * 5) << " ";
        cout << fixed << setprecision(1) << studentAverages[i] << endl;
    }
}

// Save grades to file
void saveGradesToFile() {
    if (studentCount == 0) {
        cout << "\nNo data to save.\n";
        return;
    }
    
    string filename;
    cout << "\nEnter filename to save (e.g., grades.txt): ";
    getline(cin, filename);
    
    ofstream outFile("data/" + filename);
    
    if (!outFile) {
        cout << "Error: Could not open file for writing.\n";
        return;
    }
    
    outFile << studentCount << endl;
    
    for (int i = 0; i < studentCount; i++) {
        outFile << studentNames[i] << endl;
        outFile << numGradesPerStudent[i] << endl;
        
        for (int j = 0; j < numGradesPerStudent[i]; j++) {
            outFile << studentGrades[i][j];
            if (j < numGradesPerStudent[i] - 1) {
                outFile << " ";
            }
        }
        outFile << endl;
        outFile << studentAverages[i] << endl;
    }
    
    outFile.close();
    cout << "\nData saved successfully to data/" << filename << "!\n";
}

// Load grades from file
void loadGradesFromFile() {
    string filename;
    cout << "\nEnter filename to load (e.g., grades.txt): ";
    getline(cin, filename);
    
    ifstream inFile("data/" + filename);
    
    if (!inFile) {
        cout << "Error: File not found or could not be opened.\n";
        return;
    }
    
    int count;
    inFile >> count;
    inFile.ignore();
    
    if (count > MAX_STUDENTS) {
        cout << "Warning: File contains more students than capacity. Loading first " << MAX_STUDENTS << " students.\n";
        count = MAX_STUDENTS;
    }
    
    studentCount = 0;
    
    for (int i = 0; i < count; i++) {
        getline(inFile, studentNames[i]);
        inFile >> numGradesPerStudent[i];
        
        for (int j = 0; j < numGradesPerStudent[i]; j++) {
            inFile >> studentGrades[i][j];
        }
        
        inFile >> studentAverages[i];
        inFile.ignore();
        
        studentCount++;
    }
    
    inFile.close();
    cout << "\nData loaded successfully! " << studentCount << " students loaded.\n";
}

// Analyze grades
void analyzeGrades() {
    if (studentCount == 0) {
        cout << "\nNo student data available.\n";
        return;
    }
    
    calculateAverages();
    
    cout << "\n--- Grade Analysis ---\n";
    cout << left << setw(30) << "Student" << setw(15) << "Average Grade" << endl;
    cout << string(45, '-') << endl;
    
    // Find highest and lowest averages
    int highestIndex = 0;
    int lowestIndex = 0;
    
    for (int i = 1; i < studentCount; i++) {
        if (studentAverages[i] > studentAverages[highestIndex]) {
            highestIndex = i;
        }
        if (studentAverages[i] < studentAverages[lowestIndex]) {
            lowestIndex = i;
        }
    }
    
    // Display all students with highlighting
    for (int i = 0; i < studentCount; i++) {
        if (i == highestIndex) {
            cout << "\033[1;32m"; // Green color for highest
        } else if (i == lowestIndex) {
            cout << "\033[1;31m"; // Red color for lowest
        }
        
        cout << left << setw(30) << studentNames[i];
        cout << fixed << setprecision(1) << studentAverages[i] << endl;
        
        if (i == highestIndex || i == lowestIndex) {
            cout << "\033[0m"; // Reset color
        }
    }
    
    cout << string(45, '-') << endl;
    cout << "Highest Average: " << studentNames[highestIndex] 
         << " (" << fixed << setprecision(1) << studentAverages[highestIndex] << ")\n";
    cout << "Lowest Average: " << studentNames[lowestIndex] 
         << " (" << fixed << setprecision(1) << studentAverages[lowestIndex] << ")\n";
}

// Search for a student by name
void searchStudent() {
    if (studentCount == 0) {
        cout << "\nNo student data available.\n";
        return;
    }
    
    // First, sort by name for binary search
    quickSortByName(0, studentCount - 1);
    
    string searchName;
    cout << "\nEnter the student's name to search: ";
    getline(cin, searchName);
    
    int index = binarySearchByName(searchName);
    
    if (index != -1) {
        cout << "\nStudent found: " << studentNames[index] << endl;
        cout << "Grades: ";
        for (int i = 0; i < numGradesPerStudent[index]; i++) {
            cout << fixed << setprecision(0) << studentGrades[index][i];
            if (i < numGradesPerStudent[index] - 1) {
                cout << " ";
            }
        }
        cout << endl;
        cout << "Average: " << fixed << setprecision(1) << studentAverages[index] << endl;
    } else {
        cout << "\nStudent not found.\n";
    }
}

// Sort data menu
void sortData() {
    if (studentCount == 0) {
        cout << "\nNo student data available.\n";
        return;
    }
    
    int sortChoice;
    cout << "\n--- Sort Data ---\n";
    cout << "1. Sort by Name (Alphabetical)\n";
    cout << "2. Sort by Average Grade (Descending)\n";
    cout << "Enter your choice: ";
    cin >> sortChoice;
    clearInputStream();
    
    if (sortChoice == 1) {
        quickSortByName(0, studentCount - 1);
        cout << "\n--- Alphabetical Sort ---\n";
        cout << left << setw(30) << "Student" << setw(15) << "Average Grade" << endl;
        cout << string(45, '-') << endl;
        
        for (int i = 0; i < studentCount; i++) {
            cout << left << setw(30) << studentNames[i];
            cout << fixed << setprecision(1) << studentAverages[i] << endl;
        }
    } else if (sortChoice == 2) {
        mergeSortByAverage(0, studentCount - 1);
        cout << "\n--- Average Sort ---\n";
        cout << left << setw(30) << "Student" << setw(15) << "Average Grade" << endl;
        cout << string(45, '-') << endl;
        
        for (int i = 0; i < studentCount; i++) {
            cout << left << setw(30) << studentNames[i];
            cout << fixed << setprecision(1) << studentAverages[i] << endl;
        }
    } else {
        cout << "\nInvalid choice.\n";
    }
}

// Calculate averages for all students
void calculateAverages() {
    for (int i = 0; i < studentCount; i++) {
        studentAverages[i] = calculateAverage(i);
    }
}

// Calculate average for a single student
double calculateAverage(int studentIndex) {
    if (numGradesPerStudent[studentIndex] == 0) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (int i = 0; i < numGradesPerStudent[studentIndex]; i++) {
        sum += studentGrades[studentIndex][i];
    }
    
    return sum / numGradesPerStudent[studentIndex];
}

// Quick sort by name (alphabetical)
void quickSortByName(int left, int right) {
    if (left < right) {
        int pivotIndex = partitionByName(left, right);
        quickSortByName(left, pivotIndex - 1);
        quickSortByName(pivotIndex + 1, right);
    }
}

// Partition function for quick sort
int partitionByName(int left, int right) {
    string pivot = studentNames[right];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
        if (studentNames[j] < pivot) {
            i++;
            
            // Swap all data for these two students
            swap(studentNames[i], studentNames[j]);
            swap(numGradesPerStudent[i], numGradesPerStudent[j]);
            swap(studentAverages[i], studentAverages[j]);
            
            for (int k = 0; k < MAX_GRADES; k++) {
                swap(studentGrades[i][k], studentGrades[j][k]);
            }
        }
    }
    
    // Swap pivot into correct position
    swap(studentNames[i + 1], studentNames[right]);
    swap(numGradesPerStudent[i + 1], numGradesPerStudent[right]);
    swap(studentAverages[i + 1], studentAverages[right]);
    
    for (int k = 0; k < MAX_GRADES; k++) {
        swap(studentGrades[i + 1][k], studentGrades[right][k]);
    }
    
    return i + 1;
}

// Merge sort by average (descending order)
void mergeSortByAverage(int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        mergeSortByAverage(left, mid);
        mergeSortByAverage(mid + 1, right);
        merge(left, mid, right);
    }
}

// Merge function for merge sort
void merge(int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    // Temporary arrays
    string* tempNames1 = new string[n1];
    string* tempNames2 = new string[n2];
    double* tempAverages1 = new double[n1];
    double* tempAverages2 = new double[n2];
    int* tempNumGrades1 = new int[n1];
    int* tempNumGrades2 = new int[n2];
    double** tempGrades1 = new double*[n1];
    double** tempGrades2 = new double*[n2];
    
    for (int i = 0; i < n1; i++) {
        tempGrades1[i] = new double[MAX_GRADES];
    }
    for (int i = 0; i < n2; i++) {
        tempGrades2[i] = new double[MAX_GRADES];
    }
    
    // Copy data to temporary arrays
    for (int i = 0; i < n1; i++) {
        tempNames1[i] = studentNames[left + i];
        tempAverages1[i] = studentAverages[left + i];
        tempNumGrades1[i] = numGradesPerStudent[left + i];
        for (int j = 0; j < MAX_GRADES; j++) {
            tempGrades1[i][j] = studentGrades[left + i][j];
        }
    }
    
    for (int i = 0; i < n2; i++) {
        tempNames2[i] = studentNames[mid + 1 + i];
        tempAverages2[i] = studentAverages[mid + 1 + i];
        tempNumGrades2[i] = numGradesPerStudent[mid + 1 + i];
        for (int j = 0; j < MAX_GRADES; j++) {
            tempGrades2[i][j] = studentGrades[mid + 1 + i][j];
        }
    }
    
    // Merge in descending order
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        if (tempAverages1[i] >= tempAverages2[j]) {
            studentNames[k] = tempNames1[i];
            studentAverages[k] = tempAverages1[i];
            numGradesPerStudent[k] = tempNumGrades1[i];
            for (int g = 0; g < MAX_GRADES; g++) {
                studentGrades[k][g] = tempGrades1[i][g];
            }
            i++;
        } else {
            studentNames[k] = tempNames2[j];
            studentAverages[k] = tempAverages2[j];
            numGradesPerStudent[k] = tempNumGrades2[j];
            for (int g = 0; g < MAX_GRADES; g++) {
                studentGrades[k][g] = tempGrades2[j][g];
            }
            j++;
        }
        k++;
    }
    
    // Copy remaining elements
    while (i < n1) {
        studentNames[k] = tempNames1[i];
        studentAverages[k] = tempAverages1[i];
        numGradesPerStudent[k] = tempNumGrades1[i];
        for (int g = 0; g < MAX_GRADES; g++) {
            studentGrades[k][g] = tempGrades1[i][g];
        }
        i++;
        k++;
    }
    
    while (j < n2) {
        studentNames[k] = tempNames2[j];
        studentAverages[k] = tempAverages2[j];
        numGradesPerStudent[k] = tempNumGrades2[j];
        for (int g = 0; g < MAX_GRADES; g++) {
            studentGrades[k][g] = tempGrades2[j][g];
        }
        j++;
        k++;
    }
    
    // Clean up temporary arrays
    for (int i = 0; i < n1; i++) {
        delete[] tempGrades1[i];
    }
    for (int i = 0; i < n2; i++) {
        delete[] tempGrades2[i];
    }
    delete[] tempNames1;
    delete[] tempNames2;
    delete[] tempAverages1;
    delete[] tempAverages2;
    delete[] tempNumGrades1;
    delete[] tempNumGrades2;
    delete[] tempGrades1;
    delete[] tempGrades2;
}

// Binary search by name
int binarySearchByName(string name) {
    int left = 0;
    int right = studentCount - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (studentNames[mid] == name) {
            return mid;
        } else if (studentNames[mid] < name) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return -1; // Not found
}

// Clear input stream
void clearInputStream() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
