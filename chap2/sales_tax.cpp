// sales_tax accepts no arguments
// it calculates state and county sales tax for each purchase
// it outputs the purchase amount, state tax, county tax, and total

#include <iostream>
#include <iomanip>

int main() {
    const double STATE_RATE = 0.04;
    const double COUNTY_RATE = 0.02;
    double purchases[] = { 75.00, 85.00, 100.00 };

    for (double price : purchases) {
        double stateTax = price * STATE_RATE;
        double countyTax = price * COUNTY_RATE;
        double total = price + stateTax + countyTax;

        std::cout << "Purchase:   $";
        std::cout << std::fixed << std::setprecision(2) << price << '\n';

        std::cout << "State Tax:  $";
        std::cout << std::fixed << std::setprecision(1) << stateTax << '\n';

        std::cout << "County Tax: $";
        std::cout << std::fixed << std::setprecision(1) << countyTax << '\n';

        std::cout << "Total:      $";
        std::cout << std::fixed << std::setprecision(1) << total << '\n';

        std::cout << std::endl;
    }
    return 0;
}
