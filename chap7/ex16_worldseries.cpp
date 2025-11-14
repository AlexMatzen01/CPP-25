/*
# WorldSeriesWinners accepts (no) arguments
# it displays the list of teams (from Teams.txt), prompts the user for a team name,
# it counts how many times that team name appears in WorldSeriesWinners.txt and displays the count
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    string teamsPath = "data_files/Teams.txt";
    string winnersPath = "data_files/WorldSeriesWinners.txt";
    ifstream teams(teamsPath);
    if (!teams) {
        cerr << "Could not open " << teamsPath << "\n";
        return 1;
    }
    cout << "The following teams have won the World Series at least once:\n";
    vector<string> teamList;
    string line;
    while (getline(teams, line)) {
        if (!line.empty()) {
            teamList.push_back(line);
        }
    }
    if (teamList.empty()) {
        cerr << "No teams found in " << teamsPath << "\n";
        return 1;
    }
    // Print teams as a simple single-column list (one per line)
    for (const auto &t : teamList) {
        cout << "  " << t << "\n";
    }
    teams.close();

    cout << "\nEnter the name of one of the teams: ";
    string query;
    if (!getline(cin, query)) {
        cerr << "Failed to read input.\n";
        return 1;
    }

    // normalize query: trim and lowercase
    auto trim = [](string &s) {
        // trim left
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == string::npos) { s.clear(); return; }
        size_t end = s.find_last_not_of(" \t\n\r");
        s = s.substr(start, end - start + 1);
    };
    trim(query);
    string q = query;
    // lowercase helper
    auto to_lower = [](const string &s){ string r=s; transform(r.begin(), r.end(), r.begin(), ::tolower); return r; };
    string qlow = to_lower(q);

    ifstream winners(winnersPath);
    if (!winners) {
        cerr << "Could not open " << winnersPath << "\n";
        return 1;
    }
    int count = 0;
    while (getline(winners, line)) {
        // trim and lowercase each winner line before comparing
        string w = line;
        trim(w);
        string wlow = to_lower(w);
        if (qlow.empty()) continue;
        // substring match so inputs like "Senators" match "Washington Senators"
        if (wlow.find(qlow) != string::npos) ++count;
    }
    winners.close();

    // grammar: singular vs plural
    string verb = (count == 1) ? "has" : "have";
    cout << "\nThe " << q << " " << verb << " won the World Series " << count
         << " times between 1903 and 2012." << "\n";
    return 0;
}
