// Exercise #11 - Case Manipulator
#include <iostream>
#include <cctype>
using namespace std;

// Converts all characters in the string to uppercase
void upper(char* str)
{
    while (*str != '\0')
    {
        *str = toupper(*str);
        str++;
    }
}

// Converts all characters in the string to lowercase
void lower(char* str)
{
    while (*str != '\0')
    {
        *str = tolower(*str);
        str++;
    }
}

// Swaps the case of each character: upper -> lower, lower -> upper
void reverse(char* str)
{
    while (*str != '\0')
    {
        if (isupper(*str))
            *str = tolower(*str);
        else if (islower(*str))
            *str = toupper(*str);
        str++;
    }
}

int main()
{
    const int SIZE = 200;
    char input[SIZE];

    cout << "Enter a string: ";
    cin.getline(input, SIZE);

    cout << "\nOriginal: " << input << endl;

    reverse(input);
    cout << "After reverse: " << input << endl;

    lower(input);
    cout << "After lower:   " << input << endl;

    upper(input);
    cout << "After upper:   " << input << endl;

    return 0;
}
