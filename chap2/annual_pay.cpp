// annual_pay accepts no arguments
// it calculates annual pay from fixed pay amount and pay periods
// it outputs pay per period, number of pay periods, and the annual pay

#include <iostream>
#include <iomanip>

int main() {
    const double payAmount = 2200.0;
    const int payPeriods = 26;
    double annualPay = payAmount * payPeriods;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Pay per period: $" << payAmount << '\n';
    std::cout << "Pay periods per year: " << payPeriods << '\n';
    std::cout << "Annual pay: $" << annualPay << '\n';
    return 0;
}
