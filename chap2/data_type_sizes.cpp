// data_type_sizes accepts no arguments
// it checks the size of basic data types in C++
// it outputs the size in bytes of char, int, float, and double

#include <iostream>

int main() {
    std::cout << "A char data type uses " << sizeof(char) << " byte(s) of memory.\n";
    std::cout << "An int data type uses " << sizeof(int) << " byte(s) of memory.\n";
    std::cout << "A float data type uses " << sizeof(float) << " byte(s) of memory.\n";
    std::cout << "A double data type uses " << sizeof(double) << " byte(s) of memory.\n";
    
    return 0;
}
