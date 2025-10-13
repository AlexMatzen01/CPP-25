// chapter6_menu.cpp
// Menu runner for Chapter 6 exercises (Exercise 103 + exercises 11,15,16,24)
// Presents a menu (1-5 where 5 = Quit) and calls functions for each exercise.

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

// Forward declarations for exercise functions
void exercise11_lowestDrop();
void exercise15_hospital();
void exercise16_population();
void exercise24_rpsls();

// Helper for menu
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    int choice = 0;
    do {
        cout << "\n=== Chapter 6 Exercise Menu ===\n";
        cout << "1. Exercise 11 - Lowest Score Drop\n";
        cout << "2. Exercise 15 - Overloaded Hospital\n";
        cout << "3. Exercise 16 - Population Projection\n";
        cout << "4. Exercise 24 - Rock, Paper, Scissors, Lizard, Spock\n";
        cout << "5. Quit\n";
        cout << "Enter your choice (1-5): ";
        cin >> choice;

        if (cin.fail() || choice < 1 || choice > 5) {
            cout << "Invalid selection. Please enter a number from 1 to 5.\n";
            clearInput();
            continue;
        }

        switch (choice) {
            case 1: exercise11_lowestDrop(); break;
            case 2: exercise15_hospital(); break;
            case 3: exercise16_population(); break;
            case 4: exercise24_rpsls(); break;
            case 5:
                cout << "Exiting Chapter 6 menu. Goodbye!\n";
                break;
        }
    } while (choice != 5);

    return 0;
}

// -------------------- Exercise 11: Lowest Score Drop --------------------
// Functions: getScore (reference), findLowest (returns index), calcAverage

void getScore(double &score) {
    while (true) {
        cout << "Enter a test score (0 - 100): ";
        if (!(cin >> score)) {
            cout << "Invalid input. Please enter a numeric score.\n";
            clearInput();
            continue;
        }
        if (score < 0.0 || score > 100.0) {
            cout << "Score must be between 0 and 100. Try again.\n";
            continue;
        }
        break;
    }
}

int findLowest(const vector<double> &scores) {
    int idx = 0;
    for (int i = 1; i < (int)scores.size(); ++i) {
        if (scores[i] < scores[idx]) idx = i;
    }
    return idx;
}

void calcAverage(const vector<double> &scores) {
    if (scores.size() < 5) {
        cout << "Not enough scores provided.\n";
        return;
    }
    double sum = 0.0;
    for (double s : scores) sum += s;
    int lowIdx = findLowest(scores);
    double adjusted = (sum - scores[lowIdx]) / 4.0;
    cout << fixed << setprecision(2);
    cout << "Lowest score dropped: " << scores[lowIdx] << "\n";
    cout << "Average of the remaining four scores: " << adjusted << "\n";
}

void exercise11_lowestDrop() {
    cout << "\n--- Exercise 11: Lowest Score Drop ---\n";
    vector<double> scores(5);
    for (int i = 0; i < 5; ++i) {
        cout << "Score #" << (i + 1) << ": ";
        getScore(scores[i]);
    }
    calcAverage(scores);
}

// -------------------- Exercise 15: Overloaded Hospital --------------------
// Two overloaded functions calculate total charges

double calculateCharges(int days, double dailyRate, double meds, double services) {
    // inpatient
    return days * dailyRate + meds + services;
}

double calculateCharges(double meds, double services) {
    // outpatient
    return meds + services;
}

void exercise15_hospital() {
    cout << "\n--- Exercise 15: Overloaded Hospital Charges ---\n";
    cout << "Was the patient (1) Inpatient or (2) Outpatient? Enter 1 or 2: ";
    int type = 0;
    while (true) {
        if (!(cin >> type) || (type != 1 && type != 2)) {
            cout << "Please enter 1 for Inpatient or 2 for Outpatient: ";
            clearInput();
            continue;
        }
        break;
    }

    if (type == 1) {
        int days;
        double dailyRate, meds, services;
        do {
            cout << "Enter number of days spent in hospital: ";
            cin >> days;
            if (cin.fail() || days < 0) { cout << "Please enter a non-negative integer.\n"; clearInput(); days = -1; }
        } while (days < 0);

        do {
            cout << "Enter daily rate: $"; cin >> dailyRate;
            if (cin.fail() || dailyRate < 0.0) { cout << "Enter a non-negative amount.\n"; clearInput(); dailyRate = -1; }
        } while (dailyRate < 0.0);

        do {
            cout << "Enter medication charges: $"; cin >> meds;
            if (cin.fail() || meds < 0.0) { cout << "Enter a non-negative amount.\n"; clearInput(); meds = -1; }
        } while (meds < 0.0);

        do {
            cout << "Enter hospital services charges: $"; cin >> services;
            if (cin.fail() || services < 0.0) { cout << "Enter a non-negative amount.\n"; clearInput(); services = -1; }
        } while (services < 0.0);

        double total = calculateCharges(days, dailyRate, meds, services);
        cout << fixed << setprecision(2);
        cout << "\nTotal inpatient charges: $" << total << "\n";

    } else {
        double services, meds;
        do {
            cout << "Enter hospital services charges: $"; cin >> services;
            if (cin.fail() || services < 0.0) { cout << "Enter a non-negative amount.\n"; clearInput(); services = -1; }
        } while (services < 0.0);

        do {
            cout << "Enter medication charges: $"; cin >> meds;
            if (cin.fail() || meds < 0.0) { cout << "Enter a non-negative amount.\n"; clearInput(); meds = -1; }
        } while (meds < 0.0);

        double total = calculateCharges(meds, services);
        cout << fixed << setprecision(2);
        cout << "\nTotal outpatient charges: $" << total << "\n";
    }
}

// -------------------- Exercise 16: Population --------------------
// N = P + B*P - D*P  (B and D are rates as decimals, e.g., 0.03 for 3%)

double calcPopulation(double P, double B, double D) {
    double N = P + (B * P) - (D * P);
    if (N < 0) N = 0.0;
    return N; 
}

void exercise16_population() {
    cout << "\nThis program calculates population change.\n";
    long long start;
    do {
        cout << "Enter the starting population size: ";
        if (!(cin >> start) || start < 2) { cout << "Starting population must be at least 2.\n"; clearInput(); start = -1; }
    } while (start < 2);

    double birthPct;
    do {
        cout << "Enter the annual birth rate (as % of current population): ";
        if (!(cin >> birthPct) || birthPct < 0.0) { cout << "Birth rate must be non-negative.\n"; clearInput(); birthPct = -1; }
    } while (birthPct < 0.0);

    double deathPct;
    do {
        cout << "Enter the annual death rate (as % of current population): ";
        if (!(cin >> deathPct) || deathPct < 0.0) { cout << "Death rate must be non-negative.\n"; clearInput(); deathPct = -1; }
    } while (deathPct < 0.0);

    int years;
    do {
        cout << "For how many years do you wish to view population changes? ";
        if (!(cin >> years) || years < 2) { cout << "Number of years must be at least 2.\n"; clearInput(); years = -1; }
    } while (years < 2);

    double B = birthPct / 100.0;
    double D = deathPct / 100.0;
    double P = static_cast<double>(start); // keep fractional growth between years

    cout << "\nStarting population: " << static_cast<long long>(P) << "\n\n";
    for (int y = 1; y <= years; ++y) {
        P = calcPopulation(P, B, D);
        cout << "Population at the end of year " << y << " is " << static_cast<long long>(P) << ".\n";
    }
}

// -------------------- Exercise 24: Rock, Paper, Scissors, Lizard, Spock --------------------

int compChoice() {
    return rand() % 5 + 1; // 1..5
}

int playerChoiceInt(const string &choice) {
    string s;
    for (char c : choice) s.push_back(tolower(static_cast<unsigned char>(c)));
    if (s == "rock") return 1;
    if (s == "paper") return 2;
    if (s == "scissors") return 3;
    if (s == "lizard") return 4;
    if (s == "spock") return 5;
    return 0;
}

string nameOf(int v) {
    switch (v) {
        case 1: return "Rock";
        case 2: return "Paper";
        case 3: return "Scissors";
        case 4: return "Lizard";
        case 5: return "Spock";
        default: return "Unknown";
    }
}

// winner: 0 tie, 1 player wins, 2 computer wins
int winner(int comp, int player) {
    if (comp == player) return 0;
    // Rules encoded: player beats comp?
    // Player wins cases (player,comp)
    const vector<pair<int,int>> playerWins = {
        {3,2}, // scissors cuts paper
        {2,1}, // paper covers rock
        {1,4}, // rock crushes lizard
        {4,5}, // lizard poisons Spock
        {5,3}, // Spock smashes scissors
        {3,4}, // scissors decapitates lizard
        {4,2}, // lizard eats paper
        {2,5}, // paper disproves Spock
        {5,1}, // Spock vaporizes rock
        {1,3}  // rock crushes scissors
    };
    for (auto &p : playerWins) if (p.first == player && p.second == comp) return 1;
    return 2;
}

string readPlayerWeapon() {
    string input;
    while (true) {
        cout << "Choose your weapon (rock, paper, scissors, lizard, spock): ";
        if (!getline(cin, input)) return string();
        // trim
        string t;
        for (char c : input) if (!isspace(static_cast<unsigned char>(c))) t.push_back(c);
        // allow words with spaces removed (e.g., user types extra spaces)
        string lowered;
        for (char c : input) lowered.push_back(tolower(static_cast<unsigned char>(c)));
        int val = playerChoiceInt(lowered);
        if (val == 0) {
            cout << "Invalid weapon. Please try again.\n";
            continue;
        }
        // return original trimmed lower
        return lowered;
    }
}

void exercise24_rpsls() {
    cout << "\n--- Exercise 24: Rock, Paper, Scissors, Lizard, Spock ---\n";
    srand(static_cast<unsigned int>(time(nullptr)));
    
    clearInput();
    while (true) {
        cout << "\nNew round:\n";
        // get player
        string pstr = readPlayerWeapon();
        if (pstr.empty()) break;
        int pval = playerChoiceInt(pstr);
        int cval = compChoice();
        cout << "You chose: " << nameOf(pval) << "\n";
        cout << "Computer chose: " << nameOf(cval) << "\n";
        int res = winner(cval, pval);
        if (res == 0) cout << "It's a tie!\n";
        else if (res == 1) cout << "You win!\n";
        else cout << "Computer wins!\n";

        cout << "Play again? (y/n): ";
        string ans;
        if (!getline(cin, ans)) break;
        if (ans.empty()) break; // treat empty as no
        char ch = tolower(static_cast<unsigned char>(ans[0]));
        if (ch != 'y') break;
    }
}
