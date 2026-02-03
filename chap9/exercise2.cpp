#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

// Function prototypes
void sortScores(int *scores, int size);
double calculateAverage(int *scores, int size);
void displayScores(int *scores, int size);

int main() {
    int numScores;

    // Get the number of scores
    cout << "Enter the number of test scores: ";
    while (!(cin >> numScores) || numScores <= 0) {
        cout << "Please enter a positive integer: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }

    // Dynamically allocate the array
    int *scores = new int[numScores];

    // Get the scores
    for (int i = 0; i < numScores; i++) {
        cout << "Enter score #" << (i + 1) << ": ";
        while (!(cin >> *(scores + i)) || *(scores + i) < 0) {
            cout << "Invalid score. Enter a non-negative integer: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
    }

    // Sort the scores
    sortScores(scores, numScores);

    // Display sorted list
    cout << "\nSorted Test Scores:\n";
    displayScores(scores, numScores);

    // Calculate and display average
    double average = calculateAverage(scores, numScores);
    cout << fixed << setprecision(2);
    cout << "\nAverage Score (dropping lowest): " << average << endl;

    // Free memory
    delete[] scores;

    return 0;
}

void sortScores(int *scores, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (*(scores + j) > *(scores + j + 1)) {
                // Swap
                int temp = *(scores + j);
                *(scores + j) = *(scores + j + 1);
                *(scores + j + 1) = temp;
            }
        }
    }
}

double calculateAverage(int *scores, int size) {
    if (size <= 1) return 0.0; // Cannot drop lowest if there's only 1 (or none)

    int total = 0;
    // Since it's sorted, the lowest score is at index 0.
    // Start from index 1.
    for (int i = 1; i < size; i++) {
        total += *(scores + i);
    }

    return static_cast<double>(total) / (size - 1);
}

void displayScores(int *scores, int size) {
    for (int i = 0; i < size; i++) {
        cout << *(scores + i) << " ";
    }
    cout << endl;
}
