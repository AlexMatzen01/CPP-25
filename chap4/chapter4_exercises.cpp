/*
chapter4_exercises

This program puts all Chapter 4 exercises into one menu. Pick an exercise to run it.  
Includes:
 - Exercise 101: Switch-based exercise menu (this program)
 - Exercise 11: Math Tutor (revised)
 - Exercise 17: Personal Best (pole vaults)
 - Exercise 23: Geometry Calculator (areas)
 - Exercise 27: WiFi Diagnostic Tree (troubleshooter)
 - Exercise 28: Restaurant Selector (dietary filters)

*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>

using namespace std;

// Forward declarations for exercise functions
void exercise11_mathTutor();
void exercise17_personalBest();
void exercise23_geometryCalculator();
void exercise27_wifiDiagnostic();
void exercise28_restaurantSelector();

// Small input helper functions used by multiple exercises
void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Ask a yes/no question to the user. Accepts y/yes or n/no (case-insensitive).
// Returns true for yes, false for no.
bool askYesNo(const string &prompt)
{
    string resp;
    while (true) {
        cout << prompt << " (y/n): ";
        if (!(cin >> resp)) {
            clearInput();
            continue;
        }
        for (auto &c : resp) c = tolower(c);
        if (resp == "y" || resp == "yes") return true;
        if (resp == "n" || resp == "no") return false;
        cout << "Please enter 'y' or 'n'.\n";
    }
}

int main()
{
    while (true) {
        cout << "\n=== Chapter 4 Exercises Menu ===\n";
        cout << "1) Exercise 11 - Math Tutor (revised)\n";
        cout << "2) Exercise 17 - Personal Best\n";
        cout << "3) Exercise 23 - Geometry Calculator\n";
        cout << "4) Exercise 27 - WiFi Troubleshooter\n";
        cout << "5) Exercise 28 - Restaurant Selector\n";
        cout << "6) Quit\n";
        cout << "Choose an option (1-6): ";

        int choice;
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number between 1 and 6.\n";
            clearInput();
            continue;
        }

        switch (choice) {
            case 1: exercise11_mathTutor(); break;
            case 2: exercise17_personalBest(); break;
            case 3: exercise23_geometryCalculator(); break;
            case 4: exercise27_wifiDiagnostic(); break;
            case 5: exercise28_restaurantSelector(); break;
            case 6: cout << "Goodbye!\n"; return 0;
            default:
                cout << "Choice out of range. Please enter 1 - 6.\n";
        }
    }
    return 0;
}

/*
Exercise 11 - Math Tutor (revised)
This small exercise asks the user to supply two integers, then prompts for the
sum. It tells the user whether their answer was correct and, if not, shows
the correct equation.
*/
void exercise11_mathTutor()
{
    cout << "\n--- Exercise 11: Math Tutor (revised) ---\n";
    // Instead of random problems, ask the user for two integers so it's easy to test.
    int a, b;
    cout << "Enter the first integer: ";
    while (!(cin >> a)) { cout << "Please enter a valid integer: "; clearInput(); }
    cout << "Enter the second integer: ";
    while (!(cin >> b)) { cout << "Please enter a valid integer: "; clearInput(); }

    int correct = a + b;
    cout << "What is " << a << " + " << b << "? ";
    int answer;
    while (!(cin >> answer)) { cout << "Please enter a valid integer: "; clearInput(); }

    if (answer == correct) {
        cout << "That's correct — nice work!\n";
    } else {
        cout << "Not quite. The correct answer is: " << a << " + " << b << " = " << correct << "\n";
    }
}

/*
Exercise 17 - Personal Best
Ask for an athlete's name and their three best pole vaults (date and height).
Heights must be between 2 and 5 meters. The program then lists the vaults
sorted from highest to lowest, showing date and height.
*/
void exercise17_personalBest()
{
    cout << "\n--- Exercise 17: Personal Best ---\n";
    string athlete;
    cout << "Enter the athlete's name: ";
    clearInput();
    getline(cin, athlete);

    struct Vault { string date; double height; };
    vector<Vault> vaults;

    for (int i = 1; i <= 3; ++i) {
        Vault v;
        cout << "Enter date for vault #" << i << " (e.g., 2025-09-16 or Sep 16): ";
        getline(cin, v.date);
        cout << "Enter height in meters for vault #" << i << " (between 2 and 5): ";
        while (!(cin >> v.height) || v.height < 2.0 || v.height > 5.0) {
            cout << "That value is out of range. Please enter a height between 2 and 5 meters: ";
            clearInput();
        }
        clearInput();
        vaults.push_back(v);
    }

    sort(vaults.begin(), vaults.end(), [](const Vault &a, const Vault &b){ return a.height > b.height; });

    cout << "\nTop 3 vaults for " << athlete << ":\n";
    for (size_t i = 0; i < vaults.size(); ++i) {
        cout << (i+1) << ") " << vaults[i].date << " - " << vaults[i].height << " m\n";
    }
}

/*
Exercise 23 - Geometry Calculator
Displays a small menu for computing areas of basic shapes. Validates input and
prints a clear, formatted result. Selecting "Quit" returns to the main menu.
*/
void exercise23_geometryCalculator()
{
    cout << "\n--- Exercise 23: Geometry Calculator ---\n";
    while (true) {
        cout << "\nGeometry Calculator\n";
        cout << "1) Calculate the Area of a Circle\n";
        cout << "2) Calculate the Area of a Rectangle\n";
        cout << "3) Calculate the Area of a Triangle\n";
        cout << "4) Quit\n";
        cout << "Enter your choice (1 - 4): ";

        int choice;
        if (!(cin >> choice)) { cout << "Invalid input. Please enter 1 - 4.\n"; clearInput(); continue; }

        if (choice < 1 || choice > 4) { cout << "Error: selection must be between 1 and 4.\n"; continue; }
        if (choice == 4) { cout << "Returning to the main menu.\n"; break; }

        constexpr double PI = 3.14159265358979323846;
        if (choice == 1) {
            double radius;
            cout << "Enter the circle's radius: ";
            while (!(cin >> radius) || radius < 0.0) { cout << "Radius must be zero or positive. Try again: "; clearInput(); }
            double area = PI * radius * radius;
            cout << "Area of circle (r = " << radius << ") = " << area << "\n";
        } else if (choice == 2) {
            double length, width;
            cout << "Enter the rectangle's length: ";
            while (!(cin >> length) || length < 0.0) { cout << "Length must be zero or positive. Try again: "; clearInput(); }
            cout << "Enter the rectangle's width: ";
            while (!(cin >> width) || width < 0.0) { cout << "Width must be zero or positive. Try again: "; clearInput(); }
            double area = length * width;
            cout << "Area of rectangle (" << length << " x " << width << ") = " << area << "\n";
        } else if (choice == 3) {
            double base, height;
            cout << "Enter the triangle's base: ";
            while (!(cin >> base) || base < 0.0) { cout << "Base must be zero or positive. Try again: "; clearInput(); }
            cout << "Enter the triangle's height: ";
            while (!(cin >> height) || height < 0.0) { cout << "Height must be zero or positive. Try again: "; clearInput(); }
            double area = 0.5 * base * height;
            cout << "Area of triangle (base = " << base << ", height = " << height << ") = " << area << "\n";
        }
    }
}

/*
Exercise 27 - WiFi Troubleshooter
This interactive helper follows the flowchart steps to guide the user through
common fixes. Answer yes or no to each step. If one of the steps fixes the
problem, the program stops and reports success.
*/
void exercise27_wifiDiagnostic()
{
    cout << "\n--- Exercise 27: WiFi Troubleshooter ---\n";
    cout << "I'll guide you through a few simple checks. Answer y or n to each prompt.\n";

    if (askYesNo("Try rebooting the computer and then reconnecting. Did that fix the problem?")) {
        cout << "Connection restored — enjoy your stream!\n";
        return;
    }

    if (askYesNo("Try rebooting the router and then reconnecting. Did that fix the problem?")) {
        cout << "Connection restored — enjoy your stream!\n";
        return;
    }

    if (askYesNo("Check that all cables are firmly attached. Did that fix the problem?")) {
        cout << "Connection restored — enjoy your stream!\n";
        return;
    }

    if (askYesNo("Try moving the router to a better location and test again. Did that fix the problem?")) {
        cout << "Connection restored — enjoy your stream!\n";
        return;
    }

    cout << "If none of those steps worked, consider getting a new router.\n";
}

/*
Exercise 28 - Restaurant Selector
Ask whether members of the dining party are vegetarian, vegan, or gluten-free,
and then print a filtered list of nearby restaurants that meet those needs.
*/
void exercise28_restaurantSelector()
{
    cout << "\n--- Exercise 28: Restaurant Selector ---\n";
    bool vegetarian = askYesNo("Is anyone in your party vegetarian?");
    bool vegan = askYesNo("Is anyone in your party vegan?");
    bool glutenFree = askYesNo("Is anyone in your party gluten-free?");

    struct Restaurant { string name; bool veg; bool vgn; bool gf; };
    vector<Restaurant> restaurants = {
        {"Joe's Gourmet Burgers", false, false, false},
        {"Main Street Pizza Company", true, false, true},
        {"Corner Cafe", true, true, true},
        {"Mama's Fine Italian", true, false, false},
        {"The Chef's Kitchen", true, true, true}
    };

    cout << "\nRestaurants that match your group's requirements:\n";
    bool found = false;
    for (auto &r : restaurants) {
        if ((vegetarian && !r.veg) || (vegan && !r.vgn) || (glutenFree && !r.gf)) continue;
        cout << "- " << r.name << '\n';
        found = true;
    }
    if (!found) cout << "No restaurants available for the selected dietary restrictions.\n";
}
