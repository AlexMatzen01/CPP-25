/*
# DisplayMenu accepts (no) arguments
# it displays a menu and returns the user's choice
*/

#include "DisplayMenu.h"
#include <iostream>
#include <limits>
using namespace std;

int displayMenu(){
    cout << "\nChapter 7 Exercises Menu\n";
    cout << "1) Grade Book (ex12)\n";
    cout << "2) World Series Winners (ex16)\n";
    cout << "3) Tic-Tac-Toe (ex18)\n";
    cout << "4) Magic 8-Ball (ex19)\n";
    cout << "5) 1994 Gas Prices (ex20)\n";
    cout << "6) Theater Seating (ex22)\n";
    cout << "0) Quit\n";
    cout << "Enter choice: ";
    int choice;
    while (true) {
        if (cin >> choice) break;
        if (cin.eof()) return 0;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Enter numeric choice: ";
    }
    // consume rest of line
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return choice;
}
