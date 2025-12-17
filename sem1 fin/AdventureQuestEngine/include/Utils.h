/*******************************************************************************
 * C++ Adventure Quest Engine
 * Utils.h - Utility Functions Header
 * 
 * This file contains utility function declarations for random number generation,
 * string manipulation, input validation, and helper functions.
 * 
 * Author: Adventure Quest Engine Team
 * Version: 1.0
 ******************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <random>
#include "Types.h"
#include "Constants.h"

namespace AQE {

//==============================================================================
// COLOR UTILITIES
//==============================================================================

/**
 * @brief Wraps text in ANSI color codes
 * @param text The text to colorize
 * @param colorCode The ANSI color code string
 * @return String with color codes applied
 */
std::string colorize(const std::string& text, const std::string& colorCode);

/**
 * @brief Prints colored text to stdout
 * @param text The text to print
 * @param colorCode The ANSI color code string
 * @param newLine Whether to append a newline
 */
void printColor(const std::string& text, const std::string& colorCode, bool newLine = true);

//==============================================================================
// RANDOM NUMBER GENERATION
//==============================================================================

/**
 * @brief Initialize the random number generator with a seed
 * @param seed The seed value (use 0 for time-based seed)
 */
void initRandom(unsigned int seed = 0);

/**
 * @brief Get a random integer in range [min, max] inclusive
 * @param min Minimum value
 * @param max Maximum value
 * @return Random integer in the specified range
 */
int randomInt(int min, int max);

/**
 * @brief Get a random float in range [0.0, 1.0]
 * @return Random float between 0 and 1
 */
float randomFloat();

/**
 * @brief Check if a random event occurs based on percentage chance
 * @param percentChance Chance of occurring (0-100)
 * @return True if event occurs, false otherwise
 */
bool randomChance(int percentChance);

/**
 * @brief Get a random element from a vector
 * @param vec The vector to choose from
 * @return Reference to a random element
 */
template<typename T>
const T& randomElement(const std::vector<T>& vec);

//==============================================================================
// STRING UTILITIES
//==============================================================================

/**
 * @brief Convert a string to lowercase
 * @param str The string to convert
 * @return Lowercase version of the string
 */
std::string toLower(const std::string& str);

/**
 * @brief Convert a string to uppercase
 * @param str The string to convert
 * @return Uppercase version of the string
 */
std::string toUpper(const std::string& str);

/**
 * @brief Trim whitespace from both ends of a string
 * @param str The string to trim
 * @return Trimmed string
 */
std::string trim(const std::string& str);

/**
 * @brief Split a string by delimiter
 * @param str The string to split
 * @param delimiter The delimiter character
 * @return Vector of split strings
 */
std::vector<std::string> split(const std::string& str, char delimiter);

/**
 * @brief Center a string within a specified width
 * @param str The string to center
 * @param width The total width
 * @param fill The fill character (default space)
 * @return Centered string
 */
std::string centerString(const std::string& str, int width, char fill = ' ');

/**
 * @brief Wrap text to a specified width
 * @param text The text to wrap
 * @param width Maximum line width
 * @return Wrapped text with newlines
 */
std::string wrapText(const std::string& text, int width);

//==============================================================================
// INPUT VALIDATION
//==============================================================================

/**
 * @brief Get a validated integer input from user
 * @param prompt The prompt to display
 * @param min Minimum valid value
 * @param max Maximum valid value
 * @return Valid integer input
 */
int getValidatedInt(const std::string& prompt, int min, int max);

/**
 * @brief Get a yes/no response from user
 * @param prompt The prompt to display
 * @return True for yes, false for no
 */
bool getYesNo(const std::string& prompt);

/**
 * @brief Get a string input from user with optional validation
 * @param prompt The prompt to display
 * @param minLength Minimum string length (0 for no minimum)
 * @param maxLength Maximum string length (0 for no maximum)
 * @return Valid string input
 */
std::string getValidatedString(const std::string& prompt, int minLength = 1, int maxLength = 50);

/**
 * @brief Display a menu and get a validated choice
 * @param title Menu title
 * @param options Vector of option strings
 * @return Selected option index (1-based)
 */
int displayMenu(const std::string& title, const std::vector<std::string>& options);

//==============================================================================
// DISPLAY UTILITIES
//==============================================================================

/**
 * @brief Clear the console screen
 */
void clearScreen();

/**
 * @brief Pause and wait for user input
 * @param message Message to display (default "Press Enter to continue...")
 */
void pause(const std::string& message = "Press Enter to continue...");

/**
 * @brief Display a progress bar
 * @param current Current value
 * @param max Maximum value
 * @param width Width of the bar in characters
 * @param label Label to display
 */
void displayProgressBar(int current, int max, int width = 20, const std::string& label = "");

/**
 * @brief Display a horizontal separator line
 * @param width Width of the line
 * @param character The character to use (default '=')
 */
void displaySeparator(int width = 80, char character = '=');

/**
 * @brief Display colored text (uses ANSI codes if supported)
 * @param text The text to display
 * @param colorCode ANSI color code
 */
void displayColored(const std::string& text, int colorCode);

/**
 * @brief Add a delay (for dramatic effect)
 * @param milliseconds Time to wait
 */
void delay(int milliseconds);

//==============================================================================
// TYPE CONVERSION UTILITIES
//==============================================================================

/**
 * @brief Convert ItemRarity enum to string
 * @param rarity The rarity value
 * @return String representation
 */
std::string rarityToString(ItemRarity rarity);

/**
 * @brief Convert ItemType enum to string
 * @param type The item type
 * @return String representation
 */
std::string itemTypeToString(ItemType type);

/**
 * @brief Convert RoomType enum to string
 * @param type The room type
 * @return String representation
 */
std::string roomTypeToString(RoomType type);

/**
 * @brief Convert CharacterClass enum to string
 * @param charClass The character class
 * @return String representation
 */
std::string classToString(CharacterClass charClass);

/**
 * @brief Convert QuestStatus enum to string
 * @param status The quest status
 * @return String representation
 */
std::string questStatusToString(QuestStatus status);

/**
 * @brief Convert EndingType enum to string
 * @param ending The ending type
 * @return String representation
 */
std::string endingToString(EndingType ending);

//==============================================================================
// GAME CALCULATIONS
//==============================================================================

/**
 * @brief Calculate damage after defense reduction
 * @param rawDamage The raw damage value
 * @param defense The defense value
 * @return Final damage (minimum 1)
 */
int calculateDamage(int rawDamage, int defense);

/**
 * @brief Calculate experience needed for a level
 * @param level The level to calculate for
 * @return Experience points required
 */
int calculateExpForLevel(int level);

/**
 * @brief Calculate item sell value
 * @param item The item to sell
 * @return Gold value
 */
int calculateSellValue(const Item& item);

/**
 * @brief Calculate player score for leaderboard
 * @param player The player to score
 * @return Total score
 */
int calculateScore(const Player& player);

//==============================================================================
// TEMPLATE IMPLEMENTATIONS
//==============================================================================

template<typename T>
const T& randomElement(const std::vector<T>& vec) {
    if (vec.empty()) {
        static T defaultValue;
        return defaultValue;
    }
    return vec[randomInt(0, static_cast<int>(vec.size()) - 1)];
}

}  // namespace AQE

#endif // UTILS_H
