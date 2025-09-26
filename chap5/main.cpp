/*
#Chapter5Exercises accepts no arguments
#It presents a menu allowing the user to select and run one of several programming exercises for Chapter 5.
#It outputs the results of each exercise or exits the program when requested.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <iomanip>

using namespace std;

// Function prototypes
void randomNumberGuessingGame();
void numericProcessing();
void trianglePatterns();
void webPageCreator();
void averageSteps();
void clearInput();

int main() {
    int choice = 0;
    do {
        cout << "\n=== Chapter 5 Exercise Menu ===\n";
        cout << "1. Random Number Guessing Game\n";
        cout << "2. Numeric Processing\n";
        cout << "3. Triangle Patterns\n";
        cout << "4. Web Page Creator\n";
        cout << "5. Average Steps\n";
        cout << "6. Quit\n";
        cout << "Enter your choice (1-6): ";
        cin >> choice;

        if (cin.fail() || choice < 1 || choice > 6) {
            cout << "Invalid selection. Please enter a number from 1 to 6.\n";
            clearInput();
            continue;
        }

        switch (choice) {
            case 1:
                randomNumberGuessingGame();
                break;
            case 2:
                numericProcessing();
                break;
            case 3:
                trianglePatterns();
                break;
            case 4:
                webPageCreator();
                break;
            case 5:
                averageSteps();
                break;
            case 6:
                cout << "Exiting program. Goodbye!\n";
                break;
            default:
                break;
        }
    } while (choice != 6);
    return 0;
}

// Clears failed input and ignores rest of line
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

/*
#RandomNumberGuessingGame accepts no arguments
#It generates a random number from 1 to 1000 and prompts the user to guess it, providing hints.
#It outputs the number of guesses and whether the guess was correct.
*/
void randomNumberGuessingGame() {
    srand(static_cast<unsigned int>(time(0)));
    int target = rand() % 1000 + 1;
    int guess = 0, attempts = 0;

    cout << "\n--- Random Number Guessing Game ---\n";
    cout << "I've picked a number between 1 and 1000. Try to guess it!\n";

    do {
        cout << "Enter your guess (1-1000): ";
        cin >> guess;

        if (cin.fail() || guess < 1 || guess > 1000) {
            cout << "Invalid input. Please enter a number between 1 and 1000.\n";
            clearInput();
            continue;
        }

        attempts++;

        if (guess > target)
            cout << "Too high, try again.\n";
        else if (guess < target)
            cout << "Too low, try again.\n";
        else
            cout << "Congratulations! You guessed the number in " << attempts << " attempts.\n";
    } while (guess != target);
}

/*
#NumericProcessing accepts no arguments
#It reads numbers from 'data_files/Random.txt' and calculates count, sum, average, highest, and lowest values.
#It outputs all these calculated values.
*/
void numericProcessing() {
    ifstream fin("data_files/Random.txt");
    if (!fin) {
        cout << "Error: Could not open 'data_files/Random.txt'.\n";
        return;
    }

    int num, count = 0;
    long long sum = 0;
    int highest = numeric_limits<int>::min();
    int lowest = numeric_limits<int>::max();

    cout << "\n--- Numeric Processing ---\n";

    while (fin >> num) {
        count++;
        sum += num;
        if (num > highest) highest = num;
        if (num < lowest) lowest = num;
    }

    if (count == 0) {
        cout << "No numbers found in file.\n";
        return;
    }

    double average = static_cast<double>(sum) / count;

    cout << "Number of numbers: " << count << endl;
    cout << "Sum of numbers: " << sum << endl;
    cout << "Average of numbers: " << fixed << setprecision(2) << average << endl;
    cout << "Highest number: " << highest << endl;
    cout << "Lowest number: " << lowest << endl;
}

/*
#TrianglePatterns accepts no arguments
#It prints two triangle patterns: one increasing and one decreasing in size.
#It outputs both patterns to the console.
*/
void trianglePatterns() {
    cout << "\n--- Triangle Patterns ---\n";
    cout << "Pattern A:\n";
    for (int i = 1; i <= 10; i++) {
        for (int j = 1; j <= i; j++)
            cout << "*";
        cout << endl;
    }
    cout << "Pattern B:\n";
    for (int i = 10; i >= 1; i--) {
        for (int j = 1; j <= i; j++)
            cout << "*";
        cout << endl;
    }
}

/*
#WebPageCreator accepts no arguments
#It collects user information and creates a simple HTML webpage file with the input.
#It outputs a success message when the file is saved.
*/
void webPageCreator() {
    string name, description, filename;
    cout << "\n--- Web Page Creator ---\n";
    clearInput();
    cout << "Enter your name: ";
    getline(cin, name);
    cout << "Enter a sentence describing yourself: ";
    getline(cin, description);

    cout << "Enter a filename (without extension): ";
    getline(cin, filename);
    filename += ".html";

    ofstream fout(filename.c_str());
    if (!fout) {
        cout << "Error: Could not create file '" << filename << "'.\n";
        return;
    }

    fout << "<!DOCTYPE html>\n<html>\n<head>\n<title>" << name << "'s Webpage</title>\n</head>\n<body>\n";
    fout << "<h1>" << name << "</h1>\n";
    fout << "<p>" << description << "</p>\n";
    fout << "</body>\n</html>\n";
    fout.close();

    cout << "Webpage created successfully as '" << filename << "'.\n";
}

/*
#AverageSteps accepts no arguments
#It reads daily step counts from 'data_files/steps.txt' and calculates the average steps taken per month.
#It outputs each month and its average steps.
*/
void averageSteps() {
    const int months = 12;
    const int daysInMonth[months] = {31,28,31,30,31,30,31,31,30,31,30,31};
    string monthNames[months] = {"January","February","March","April","May","June","July","August","September","October","November","December"};

    ifstream fin("data_files/steps.txt");
    if (!fin) {
        cout << "Error: Could not open 'data_files/steps.txt'.\n";
        return;
    }

    cout << "\n--- Average Steps Per Month ---\n";

    int day = 0;
    for (int m = 0; m < months; m++) {
        long long monthSum = 0;
        for (int d = 0; d < daysInMonth[m]; d++) {
            int steps;
            if (!(fin >> steps)) {
                cout << "Error reading steps for " << monthNames[m] << " day " << (d+1) << ".\n";
                return;
            }
            monthSum += steps;
            day++;
        }
        double avg = static_cast<double>(monthSum) / daysInMonth[m];
        cout << monthNames[m] << ": " << fixed << setprecision(2) << avg << " average steps\n";
    }
}
