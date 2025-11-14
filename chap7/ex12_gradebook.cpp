/*
# GradeBook accepts (no) arguments
# it prompts the user to enter five student names and four test scores for each student
# it calculates each student's average and assigns a letter grade, then displays the results
*/

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <array>
#include <limits>

using namespace std;

char letterGrade(double avg) {
    if (avg >= 90) return 'A';
    if (avg >= 80) return 'B';
    if (avg >= 70) return 'C';
    if (avg >= 60) return 'D';
    return 'F';
}

int main() {
    const int STUDENTS = 5;
    constexpr int TESTS = 4;
    vector<string> names(STUDENTS);
    vector<array<double, TESTS>> scores(STUDENTS);

    cout << "Grade Book - enter 5 students and 4 test scores each" << endl;
    for (int i = 0; i < STUDENTS; ++i) {
        cout << "\nStudent " << (i+1) << " name: ";
        while (true) {
            if (!getline(cin, names[i])) { cerr << "Failed to read name input.\n"; return 1; }
            // trim whitespace
            size_t s = names[i].find_first_not_of(" \t\r\n");
            if (s == string::npos) {
                cout << "  Name cannot be empty. Enter name: ";
                continue;
            }
            size_t e = names[i].find_last_not_of(" \t\r\n");
            names[i] = names[i].substr(s, e - s + 1);
            break;
        }
        for (int t = 0; t < TESTS; ++t) {
            double s;
            cout << "  Test " << (t+1) << " score: ";
            while (!(cin >> s) || s < 0 || s > 100) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "    Enter a valid score (0-100): ";
            }
            scores[i][t] = s;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush newline
    }

    cout << "\nResults:\n";
    cout << left << setw(20) << "Name" << setw(12) << "Average" << "Grade\n";
    cout << string(40, '-') << "\n";
    for (int i = 0; i < STUDENTS; ++i) {
        double sum = 0;
        for (double s : scores[i]) sum += s;
        double avg = sum / TESTS;
        cout << left << setw(20) << names[i]
             << setw(12) << fixed << setprecision(2) << avg
             << letterGrade(avg) << '\n';
    }

    return 0;
}
