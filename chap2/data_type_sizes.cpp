// data_type_sizes accepts no arguments
// it checks the size of basic data types in C++
// it outputs the size in bytes of char, int, float, and double

#include <iostream>

int main() {
    std::cout << "Size of char:   " << sizeof(char) << " byte(s)\n";
    std::cout << "Size of int:    " << sizeof(int) << " byte(s)\n";
    std::cout << "Size of float:  " << sizeof(float) << " byte(s)\n";
    std::cout << "Size of double: " << sizeof(double) << " byte(s)\n";
    return 0;
}
