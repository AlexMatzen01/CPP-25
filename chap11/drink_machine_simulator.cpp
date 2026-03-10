// drink_machine_simulator accepts no arguments.
// it simulates a drink machine that sells soft drinks until the user quits.
// it outputs the drink menu, change due, inventory updates, and total earnings.

#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

using namespace std;

struct Drink
{
    string name;
    int costInCents;
    int drinksInMachine;
};

const int DRINK_COUNT = 5;

void displayDrinkMenu(const Drink drinks[], int size);
int getDrinkChoice(int size);
double getInsertedAmount();
double centsToDollars(int cents);

int main()
{
    Drink drinks[DRINK_COUNT] = {
        {"Cola", 75, 20},
        {"Root Beer", 75, 20},
        {"Lemon-Lime", 75, 20},
        {"Grape Soda", 80, 20},
        {"Cream Soda", 80, 20}};

    int totalMoneyEarnedInCents = 0;
    int drinkChoice = 0;

    do
    {
        displayDrinkMenu(drinks, DRINK_COUNT);
        drinkChoice = getDrinkChoice(DRINK_COUNT);

        if (drinkChoice == 0)
        {
            break;
        }

        const int drinkIndex = drinkChoice - 1;

        if (drinks[drinkIndex].drinksInMachine == 0)
        {
            cout << "That drink is sold out.\n\n";
            continue;
        }

        const double insertedAmount = getInsertedAmount();
        const int insertedAmountInCents = static_cast<int>(insertedAmount * 100 + 0.5);

        if (insertedAmountInCents < drinks[drinkIndex].costInCents)
        {
            cout << "Not enough money inserted. Returning $"
                 << fixed << setprecision(2) << insertedAmount << ".\n\n";
            continue;
        }

        const int changeInCents = insertedAmountInCents - drinks[drinkIndex].costInCents;
        --drinks[drinkIndex].drinksInMachine;
        totalMoneyEarnedInCents += drinks[drinkIndex].costInCents;

        cout << "Dispensing " << drinks[drinkIndex].name << ".\n";
        cout << "Change returned: $" << fixed << setprecision(2) << centsToDollars(changeInCents) << ".\n\n";
    } while (true);

    cout << "Total money earned: $" << fixed << setprecision(2)
         << centsToDollars(totalMoneyEarnedInCents) << "\n";

    return 0;
}

void displayDrinkMenu(const Drink drinks[], int size)
{
    cout << "Drink Machine Menu\n";

    for (int index = 0; index < size; ++index)
    {
        cout << (index + 1) << ". " << left << setw(12) << drinks[index].name
             << "$" << fixed << setprecision(2) << centsToDollars(drinks[index].costInCents)
             << "  Remaining: " << drinks[index].drinksInMachine << '\n';
    }

    cout << "0. Quit\n";
}

int getDrinkChoice(int size)
{
    int drinkChoice = 0;

    while (true)
    {
        cout << "Choose a drink (0-" << size << "): ";

        if (cin >> drinkChoice && drinkChoice >= 0 && drinkChoice <= size)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return drinkChoice;
        }

        cout << "Enter a number from 0 to " << size << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double getInsertedAmount()
{
    double insertedAmount = 0.0;

    while (true)
    {
        cout << "Enter money amount (0.00 to 1.00): $";

        if (cin >> insertedAmount && insertedAmount >= 0.0 && insertedAmount <= 1.0)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return insertedAmount;
        }

        cout << "Enter an amount between $0.00 and $1.00.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double centsToDollars(int cents)
{
    return cents / 100.0;
}