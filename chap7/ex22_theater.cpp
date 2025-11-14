/*
# TheaterSeating accepts (no) arguments
# it provides a skeleton for a theater seating ticketing system for a 15x30 auditorium
# it allows entering prices per row, viewing the seating chart, purchasing seats, and viewing sales summary
*/

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

// helper: print seating chart
void printSeatingChart(const vector<vector<char>>& seats) {
    const int SEATS = seats.empty() ? 0 : (int)seats[0].size();
    cout << "\nSeating chart (# available, * taken):\n";
    cout << "         Seats\n";
    cout << "    ";
    for (int s = 1; s <= SEATS; ++s) cout << (s % 10);
    cout << "\n";
    for (size_t r = 0; r < seats.size(); ++r) {
        cout << "Row " << setw(2) << (r + 1) << '\t';
        for (int s = 0; s < SEATS; ++s) cout << seats[r][s];
        cout << '\n';
    }
}

// helper: compute available and sold counts
pair<int,int> computeAvailability(const vector<vector<char>>& seats, int rowIndex = -1) {
    int totalAvailable = 0, totalSold = 0;
    for (size_t r = 0; r < seats.size(); ++r) {
        for (size_t s = 0; s < seats[r].size(); ++s) {
            if (seats[r][s] == '#') ++totalAvailable;
            else ++totalSold;
        }
    }
    if (rowIndex >= 0 && rowIndex < (int)seats.size()) {
        int avail = 0;
        for (size_t s = 0; s < seats[rowIndex].size(); ++s) if (seats[rowIndex][s] == '#') ++avail;
        return {avail, (int)seats[rowIndex].size() - avail};
    }
    return {totalAvailable, totalSold};
}

int main(){
    const int ROWS = 15;
    const int SEATS = 30;
    vector<double> prices(ROWS);
    vector<vector<char>> seats(ROWS, vector<char>(SEATS, '#'));
    double totalSales = 0.0;

    cout << "Enter ticket price for each row (1-15):\n";
    for (int r=0;r<ROWS;++r){
        cout << " Row " << (r+1) << " price: ";
        while (!(cin >> prices[r]) || prices[r] < 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  Enter non-negative price: ";
        }
    }

    while (true) {
        printSeatingChart(seats);

        cout << "\nOptions:\n";
        cout << "1) Buy ticket(s)\n";
        cout << "2) View total sales\n";
        cout << "3) Row/seat availability summary\n";
        cout << "4) Quit\n";
        cout << "Choose: ";

        int choice;
        if (!(cin >> choice)) {
            if (cin.eof()) break;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice.\n";
            continue;
        }

        if (choice == 1) {
            // group purchase support
            cout << "How many tickets would you like to buy? ";
            int k;
            if (!(cin >> k) || k <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid number of tickets.\n";
                continue;
            }
            if (k > ROWS * SEATS) { cout << "Too many tickets requested.\n"; continue; }

            vector<pair<int,int>> requested;
            requested.reserve(k);
            cout << "Enter the row and seat number for each ticket (e.g. 3 12).\n";
            for (int i=0;i<k;++i) {
                cout << "Ticket " << (i+1) << ": ";
                int rr, ss;
                if (!(cin >> rr >> ss)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Aborting transaction.\n";
                    requested.clear();
                    break;
                }
                requested.emplace_back(rr, ss);
            }
            if (requested.empty()) continue;

            // validate all requested seats first
            bool ok = true;
            double transactionTotal = 0.0;
            for (auto &p : requested) {
                int rr = p.first, ss = p.second;
                if (rr < 1 || rr > ROWS || ss < 1 || ss > SEATS) {
                    cout << "Seat (" << rr << "," << ss << ") is invalid. Transaction canceled.\n";
                    ok = false; break;
                }
                if (seats[rr-1][ss-1] == '*') {
                    cout << "Seat (" << rr << "," << ss << ") already sold. Transaction canceled.\n";
                    ok = false; break;
                }
                transactionTotal += prices[rr-1];
            }
            if (!ok) continue;

            // commit the transaction
            for (auto &p : requested) seats[p.first-1][p.second-1] = '*';
            totalSales += transactionTotal;
            cout << "Transaction complete. Total = $" << fixed << setprecision(2) << transactionTotal << "\n";

        } else if (choice == 2) {
            cout << "Total sales: $" << fixed << setprecision(2) << totalSales << "\n";
        } else if (choice == 3) {
            int grandAvailable = 0;
            int grandSold = 0;
            for (int r=0;r<ROWS;++r){
                int avail = 0;
                for (int s=0;s<SEATS;++s) if (seats[r][s] == '#') ++avail;
                int sold = SEATS - avail;
                cout << "Row " << (r+1) << ": " << sold << " sold, " << avail << " available\n";
                grandAvailable += avail;
                grandSold += sold;
            }
            cout << "Total sold: " << grandSold << "\n";
            cout << "Total available seats: " << grandAvailable << "\n";
        } else if (choice == 4) {
            break;
        } else {
            cout << "Unknown menu choice.\n";
        }
    }

    cout << "Exiting. Total sales: $" << fixed << setprecision(2) << totalSales << "\n";
    return 0;
}
