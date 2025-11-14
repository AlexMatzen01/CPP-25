/*
# Gas1994 accepts (no) arguments
# it reads 52 weekly average gas prices from 1994_Weekly_Gas_Averages.txt
# it displays min/max with week and month name and monthly averages
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

int main(){
    // data_files is located relative to this source when running from the 'chap7' folder
    string path = "data_files/1994_Weekly_Gas_Averages.txt";
    ifstream in(path);
    if (!in) { cerr << "Could not open " << path << "\n"; return 1; }
    vector<double> weeks;
    double v;
    while (in >> v) {
        weeks.push_back(v);
    }
    if (in.bad()) { cerr << "I/O error while reading " << path << "\n"; return 1; }
    in.close();
    if (weeks.size() < 52) { cerr << "Expected 52 weekly values. Got " << weeks.size() << "\n"; return 1; }

    // Find min and max
    double minv = weeks[0], maxv = weeks[0];
    int mini = 0, maxi = 0;
    for (int i=0;i<52;++i){ if (weeks[i] < minv){ minv = weeks[i]; mini = i; } if (weeks[i] > maxv){ maxv = weeks[i]; maxi = i; } }

    vector<string> months = {"January","February","March","April","May","June","July","August","September","October","November","December"};
    // weeks per month mapping that sums to 52
    vector<int> weeksInMonth = {4,4,5,4,5,4,5,4,4,5,4,4};

    auto monthForWeek = [&](int weekIndex){ // 0-based weekIndex
        int w = weekIndex + 1;
        int acc=0;
        for (int m=0;m<12;++m){ acc += weeksInMonth[m]; if (w <= acc) return m; }
        return 11; // fallback
    };

    cout << fixed << setprecision(2);
    cout << "Lowest average: $" << minv << " in week " << (mini+1) << " (" << months[monthForWeek(mini)] << ")\n";
    cout << "Highest average: $" << maxv << " in week " << (maxi+1) << " (" << months[monthForWeek(maxi)] << ")\n";

    // Monthly averages
    cout << "\nAverage price per month:\n";
    int idx = 0;
    for (int m=0;m<12;++m){
        int cnt = weeksInMonth[m];
        if (cnt <= 0) { cout << months[m] << ": no weeks\n"; continue; }
        double sum = 0;
        int used = 0;
        for (int k=0;k<cnt && idx < (int)weeks.size(); ++k){ sum += weeks[idx++]; ++used; }
        if (used == 0) { cout << months[m] << ": no data\n"; }
        else cout << months[m] << ": $" << (sum / used) << '\n';
    }

    return 0;
}
