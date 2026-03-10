// speakers_bureau accepts no arguments.
// it lets the user add, edit, display, and search speaker records.
// it outputs a menu-driven list of speakers and topic search results.

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

using namespace std;

struct Speaker
{
    string name;
    string telephoneNumber;
    string speakingTopic;
    double feeRequired = 0.0;
    bool hasData = false;
};

const int MAX_SPEAKERS = 10;

string toLowerCopy(const string &text);
void displayMenu();
int getMenuChoice();
string getRequiredText(const string &prompt);
double getNonNegativeFee();
int getSpeakerIndex(const string &prompt, bool requireExisting, const Speaker speakers[], int size);
void addSpeaker(Speaker speakers[], int size);
void editSpeaker(Speaker speakers[], int size);
void displaySpeakers(const Speaker speakers[], int size);
void searchByTopic(const Speaker speakers[], int size);

int main()
{
    Speaker speakers[MAX_SPEAKERS]{};
    int menuChoice = 0;

    do
    {
        displayMenu();
        menuChoice = getMenuChoice();

        switch (menuChoice)
        {
        case 1:
            addSpeaker(speakers, MAX_SPEAKERS);
            break;
        case 2:
            editSpeaker(speakers, MAX_SPEAKERS);
            break;
        case 3:
            displaySpeakers(speakers, MAX_SPEAKERS);
            break;
        case 4:
            searchByTopic(speakers, MAX_SPEAKERS);
            break;
        case 5:
            cout << "Exiting Speakers' Bureau.\n";
            break;
        }

        cout << '\n';
    } while (menuChoice != 5);

    return 0;
}

string toLowerCopy(const string &text)
{
    string lowered = text;

    transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char character)
              { return static_cast<char>(tolower(character)); });

    return lowered;
}

void displayMenu()
{
    cout << "Speakers' Bureau Menu\n";
    cout << "1. Add speaker\n";
    cout << "2. Edit speaker\n";
    cout << "3. Display all speakers\n";
    cout << "4. Search by topic\n";
    cout << "5. Quit\n";
}

int getMenuChoice()
{
    int menuChoice = 0;

    while (true)
    {
        cout << "Enter your choice: ";

        if (cin >> menuChoice && menuChoice >= 1 && menuChoice <= 5)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return menuChoice;
        }

        cout << "Please enter a number from 1 to 5.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string getRequiredText(const string &prompt)
{
    string userInput;

    do
    {
        cout << prompt;
        getline(cin, userInput);

        if (userInput.empty())
        {
            cout << "This field cannot be empty.\n";
        }
    } while (userInput.empty());

    return userInput;
}

double getNonNegativeFee()
{
    double feeRequired = 0.0;

    while (true)
    {
        cout << "Enter the speaker fee: $";

        if (cin >> feeRequired && feeRequired >= 0.0)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return feeRequired;
        }

        cout << "Enter a non-negative fee amount.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int getSpeakerIndex(const string &prompt, bool requireExisting, const Speaker speakers[], int size)
{
    int speakerNumber = 0;

    while (true)
    {
        cout << prompt;

        if (!(cin >> speakerNumber))
        {
            cout << "Enter a whole number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (speakerNumber < 1 || speakerNumber > size)
        {
            cout << "Choose a speaker number between 1 and " << size << ".\n";
            continue;
        }

        const int index = speakerNumber - 1;

        if (requireExisting && !speakers[index].hasData)
        {
            cout << "That speaker slot is empty.\n";
            continue;
        }

        return index;
    }
}

void addSpeaker(Speaker speakers[], int size)
{
    const int index = getSpeakerIndex("Enter the speaker slot to fill (1-10): ", false, speakers, size);

    speakers[index].name = getRequiredText("Enter the speaker name: ");
    speakers[index].telephoneNumber = getRequiredText("Enter the telephone number: ");
    speakers[index].speakingTopic = getRequiredText("Enter the speaking topic: ");
    speakers[index].feeRequired = getNonNegativeFee();
    speakers[index].hasData = true;

    cout << "Speaker saved in slot " << (index + 1) << ".\n";
}

void editSpeaker(Speaker speakers[], int size)
{
    const int index = getSpeakerIndex("Enter the speaker number to edit (1-10): ", true, speakers, size);

    cout << "Editing speaker " << (index + 1) << ".\n";
    speakers[index].name = getRequiredText("Enter the speaker name: ");
    speakers[index].telephoneNumber = getRequiredText("Enter the telephone number: ");
    speakers[index].speakingTopic = getRequiredText("Enter the speaking topic: ");
    speakers[index].feeRequired = getNonNegativeFee();

    cout << "Speaker updated.\n";
}

void displaySpeakers(const Speaker speakers[], int size)
{
    bool foundSpeaker = false;

    cout << left << setw(6) << "Slot"
         << setw(22) << "Name"
         << setw(18) << "Telephone"
         << setw(24) << "Topic"
         << "Fee\n";
    cout << string(80, '-') << '\n';

    for (int index = 0; index < size; ++index)
    {
        if (!speakers[index].hasData)
        {
            continue;
        }

        foundSpeaker = true;
        cout << left << setw(6) << (index + 1)
             << setw(22) << speakers[index].name.substr(0, 21)
             << setw(18) << speakers[index].telephoneNumber.substr(0, 17)
             << setw(24) << speakers[index].speakingTopic.substr(0, 23)
             << '$' << fixed << setprecision(2) << speakers[index].feeRequired << '\n';
    }

    if (!foundSpeaker)
    {
        cout << "No speaker records are stored.\n";
    }
}

void searchByTopic(const Speaker speakers[], int size)
{
    const string keyword = toLowerCopy(getRequiredText("Enter a topic keyword: "));
    bool foundMatch = false;

    for (int index = 0; index < size; ++index)
    {
        if (!speakers[index].hasData)
        {
            continue;
        }

        if (toLowerCopy(speakers[index].speakingTopic).find(keyword) != string::npos)
        {
            if (!foundMatch)
            {
                cout << "Matching speakers:\n";
            }

            foundMatch = true;
            cout << "Slot " << (index + 1)
                 << ": " << speakers[index].name
                 << ", " << speakers[index].telephoneNumber
                 << ", Topic: " << speakers[index].speakingTopic
                 << ", Fee: $" << fixed << setprecision(2) << speakers[index].feeRequired << '\n';
        }
    }

    if (!foundMatch)
    {
        cout << "No speakers matched that topic keyword.\n";
    }
}