// Exercise #12 - Password Verifier
#include <iostream>
#include <cctype>
#include <cstring>
using namespace std;

int main()
{
    const int SIZE = 100;
    char password[SIZE];

    cout << "Enter a password: ";
    cin.getline(password, SIZE);

    int length    = strlen(password);
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool valid    = true;

    for (int i = 0; i < length; i++)
    {
        if (isupper(password[i])) hasUpper = true;
        if (islower(password[i])) hasLower = true;
        if (isdigit(password[i])) hasDigit = true;
    }

    if (length < 9)
    {
        cout << "Invalid: Password must be at least 9 characters long." << endl;
        valid = false;
    }
    if (!hasUpper)
    {
        cout << "Invalid: Password must contain at least one uppercase letter." << endl;
        valid = false;
    }
    if (!hasLower)
    {
        cout << "Invalid: Password must contain at least one lowercase letter." << endl;
        valid = false;
    }
    if (!hasDigit)
    {
        cout << "Invalid: Password must contain at least one digit." << endl;
        valid = false;
    }

    if (valid)
        cout << "Password is valid." << endl;

    return 0;
}
