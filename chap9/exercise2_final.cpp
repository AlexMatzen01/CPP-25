#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;

// Function prototypes
void sortScores(int *scores, int size);
double calculateAverage(int *scores, int size);
void displayAndSaveResults(int *scores, int size, double average);

int main() {
    int numScores;

    // Member 1: Input Handling and Validation
    cout << "Enter the number of test scores to enter: ";
    while (!(cin >> numScores) || numScores <= 0) {
        cout << "Invalid input. Please enter a positive integer for the number of scores: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }

    // Dynamic Memory Allocation
    int *scores = new int[numScores];

    cout << "Please enter " << numScores << " test scores (non-negative integers):\n";
    for (int i = 0; i < numScores; i++) {
        cout << "Score #" << (i + 1) << ": ";
        while (!(cin >> *(scores + i)) || *(scores + i) < 0) {
            cout << "Invalid score. Enter a non-negative integer: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
    }

    // Member 2: Sorting Algorithm
    sortScores(scores, numScores);

    // Member 1: Average Calculation
    double average = calculateAverage(scores, numScores);

    // Member 2: Display and File I/O
    displayAndSaveResults(scores, numScores, average);

    // Free memory
    delete[] scores;
    scores = nullptr;

    return 0;
}

// Sorting function using pointer notation
void sortScores(int *scores, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (*(scores + j) > *(scores + j + 1)) {
                // Swap using pointer notation
                int temp = *(scores + j);
                *(scores + j) = *(scores + j + 1);
                *(scores + j + 1) = temp;
            }
        }
    }
}

// Average calculation dropping the lowest score, using pointer notation
double calculateAverage(int *scores, int size) {
    if (size <= 1) {
        return 0.0;
    }

    int total = 0;
    // Since scores are sorted, the lowest is at *(scores + 0)
    // Start loop from index 1 to drop the lowest
    for (int i = 1; i < size; i++) {
        total += *(scores + i);
    }

    return static_cast<double>(total) / (size - 1);
}

// Display results to screen and save to results.txt
void displayAndSaveResults(int *scores, int size, double average) {
    // Screen Output
    cout << "\n----------------------------------------\n";
    cout << "            TEST SCORE RESULTS          \n";
    cout << "----------------------------------------\n";
    cout << "Sorted Scores (ascending order):\n";
    for (int i = 0; i < size; i++) {
        cout << *(scores + i) << " ";
    }
    cout << "\n\nAverage Score (lowest dropped): " << fixed << setprecision(2) << average << endl;
    cout << "----------------------------------------\n";

    // File I/O
    ofstream outFile("results.txt");
    if (outFile) {
        outFile << "TEST SCORE RESULTS\n";
        outFile << "==================\n";
        outFile << "Sorted Scores:\n";
        for (int i = 0; i < size; i++) {
            outFile << *(scores + i) << " ";
        }
        outFile << "\n\nAverage Score (dropped lowest): " << fixed << setprecision(2) << average << endl;
        outFile.close();
        cout << "Results have been saved to 'results.txt'.\n";
    } else {
        cout << "Error: Could not open results.txt for writing.\n";
    }
}
