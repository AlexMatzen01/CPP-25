/*
#monthly_payments accepts 0 arguments
#it asks for annual interest rate (percent), number of payments (months), and loan amount
#it outputs monthly payment, total paid, and interest paid using the standard amortization formula
#Formula from your brief:
#  Rate = (annual_rate / 100) / 12
#  Adjusted = (1 + Rate)^N
#  Payment = (Rate * Adjusted) / (Adjusted - 1) * L
#  Total Paid = Payment * N
#  Interest Paid = Total Paid - L
*/

#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    double annualRatePct = 0.0;
    int    N = 0;
    double L = 0.0;

    std::cout << "Loan amount: ";
    std::cin >> L;
    std::cout << "Annual interest rate (e.g., 12 for 12%): ";
    std::cin >> annualRatePct;
    std::cout << "Number of payments (months): ";
    std::cin >> N;

    double Rate = (annualRatePct / 100.0) / 12.0;   // monthly rate
    double Adjusted = std::pow(1.0 + Rate, N);
    double payment = (Rate * Adjusted) / (Adjusted - 1.0) * L;

    double totalPaid = payment * N;
    double interestPaid = totalPaid - L;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n--- Loan Report ---\n";
    std::cout << "Loan Amount     : $" << L << "\n";
    std::cout << "Monthly Rate    : " << (Rate * 100) << "%\n";
    std::cout << "Number of Payments: " << N << "\n";
    std::cout << "Monthly Payment : $" << payment << "\n";
    std::cout << "Total Paid Back : $" << totalPaid << "\n";
    std::cout << "Interest Paid   : $" << interestPaid << "\n";
    return 0;
}
