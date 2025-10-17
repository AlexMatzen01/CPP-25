#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

using namespace std;

bool parseHHMM(const string &s, int &hour, int &minute) {
    size_t pos = s.find_first_of(".:,");
    if (pos == string::npos) return false;
    string h = s.substr(0, pos);
    string m = s.substr(pos + 1);
    try {
        hour = stoi(h);
        if (m.size() == 0) return false;
        if (m.size() > 2) m = m.substr(0, 2);
        if (m.size() == 1) m = string("0") + m;
        minute = stoi(m);
    } catch (...) {
        return false;
    }
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) return false;
    return true;
}

int getTotalDays() {
    int days;
    do {
        cout << "How many days were spent on the trip? ";
        cin >> days;
        if (days < 1) cout << "Please enter a valid number of days (at least 1).\n";
    } while (days < 1);
    return days;
}

void getTravelTimes(int &depH, int &depM, int &arrH, int &arrM) {
    string s;
    bool ok = false;
    do {
        cout << "Enter the departure time (HH.MM): ";
        cin >> s;
        ok = parseHHMM(s, depH, depM);
        if (!ok) cout << "Please enter time in HH.MM format (24-hour).\n";
    } while (!ok);

    ok = false;
    do {
        cout << "Enter the return time (HH.MM): ";
        cin >> s;
        ok = parseHHMM(s, arrH, arrM);
        if (!ok) cout << "Please enter time in HH.MM format (24-hour).\n";
    } while (!ok);
}

double getNonNegativeAmount(const string &prompt) {
    double a;
    do {
        cout << prompt;
        cin >> a;
        if (a < 0) cout << "Please enter a non-negative amount.\n";
    } while (a < 0);
    return a;
}

int main() {
    cout << fixed << setprecision(2);
    int days = getTotalDays();

    int depH=0, depM=0, arrH=0, arrM=0;
    getTravelTimes(depH, depM, arrH, arrM);

    double airfare = getNonNegativeAmount("Enter the amount of airfare: ");
    double carRental = getNonNegativeAmount("Enter the amount of car rental fees: ");
    double privateMiles;
    do {
        cout << "Enter the miles driven by a private vehicle: ";
        cin >> privateMiles;
        if (privateMiles < 0) cout << "Please enter a non-negative number.\n";
    } while (privateMiles < 0);
    double privateVehicle = privateMiles * 0.27;

    double parking = getNonNegativeAmount("Enter the amount of parking fees: ");
    double taxi = getNonNegativeAmount("Enter the amount of taxi fees: ");
    double registration = getNonNegativeAmount("Enter the amount of registration fees: ");
    double nightly = getNonNegativeAmount("Enter the nightly hotel rate: ");

    double hotelExpenses = 0.0;
    if (days > 1) hotelExpenses = nightly * (days - 1);

    const double MAX_BREAKFAST = 9.0;
    const double MAX_LUNCH = 12.0;
    const double MAX_DINNER = 16.0;

    double mealsTotal = 0.0;       // actual meal amounts (employee paid)
    double allowedMealsTotal = 0.0; // company-allowed meal amounts (capped)

    // Day 1
    cout << "Day 1:\n";
    if (depH < 7 || (depH == 7 && depM == 0)) {
        double b = getNonNegativeAmount("Enter the amount spent for breakfast: ");
        mealsTotal += b;
        allowedMealsTotal += min(b, MAX_BREAKFAST);
    }
    if (depH < 12) {
        double l = getNonNegativeAmount("Enter the amount spent for lunch: ");
        mealsTotal += l;
        allowedMealsTotal += min(l, MAX_LUNCH);
    }
    if (depH < 18) {
        double d = getNonNegativeAmount("Enter the amount spent for dinner: ");
        mealsTotal += d;
        allowedMealsTotal += min(d, MAX_DINNER);
    }

    // Middle days
    for (int day = 2; day <= days - 1; ++day) {
        cout << "Day " << day << ":\n";
        double b = getNonNegativeAmount("Enter the amount spent for breakfast: ");
        mealsTotal += b; allowedMealsTotal += min(b, MAX_BREAKFAST);
        double l = getNonNegativeAmount("Enter the amount spent for lunch: ");
        mealsTotal += l; allowedMealsTotal += min(l, MAX_LUNCH);
        double d = getNonNegativeAmount("Enter the amount spent for dinner: ");
        mealsTotal += d; allowedMealsTotal += min(d, MAX_DINNER);
    }

    // Last day
    if (days > 1) {
        cout << "Day " << days << ":\n";
        bool anyMeal = false;
        if (arrH > 8 || (arrH == 8 && arrM > 0)) {
            double b = getNonNegativeAmount("Enter the amount spent for breakfast: ");
            mealsTotal += b; allowedMealsTotal += min(b, MAX_BREAKFAST); anyMeal = true;
        }
        if (arrH > 13 || (arrH == 13 && arrM > 0)) {
            double l = getNonNegativeAmount("Enter the amount spent for lunch: ");
            mealsTotal += l; allowedMealsTotal += min(l, MAX_LUNCH); anyMeal = true;
        }
        if (arrH > 19 || (arrH == 19 && arrM > 0)) {
            double d = getNonNegativeAmount("Enter the amount spent for dinner: ");
            mealsTotal += d; allowedMealsTotal += min(d, MAX_DINNER); anyMeal = true;
        }
        if (!anyMeal) cout << "No meals allowed due to return before 8:00 a.m." << "\n";
    }

    double totalExpenses = airfare + carRental + privateVehicle + parking + taxi + registration + hotelExpenses + mealsTotal;
    double allowableParking = min(6.0 * days, parking);
    double allowableTaxi = min(10.0 * days, taxi);
    double allowableHotel = min(90.0 * (days - 1), hotelExpenses);

    double allowable = airfare + carRental + privateVehicle + allowableParking + allowableTaxi + registration + allowableHotel + allowedMealsTotal;

    cout << "\nTotal expenses: $" << totalExpenses << "\n\n";
    cout << "Allowable expenses: $" << allowable << "\n";
    if (totalExpenses > allowable) cout << "Amount to be reimbursed: $" << totalExpenses - allowable << "\n";
    else cout << "Amount to be reimbursed: $0.00\n";

    return 0;
}

