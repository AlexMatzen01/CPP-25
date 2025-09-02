// average_rainfall accepts 0 arguments
// it asks for three month names and their rainfall (in inches), then computes the average
// it outputs the formatted average rainfall across the three months

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

int main() {
    std::string m1, m2, m3;
    double r1 = 0, r2 = 0, r3 = 0;

    std::cout << "Enter first month name: ";
    std::getline(std::cin, m1);
    std::cout << "Rainfall in inches for " << m1 << ": ";
    std::cin >> r1;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter second month name: ";
    std::getline(std::cin, m2);
    std::cout << "Rainfall in inches for " << m2 << ": ";
    std::cin >> r2;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter third month name: ";
    std::getline(std::cin, m3);
    std::cout << "Rainfall in inches for " << m3 << ": ";
    std::cin >> r3;

    double avg = (r1 + r2 + r3) / 3.0;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nAverage rainfall for " << m1 << ", " << m2 << ", and " << m3
        << " is " << avg << " inches.\n";
    return 0;
}
