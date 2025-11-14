// ingredient_adjuster accepts 0 arguments
// asks how many cookies to make and scales the base recipe to that amount
// outputs the cups of sugar, butter, and flour needed

#include <iostream>
#include <iomanip>

int main() {
    const double BASE_COOKIES = 48.0;
    const double BASE_SUGAR_CUPS = 1.5;
    const double BASE_BUTTER_CUPS = 1.0;
    const double BASE_FLOUR_CUPS = 2.75;

    double desired = 0.0;
    std::cout << "How many cookies do you want to make? ";
    std::cin >> desired;

    double factor = desired / BASE_COOKIES;
    double sugar = BASE_SUGAR_CUPS * factor;
    double butter = BASE_BUTTER_CUPS * factor;
    double flour = BASE_FLOUR_CUPS * factor;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nIngredients needed for " << desired << " cookies:\n";
    std::cout << "  Sugar : " << sugar << " cups\n";
    std::cout << "  Butter: " << butter << " cups\n";
    std::cout << "  Flour : " << flour << " cups\n";
    return 0;
}
