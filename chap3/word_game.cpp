/*
#word_game accepts 0 arguments
#it asks the user for a name, age, city, college, profession, animal, and pet name
#it outputs a silly story inserting the user’s inputs (Mad Libs style)
*/

#include <iostream>
#include <string>
#include <limits>

int main() {
    std::string name, city, college, profession, animal, petName;
    int age = 0;

    std::cout << "Enter a name: ";
    std::getline(std::cin, name);

    std::cout << "Enter an age: ";
    std::cin >> age;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter the name of a city: ";
    std::getline(std::cin, city);

    std::cout << "Enter the name of a college: ";
    std::getline(std::cin, college);

    std::cout << "Enter a profession: ";
    std::getline(std::cin, profession);

    std::cout << "Enter a type of animal: ";
    std::getline(std::cin, animal);

    std::cout << "Enter a pet's name: ";
    std::getline(std::cin, petName);

    // Determine if 'a' or 'an' should be used
    char firstChar = !animal.empty() ? std::tolower(animal[0]) : 'a';
    std::string article = (firstChar == 'a' || firstChar == 'e' || firstChar == 'i' || firstChar == 'o' || firstChar == 'u') ? "an" : "a";

    std::cout << "\nThere once was a person named " << name << " who lived in " << city << ". "
        << "At the age of " << age << " " << name << " went to college at " << college << ". "
        << name << " graduated and went to work as a " << profession << ". "
        << "Then, " << name << " adopted " << article << " " << animal << " named " << petName << ". "
        << "They both lived happily ever after.\n";
    return 0;
}
