// diamond_pattern accepts no arguments
// it prints a diamond pattern made from a given character
// it outputs the diamond shape to the console

#include <iostream>
#include <string>

void printDiamond(int n, char symbol) {
    for (int i = 1; i <= n; ++i) {
        std::cout << std::string(n - i, ' ') << std::string(2 * i - 1, symbol) << '\n';
    }
    for (int i = n - 1; i >= 1; --i) {
        std::cout << std::string(n - i, ' ') << std::string(2 * i - 1, symbol) << '\n';
    }
}

int main() {
    int halfHeight = 5;
    char symbol = '*';
    printDiamond(halfHeight, symbol);
    return 0;
}
