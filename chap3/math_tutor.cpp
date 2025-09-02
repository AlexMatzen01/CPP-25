// h_tutor accepts 0 arguments
// generates two random integers from 1 to 500, shows an addition problem, and waits for the student
// outputs the correct solution after the user presses Enter

#include <iostream>
#include <random>
#include <string>

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 500);

    int a = dist(gen);
    int b = dist(gen);

    std::cout << "Add the following numbers:\n\n";
    std::cout << "  " << a << "\n+ " << b << "\n------\n";
    std::cout << "\nPress Enter when you're ready to see the answer...";
    std::string dummy;
    std::getline(std::cin, dummy);

    int answer = a + b;
    std::cout << "\nCorrect answer: " << answer << "\n";
    return 0;
}
