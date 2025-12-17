/*
 * Student Grades Management System
 * A menu-driven program to manage student grades using arrays, functions,
 * file I/O, search, and sorting algorithms.
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <limits>

using namespace std;

// Constants
const int MAX_STUDENTS = 50;
const int MAX_ASSIGNMENTS = 5;
const string DATA_FILE = "student_data.txt";

// Global arrays for student data
string studentNames[MAX_STUDENTS];
double studentGrades[MAX_STUDENTS][MAX_ASSIGNMENTS];
double studentAverages[MAX_STUDENTS];
int numStudents = 0;
int numAssignments = 0;

// Function prototypes
void displayMenu();
void inputGrades();
void viewGrades();
void saveData();
void loadData();
void analyzeGrades();
void searchStudent();
void sortData();
void calculateAverages();
double getValidGrade();
int getValidInt(int min, int max);
void quickSortByName(int low, int high);
int partitionByName(int low, int high);
void mergeSortByAverage(int left, int right);
void merge(int left, int mid, int right);
int binarySearchStudent(const string& name);

int main() {
    int choice;
    bool running = true;

    cout << "=== Student Grades Management System ===" << endl << endl;

    while (running) {
        displayMenu();
        choice = getValidInt(0, 7);

        switch (choice) {
            case 1:
                inputGrades();
                break;
            case 2:
                viewGrades();
                break;
            case 3:
                saveData();
                break;
            case 4:
                loadData();
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
            case 0:
                cout << "\nExiting program. Goodbye!" << endl;
                running = false;
                break;
        }
    }

    return 0;
}

void displayMenu() {
    cout << "\n========== MAIN MENU ==========" << endl;
    cout << "1. Input grades" << endl;
    cout << "2. View grades" << endl;
    cout << "3. Save data" << endl;
    cout << "4. Load data" << endl;
    cout << "5. Analyze grades" << endl;
    cout << "6. Search student" << endl;
    cout << "7. Sort data" << endl;
    cout << "0. Exit" << endl;
    cout << "===============================" << endl;
    cout << "Enter your choice: ";
}

void inputGrades() {
    cout << "\n=== Input Grades ===" << endl;
    
    // Get number of students
    cout << "Enter number of students (1-" << MAX_STUDENTS << "): ";
    numStudents = getValidInt(1, MAX_STUDENTS);
    
    // Get number of assignments
    cout << "Enter number of assignments (1-" << MAX_ASSIGNMENTS << "): ";
    numAssignments = getValidInt(1, MAX_ASSIGNMENTS);
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    // Input data for each student
    for (int i = 0; i < numStudents; ++i) {
        cout << "\nStudent " << (i + 1) << ":" << endl;
        
        // Get student name
        cout << "  Name: ";
        while (true) {
            getline(cin, studentNames[i]);
            // Trim whitespace
            size_t start = studentNames[i].find_first_not_of(" \t\r\n");
            if (start == string::npos) {
                cout << "  Name cannot be empty. Enter name: ";
                continue;
            }
            size_t end = studentNames[i].find_last_not_of(" \t\r\n");
            studentNames[i] = studentNames[i].substr(start, end - start + 1);
            break;
        }
        
        // Get grades for each assignment
        for (int j = 0; j < numAssignments; ++j) {
            cout << "  Assignment " << (j + 1) << " grade: ";
            studentGrades[i][j] = getValidGrade();
        }
    }
    
    calculateAverages();
    cout << "\nGrades input complete!" << endl;
}

void viewGrades() {
    if (numStudents == 0) {
        cout << "\nNo student data available. Please input or load data first." << endl;
        return;
    }
    
    cout << "\n=== Student Grades ===" << endl;
    cout << fixed << setprecision(2);
    
    // Header
    cout << left << setw(20) << "Name";
    for (int i = 0; i < numAssignments; ++i) {
        cout << setw(10) << ("Assign" + to_string(i + 1));
    }
    cout << setw(10) << "Average" << endl;
    cout << string(20 + numAssignments * 10 + 10, '-') << endl;
    
    // Student data
    for (int i = 0; i < numStudents; ++i) {
        cout << left << setw(20) << studentNames[i];
        for (int j = 0; j < numAssignments; ++j) {
            cout << setw(10) << studentGrades[i][j];
        }
        cout << setw(10) << studentAverages[i] << endl;
    }
}

void saveData() {
    if (numStudents == 0) {
        cout << "\nNo student data to save." << endl;
        return;
    }
    
    ofstream outFile(DATA_FILE);
    
    if (!outFile) {
        cerr << "\nError: Unable to open file for writing." << endl;
        return;
    }
    
    // Save metadata
    outFile << numStudents << " " << numAssignments << endl;
    
    // Save student data
    for (int i = 0; i < numStudents; ++i) {
        outFile << studentNames[i] << endl;
        for (int j = 0; j < numAssignments; ++j) {
            outFile << studentGrades[i][j];
            if (j < numAssignments - 1) outFile << " ";
        }
        outFile << endl;
        outFile << studentAverages[i] << endl;
    }
    
    outFile.close();
    cout << "\nData saved successfully to " << DATA_FILE << endl;
}

void loadData() {
    ifstream inFile(DATA_FILE);
    
    if (!inFile) {
        cerr << "\nError: File not found. Please check if " << DATA_FILE << " exists." << endl;
        return;
    }
    
    // Load metadata
    inFile >> numStudents >> numAssignments;
    inFile.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (numStudents > MAX_STUDENTS || numAssignments > MAX_ASSIGNMENTS) {
        cerr << "\nError: File contains invalid data (too many students or assignments)." << endl;
        inFile.close();
        numStudents = 0;
        numAssignments = 0;
        return;
    }
    
    // Load student data
    for (int i = 0; i < numStudents; ++i) {
        getline(inFile, studentNames[i]);
        for (int j = 0; j < numAssignments; ++j) {
            inFile >> studentGrades[i][j];
        }
        inFile >> studentAverages[i];
        inFile.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    inFile.close();
    cout << "\nData loaded successfully from " << DATA_FILE << endl;
    cout << "Loaded " << numStudents << " students with " << numAssignments << " assignments each." << endl;
}

void analyzeGrades() {
    if (numStudents == 0) {
        cout << "\nNo student data available." << endl;
        return;
    }
    
    cout << "\n=== Grade Analysis ===" << endl;
    cout << fixed << setprecision(2);
    
    // Find highest and lowest averages
    int highestIdx = 0, lowestIdx = 0;
    for (int i = 1; i < numStudents; ++i) {
        if (studentAverages[i] > studentAverages[highestIdx]) {
            highestIdx = i;
        }
        if (studentAverages[i] < studentAverages[lowestIdx]) {
            lowestIdx = i;
        }
    }
    
    cout << "\nHighest Average: " << studentNames[highestIdx] 
         << " with " << studentAverages[highestIdx] << "%" << endl;
    cout << "Lowest Average:  " << studentNames[lowestIdx] 
         << " with " << studentAverages[lowestIdx] << "%" << endl;
    
    // Calculate class average
    double classTotal = 0.0;
    for (int i = 0; i < numStudents; ++i) {
        classTotal += studentAverages[i];
    }
    double classAverage = classTotal / numStudents;
    cout << "\nClass Average: " << classAverage << "%" << endl;
    
    // Display table with highlights
    cout << "\n=== Student Performance Table ===" << endl;
    cout << left << setw(20) << "Name" << setw(12) << "Average" << "Status" << endl;
    cout << string(40, '-') << endl;
    
    for (int i = 0; i < numStudents; ++i) {
        cout << left << setw(20) << studentNames[i] 
             << setw(12) << studentAverages[i];
        
        if (i == highestIdx) {
            cout << "** HIGHEST **";
        } else if (i == lowestIdx) {
            cout << "** LOWEST **";
        }
        cout << endl;
    }
}

void searchStudent() {
    if (numStudents == 0) {
        cout << "\nNo student data available." << endl;
        return;
    }
    
    cout << "\n=== Search Student ===" << endl;
    
    // First, sort by name for binary search
    quickSortByName(0, numStudents - 1);
    cout << "Data sorted alphabetically for search." << endl;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    string searchName;
    cout << "Enter student name to search: ";
    getline(cin, searchName);
    
    // Trim whitespace
    size_t start = searchName.find_first_not_of(" \t\r\n");
    if (start != string::npos) {
        size_t end = searchName.find_last_not_of(" \t\r\n");
        searchName = searchName.substr(start, end - start + 1);
    }
    
    int index = binarySearchStudent(searchName);
    
    if (index != -1) {
        cout << "\n*** Student Found ***" << endl;
        cout << "Name: " << studentNames[index] << endl;
        cout << fixed << setprecision(2);
        cout << "\nGrades:" << endl;
        for (int i = 0; i < numAssignments; ++i) {
            cout << "  Assignment " << (i + 1) << ": " << studentGrades[index][i] << endl;
        }
        cout << "\nAverage: " << studentAverages[index] << "%" << endl;
    } else {
        cout << "\nStudent not found." << endl;
    }
}

void sortData() {
    if (numStudents == 0) {
        cout << "\nNo student data available." << endl;
        return;
    }
    
    cout << "\n=== Sort Data ===" << endl;
    cout << "1. Sort by name (alphabetically)" << endl;
    cout << "2. Sort by average (descending)" << endl;
    cout << "Enter choice: ";
    
    int choice = getValidInt(1, 2);
    
    if (choice == 1) {
        quickSortByName(0, numStudents - 1);
        cout << "\nData sorted alphabetically by name." << endl;
    } else {
        mergeSortByAverage(0, numStudents - 1);
        cout << "\nData sorted by average (highest to lowest)." << endl;
    }
    
    viewGrades();
}

void calculateAverages() {
    for (int i = 0; i < numStudents; ++i) {
        double sum = 0.0;
        for (int j = 0; j < numAssignments; ++j) {
            sum += studentGrades[i][j];
        }
        studentAverages[i] = sum / numAssignments;
    }
}

double getValidGrade() {
    double grade;
    while (true) {
        cin >> grade;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "    Invalid input. Enter a valid grade (0-100): ";
        } else if (grade < 0 || grade > 100) {
            cout << "    Grade must be between 0 and 100. Try again: ";
        } else {
            return grade;
        }
    }
}

int getValidInt(int min, int max) {
    int value;
    while (true) {
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "    Invalid input. Enter a number (" << min << "-" << max << "): ";
        } else if (value < min || value > max) {
            cout << "    Value must be between " << min << " and " << max << ". Try again: ";
        } else {
            return value;
        }
    }
}

// Quick Sort implementation for sorting by name
void quickSortByName(int low, int high) {
    if (low < high) {
        int pi = partitionByName(low, high);
        quickSortByName(low, pi - 1);
        quickSortByName(pi + 1, high);
    }
}

int partitionByName(int low, int high) {
    string pivot = studentNames[high];
    int i = low - 1;
    
    for (int j = low; j < high; ++j) {
        if (studentNames[j] < pivot) {
            ++i;
            // Swap students
            swap(studentNames[i], studentNames[j]);
            swap(studentAverages[i], studentAverages[j]);
            for (int k = 0; k < numAssignments; ++k) {
                swap(studentGrades[i][k], studentGrades[j][k]);
            }
        }
    }
    
    // Swap pivot
    swap(studentNames[i + 1], studentNames[high]);
    swap(studentAverages[i + 1], studentAverages[high]);
    for (int k = 0; k < numAssignments; ++k) {
        swap(studentGrades[i + 1][k], studentGrades[high][k]);
    }
    
    return i + 1;
}

// Merge Sort implementation for sorting by average (descending)
void mergeSortByAverage(int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortByAverage(left, mid);
        mergeSortByAverage(mid + 1, right);
        merge(left, mid, right);
    }
}

void merge(int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    // Create temporary arrays (fixed-size for portability)
    string tempNames1[MAX_STUDENTS], tempNames2[MAX_STUDENTS];
    double tempAvgs1[MAX_STUDENTS], tempAvgs2[MAX_STUDENTS];
    double tempGrades1[MAX_STUDENTS][MAX_ASSIGNMENTS], tempGrades2[MAX_STUDENTS][MAX_ASSIGNMENTS];
    
    // Copy data to temp arrays
    for (int i = 0; i < n1; ++i) {
        tempNames1[i] = studentNames[left + i];
        tempAvgs1[i] = studentAverages[left + i];
        for (int k = 0; k < numAssignments; ++k) {
            tempGrades1[i][k] = studentGrades[left + i][k];
        }
    }
    for (int j = 0; j < n2; ++j) {
        tempNames2[j] = studentNames[mid + 1 + j];
        tempAvgs2[j] = studentAverages[mid + 1 + j];
        for (int k = 0; k < numAssignments; ++k) {
            tempGrades2[j][k] = studentGrades[mid + 1 + j][k];
        }
    }
    
    // Merge (descending order by average)
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (tempAvgs1[i] >= tempAvgs2[j]) {
            studentNames[k] = tempNames1[i];
            studentAverages[k] = tempAvgs1[i];
            for (int m = 0; m < numAssignments; ++m) {
                studentGrades[k][m] = tempGrades1[i][m];
            }
            ++i;
        } else {
            studentNames[k] = tempNames2[j];
            studentAverages[k] = tempAvgs2[j];
            for (int m = 0; m < numAssignments; ++m) {
                studentGrades[k][m] = tempGrades2[j][m];
            }
            ++j;
        }
        ++k;
    }
    
    // Copy remaining elements
    while (i < n1) {
        studentNames[k] = tempNames1[i];
        studentAverages[k] = tempAvgs1[i];
        for (int m = 0; m < numAssignments; ++m) {
            studentGrades[k][m] = tempGrades1[i][m];
        }
        ++i;
        ++k;
    }
    while (j < n2) {
        studentNames[k] = tempNames2[j];
        studentAverages[k] = tempAvgs2[j];
        for (int m = 0; m < numAssignments; ++m) {
            studentGrades[k][m] = tempGrades2[j][m];
        }
        ++j;
        ++k;
    }
}

// Binary Search implementation
int binarySearchStudent(const string& name) {
    int left = 0, right = numStudents - 1;
    
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
