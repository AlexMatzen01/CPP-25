#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// Function prototypes
int getTotalDays();
void getTravelTimes(int& departureHour, int& departureMinute, 
                   int& arrivalHour, int& arrivalMinute);
double getAirfare();
double getCarRental();
double getPrivateVehicleExpense();
double getParkingFees(int days);
double getTaxiFees(int days);
double getRegistrationFees();
double getHotelExpenses(int days);
double getMealExpenses(int days, int departureHour, int departureMinute,
                      int arrivalHour, int arrivalMinute);
bool isValidTime(int hour, int minute);

int main() {
    // Variables for storing expenses
    int totalDays;
    int departureHour, departureMinute, arrivalHour, arrivalMinute;
    double totalExpenses = 0.0;
    double allowableExpenses = 0.0;
    
    // Get trip information
    cout << "Business Trip Expense Calculator\n\n";
    
    totalDays = getTotalDays();
    getTravelTimes(departureHour, departureMinute, arrivalHour, arrivalMinute);
    
    // Calculate all expenses
    double airfare = getAirfare();
    double carRental = getCarRental();
    double vehicleExpense = getPrivateVehicleExpense();
    double parkingFees = getParkingFees(totalDays);
    double taxiFees = getTaxiFees(totalDays);
    double registrationFees = getRegistrationFees();
    double hotelExpenses = getHotelExpenses(totalDays);
    double mealExpenses = getMealExpenses(totalDays, departureHour, departureMinute,
                                        arrivalHour, arrivalMinute);
    
    // Calculate totals
    totalExpenses = airfare + carRental + vehicleExpense + parkingFees +
                    taxiFees + registrationFees + hotelExpenses + mealExpenses;
    
    // Calculate allowable amounts
    double allowableParking = min(6.0 * totalDays, parkingFees);
    double allowableTaxi = min(10.0 * totalDays, taxiFees);
    double allowableHotel = min(90.0 * (totalDays - 1), hotelExpenses);
    
    allowableExpenses = airfare + carRental + vehicleExpense + allowableParking +
                       allowableTaxi + registrationFees + allowableHotel + mealExpenses;
    
    // Display results
    cout << fixed << setprecision(2);
    cout << "\nExpense Report:\n";
    cout << "----------------------------------------\n";
    cout << "Total Expenses:         $" << totalExpenses << endl;
    cout << "Allowable Expenses:     $" << allowableExpenses << endl;
    
    if (totalExpenses > allowableExpenses) {
        cout << "Amount to be reimbursed: $" << totalExpenses - allowableExpenses << endl;
    } else {
        cout << "Amount saved:           $" << allowableExpenses - totalExpenses << endl;
    }
    
    return 0;
}

int getTotalDays() {
    int days;
    do {
        cout << "Enter the total number of days spent on the trip: ";
        cin >> days;
        if (days < 1)
            cout << "Please enter a valid number of days (must be at least 1).\n";
    } while (days < 1);
    return days;
}

void getTravelTimes(int& departureHour, int& departureMinute,
                   int& arrivalHour, int& arrivalMinute) {
    do {
        cout << "Enter departure time (24-hour format, HH MM): ";
        cin >> departureHour >> departureMinute;
        if (!isValidTime(departureHour, departureMinute))
            cout << "Please enter a valid time.\n";
    } while (!isValidTime(departureHour, departureMinute));
    
    do {
        cout << "Enter arrival time (24-hour format, HH MM): ";
        cin >> arrivalHour >> arrivalMinute;
        if (!isValidTime(arrivalHour, arrivalMinute))
            cout << "Please enter a valid time.\n";
    } while (!isValidTime(arrivalHour, arrivalMinute));
}

double getAirfare() {
    double amount;
    do {
        cout << "Enter round-trip airfare (0 if none): $";
        cin >> amount;
        if (amount < 0)
            cout << "Please enter a valid amount (must be non-negative).\n";
    } while (amount < 0);
    return amount;
}

double getCarRental() {
    double amount;
    do {
        cout << "Enter car rental fees (0 if none): $";
        cin >> amount;
        if (amount < 0)
            cout << "Please enter a valid amount (must be non-negative).\n";
    } while (amount < 0);
    return amount;
}

double getPrivateVehicleExpense() {
    double miles;
    do {
        cout << "Enter miles driven if private vehicle was used (0 if none): ";
        cin >> miles;
        if (miles < 0)
            cout << "Please enter a valid number of miles (must be non-negative).\n";
    } while (miles < 0);
    return miles * 0.27;
}

double getParkingFees(int days) {
    double totalFees = 0;
    for (int i = 1; i <= days; i++) {
        double fee;
        do {
            cout << "Enter parking fees for day " << i << " (0 if none): $";
            cin >> fee;
            if (fee < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (fee < 0);
        totalFees += fee;
    }
    return totalFees;
}

double getTaxiFees(int days) {
    double totalFees = 0;
    for (int i = 1; i <= days; i++) {
        double fee;
        do {
            cout << "Enter taxi fees for day " << i << " (0 if none): $";
            cin >> fee;
            if (fee < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (fee < 0);
        totalFees += fee;
    }
    return totalFees;
}

double getRegistrationFees() {
    double amount;
    do {
        cout << "Enter conference/seminar registration fees (0 if none): $";
        cin >> amount;
        if (amount < 0)
            cout << "Please enter a valid amount (must be non-negative).\n";
    } while (amount < 0);
    return amount;
}

double getHotelExpenses(int days) {
    double totalExpenses = 0;
    for (int i = 1; i < days; i++) {  // One less than total days
        double expense;
        do {
            cout << "Enter hotel expense for night " << i << ": $";
            cin >> expense;
            if (expense < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (expense < 0);
        totalExpenses += expense;
    }
    return totalExpenses;
}

double getMealExpenses(int days, int departureHour, int departureMinute,
                      int arrivalHour, int arrivalMinute) {
    double totalMealExpenses = 0;
    const double MAX_BREAKFAST = 9.0;
    const double MAX_LUNCH = 12.0;
    const double MAX_DINNER = 16.0;
    
    // First day meals
    if (departureHour < 7 || (departureHour == 7 && departureMinute == 0)) {
        double breakfast;
        do {
            cout << "Enter breakfast expense for first day: $";
            cin >> breakfast;
            if (breakfast < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (breakfast < 0);
        totalMealExpenses += min(breakfast, MAX_BREAKFAST);
    }
    
    if (departureHour < 12) {
        double lunch;
        do {
            cout << "Enter lunch expense for first day: $";
            cin >> lunch;
            if (lunch < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (lunch < 0);
        totalMealExpenses += min(lunch, MAX_LUNCH);
    }
    
    if (departureHour < 18) {
        double dinner;
        do {
            cout << "Enter dinner expense for first day: $";
            cin >> dinner;
            if (dinner < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (dinner < 0);
        totalMealExpenses += min(dinner, MAX_DINNER);
    }
    
    // Middle days (full days)
    for (int i = 2; i < days; i++) {
        double breakfast, lunch, dinner;
        
        do {
            cout << "Enter breakfast expense for day " << i << ": $";
            cin >> breakfast;
            if (breakfast < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (breakfast < 0);
        totalMealExpenses += min(breakfast, MAX_BREAKFAST);
        
        do {
            cout << "Enter lunch expense for day " << i << ": $";
            cin >> lunch;
            if (lunch < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (lunch < 0);
        totalMealExpenses += min(lunch, MAX_LUNCH);
        
        do {
            cout << "Enter dinner expense for day " << i << ": $";
            cin >> dinner;
            if (dinner < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (dinner < 0);
        totalMealExpenses += min(dinner, MAX_DINNER);
    }
    
    // Last day meals
    if (arrivalHour > 8 || (arrivalHour == 8 && arrivalMinute > 0)) {
        double breakfast;
        do {
            cout << "Enter breakfast expense for last day: $";
            cin >> breakfast;
            if (breakfast < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (breakfast < 0);
        totalMealExpenses += min(breakfast, MAX_BREAKFAST);
    }
    
    if (arrivalHour > 13 || (arrivalHour == 13 && arrivalMinute > 0)) {
        double lunch;
        do {
            cout << "Enter lunch expense for last day: $";
            cin >> lunch;
            if (lunch < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (lunch < 0);
        totalMealExpenses += min(lunch, MAX_LUNCH);
    }
    
    if (arrivalHour > 19 || (arrivalHour == 19 && arrivalMinute > 0)) {
        double dinner;
        do {
            cout << "Enter dinner expense for last day: $";
            cin >> dinner;
            if (dinner < 0)
                cout << "Please enter a valid amount (must be non-negative).\n";
        } while (dinner < 0);
        totalMealExpenses += min(dinner, MAX_DINNER);
    }
    
    return totalMealExpenses;
}

bool isValidTime(int hour, int minute) {
    return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60);
}
