/*
#pizza_pi accepts 0 arguments
#it asks for pizza diameter, people attending, and slices per person
#it outputs slices per pizza (based on 14.125 in^2 per slice) and the number of pizzas needed
#Area = π * r^2 ; slices = area / 14.125
*/
#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    const double SLICE_AREA = 14.125;
    const double PI = 3.14159265358979323846;

    double diameter = 0.0;
    std::cout << "What is the pizza's diameter (in inches)? ";
    std::cin >> diameter;

    double radius = diameter / 2.0;
    double area = PI * radius * radius;
    double slicesExact = area / SLICE_AREA;

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "\nA " << diameter << "-inch pizza yields approximately ";
    std::cout << slicesExact << " slices\n";

    int people = 0;
    std::cout << "\nHow many guests are there? ";
    std::cin >> people;

    int slicesPerPerson = 0;
    std::cout << "\nHow many slices does each person want? ";
    std::cin >> slicesPerPerson;

    int requiredSlices = people * slicesPerPerson;
    double pizzasNeeded = requiredSlices / slicesExact;

    std::cout << "You should order " << pizzasNeeded << " pizzas in total.\n";
    return 0;
}
