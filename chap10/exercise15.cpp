// Exercise #15 - Character Analysis
#include <iostream>
#include <fstream>
#include <cctype>
using namespace std;

int main()
{
    ifstream inFile("text.txt");

    if (!inFile)
    {
        cout << "Error: Could not open text.txt" << endl;
        return 1;
    }

    int upperCount    = 0;
    int lowerCount    = 0;
    int digitCount    = 0;
    int sentenceCount = 0;

    char ch;
    while (inFile.get(ch))
    {
        if (isupper(ch))  upperCount++;
        if (islower(ch))  lowerCount++;
        if (isdigit(ch))  digitCount++;
        if (ch == '.' || ch == '!' || ch == '?')  sentenceCount++;
    }

    inFile.close();

    cout << "Character Analysis of text.txt:" << endl;
    cout << "  Uppercase characters : " << upperCount    << endl;
    cout << "  Lowercase characters : " << lowerCount    << endl;
    cout << "  Digits               : " << digitCount    << endl;
    cout << "  Sentences            : " << sentenceCount << endl;

    return 0;
}
