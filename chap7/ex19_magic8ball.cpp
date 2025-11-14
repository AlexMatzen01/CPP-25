/*
# Magic8Ball accepts (no) arguments
# it reads 12 responses from 8_ball_responses.txt, prompts the user for a question
# it randomly displays one of the responses; repeats until user quits
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>

using namespace std;

int main() {
    string path = "data_files/8_ball_responses.txt";
    ifstream in(path);
    if (!in) {
        cerr << "Could not open responses file: " << path << "\n";
        return 1;
    }
    vector<string> responses;
    string line;
    while (getline(in, line)) if (!line.empty()) responses.push_back(line);
    in.close();
    if (responses.empty()) {
        cerr << "No responses loaded.\n"; return 1;
    }
    // RNG
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rng(seed);
    uniform_int_distribution<int> dist(0, (int)responses.size()-1);

    cout << "Magic 8-Ball. Ask a question (or type 'quit'):\n";
    while (true) {
        string q;
        cout << "> ";
        if (!getline(cin, q)) { cout << "\nInput closed. Exiting.\n"; break; }
        // trim
        size_t start = q.find_first_not_of(" \t\r\n");
        if (start == string::npos) continue;
        size_t end = q.find_last_not_of(" \t\r\n");
        q = q.substr(start, end - start + 1);
        for (auto &ch : q) ch = (char)tolower((unsigned char)ch);
        if (q == "quit" || q == "exit") break;
        int i = dist(rng);
        cout << responses[i] << "\n";
    }
    cout << "Goodbye.\n";
    return 0;
}
