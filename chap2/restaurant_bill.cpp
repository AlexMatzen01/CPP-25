// restaurant_bill accepts no arguments
// it calculates the total bill including tax and tip for a fixed meal price
// it outputs the meal cost, tax, tip, and total bill

#include <iostream>
#include <iomanip>

int main() {
    const double MEAL_COST = 88.67;
    const double TAX_RATE = 0.065;
    const double TIP_RATE = 0.20;

    double taxAmount = MEAL_COST * TAX_RATE;
    double tipAmount = MEAL_COST * TIP_RATE;
    double totalBill = MEAL_COST + taxAmount + tipAmount;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Meal cost:\t$" << MEAL_COST << '\n';
    std::cout << "Tax amount:\t$" << taxAmount << '\n';
    std::cout << "Tip amount:\t$" << tipAmount << '\n';
    std::cout << "Total bill:\t$" << totalBill << '\n';
    
    return 0;
}
