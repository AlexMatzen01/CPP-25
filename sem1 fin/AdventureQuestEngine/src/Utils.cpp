//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Utility functions for RNG, strings, and input validation

#include "../include/Utils.h"
#include "../include/Constants.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>

namespace AQE {

// ---
// COLOR UTILITIES
// ---

std::string colorize(const std::string& text, const std::string& colorCode) {
    return colorCode + text + Color::RESET;
}

void printColor(const std::string& text, const std::string& colorCode, bool newLine) {
    std::cout << colorize(text, colorCode);
    if (newLine) {
        std::cout << std::endl;
    }
}

// ---
// RANDOM NUMBER GENERATOR (Static instance)
// ---

static std::mt19937 rng;
static bool rngInitialized = false;

void initRandom(unsigned int seed) {
    if (seed == 0) {
        // Use high-resolution clock for truly random seed
        auto now = std::chrono::high_resolution_clock::now();
        seed = static_cast<unsigned int>(now.time_since_epoch().count());
    }
    rng.seed(seed);
    rngInitialized = true;
}

int randomInt(int min, int max) {
    if (!rngInitialized) {
        initRandom(0);
    }
    if (min > max) std::swap(min, max);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

float randomFloat() {
    if (!rngInitialized) {
        initRandom(0);
    }
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng);
}

bool randomChance(int percentChance) {
    if (percentChance <= 0) return false;
    if (percentChance >= 100) return true;
    return randomInt(1, 100) <= percentChance;
}

// ---
// STRING UTILITIES
// ---

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::string centerString(const std::string& str, int width, char fill) {
    if (static_cast<int>(str.length()) >= width) return str;
    int padding = width - static_cast<int>(str.length());
    int leftPad = padding / 2;
    int rightPad = padding - leftPad;
    return std::string(leftPad, fill) + str + std::string(rightPad, fill);
}

std::string wrapText(const std::string& text, int width) {
    std::stringstream result;
    std::stringstream ss(text);
    std::string word;
    int lineLength = 0;
    
    while (ss >> word) {
        if (lineLength + static_cast<int>(word.length()) + 1 > width) {
            result << "\n";
            lineLength = 0;
        } else if (lineLength > 0) {
            result << " ";
            lineLength++;
        }
        result << word;
        lineLength += static_cast<int>(word.length());
    }
    return result.str();
}

// ---
// INPUT VALIDATION
// ---

int getValidatedInt(const std::string& prompt, int min, int max) {
    int value;
    bool valid = false;
    
    while (!valid) {
        std::cout << prompt;
        
        if (std::cin >> value) {
            if (value >= min && value <= max) {
                valid = true;
            } else {
                std::cout << "Please enter a number between " << min 
                          << " and " << max << ".\n";
            }
        } else {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }
    }
    std::cin.ignore(10000, '\n');  // Clear remaining input
    return value;
}

bool getYesNo(const std::string& prompt) {
    std::string input;
    while (true) {
        std::cout << prompt << " (y/n): ";
        std::getline(std::cin, input);
        input = toLower(trim(input));
        
        if (input == "y" || input == "yes") return true;
        if (input == "n" || input == "no") return false;
        
        std::cout << "Please enter 'y' or 'n'.\n";
    }
}

std::string getValidatedString(const std::string& prompt, int minLength, int maxLength) {
    std::string input;
    
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        input = trim(input);
        
        int len = static_cast<int>(input.length());
        if (minLength > 0 && len < minLength) {
            std::cout << "Input must be at least " << minLength << " characters.\n";
            continue;
        }
        if (maxLength > 0 && len > maxLength) {
            std::cout << "Input must be at most " << maxLength << " characters.\n";
            continue;
        }
        break;
    }
    return input;
}

int displayMenu(const std::string& title, const std::vector<std::string>& options) {
    std::cout << "\n";
    displaySeparator(50, '-');
    std::cout << centerString(title, 50) << "\n";
    displaySeparator(50, '-');
    
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << options[i] << "\n";
    }
    displaySeparator(50, '-');
    
    return getValidatedInt("Enter your choice: ", 1, static_cast<int>(options.size()));
}

// ---
// DISPLAY UTILITIES
// ---

void clearScreen() {
    // Cross-platform clear screen
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause(const std::string& message) {
    std::cout << "\n" << message;
    std::cin.get();
}

void displayProgressBar(int current, int max, int width, const std::string& label) {
    if (max <= 0) max = 1;
    if (current < 0) current = 0;
    if (current > max) current = max;
    
    float ratio = static_cast<float>(current) / max;
    int filled = static_cast<int>(ratio * width);
    
    std::cout << label;
    if (!label.empty()) std::cout << " ";
    std::cout << "[";
    
    for (int i = 0; i < width; ++i) {
        if (i < filled) {
            std::cout << "█";
        } else {
            std::cout << "░";
        }
    }
    std::cout << "] " << current << "/" << max << "\n";
}

void displaySeparator(int width, char character) {
    std::cout << std::string(width, character) << "\n";
}

void displayColored(const std::string& text, int colorCode) {
    // ANSI color codes (may not work on all terminals)
    std::cout << "\033[" << colorCode << "m" << text << "\033[0m";
}

void delay(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// ---
// TYPE CONVERSION UTILITIES
// ---

std::string rarityToString(ItemRarity rarity) {
    switch (rarity) {
        case ItemRarity::COMMON:    return "Common";
        case ItemRarity::UNCOMMON:  return "Uncommon";
        case ItemRarity::RARE:      return "Rare";
        case ItemRarity::EPIC:      return "Epic";
        case ItemRarity::LEGENDARY: return "Legendary";
        default:                    return "Unknown";
    }
}

std::string itemTypeToString(ItemType type) {
    switch (type) {
        case ItemType::CONSUMABLE: return "Consumable";
        case ItemType::WEAPON:     return "Weapon";
        case ItemType::ARMOR:      return "Armor";
        case ItemType::ACCESSORY:  return "Accessory";
        case ItemType::KEY:        return "Key";
        case ItemType::QUEST:      return "Quest Item";
        case ItemType::SPECIAL:    return "Special";
        default:                   return "Unknown";
    }
}

std::string roomTypeToString(RoomType type) {
    switch (type) {
        case RoomType::NORMAL:   return "Normal";
        case RoomType::TREASURE: return "Treasure Room";
        case RoomType::BOSS:     return "Boss Room";
        case RoomType::TRAP:     return "Trap Room";
        case RoomType::PUZZLE:   return "Puzzle Room";
        case RoomType::SECRET:   return "Secret Room";
        case RoomType::SAFE:     return "Safe Haven";
        case RoomType::ENTRANCE: return "Entrance";
        case RoomType::EXIT:     return "Exit";
        default:                 return "Unknown";
    }
}

std::string classToString(CharacterClass charClass) {
    switch (charClass) {
        case CharacterClass::WARRIOR: return "Warrior";
        case CharacterClass::MAGE:    return "Mage";
        case CharacterClass::ROGUE:   return "Rogue";
        case CharacterClass::PALADIN: return "Paladin";
        default:                      return "Unknown";
    }
}

std::string questStatusToString(QuestStatus status) {
    switch (status) {
        case QuestStatus::NOT_STARTED: return "Not Started";
        case QuestStatus::IN_PROGRESS: return "In Progress";
        case QuestStatus::COMPLETED:   return "Completed";
        case QuestStatus::FAILED:      return "Failed";
        default:                       return "Unknown";
    }
}

std::string endingToString(EndingType ending) {
    switch (ending) {
        case EndingType::VICTORY_NORMAL:      return "Victory - Normal Ending";
        case EndingType::VICTORY_SECRET:      return "Victory - Secret Ending";
        case EndingType::VICTORY_SPEEDRUN:    return "Victory - Speedrun Ending";
        case EndingType::DEFEAT_DEATH:        return "Defeat - Death in Combat";
        case EndingType::DEFEAT_TRAP:         return "Defeat - Killed by Trap";
        case EndingType::ENDING_EVIL:         return "Dark Lord Ending";
        case EndingType::ENDING_HERO:         return "True Hero Ending";
        case EndingType::ENDING_NEUTRAL:      return "Wanderer Ending";
        case EndingType::ENDING_PACIFIST:     return "Pacifist Ending";
        case EndingType::ENDING_COMPLETIONIST:return "Completionist Ending";
        default:                              return "Unknown Ending";
    }
}

// ---
// GAME CALCULATIONS
// ---

int calculateDamage(int rawDamage, int defense) {
    int damage = rawDamage - defense;
    return (damage < 1) ? 1 : damage;  // Minimum 1 damage
}

int calculateExpForLevel(int level) {
    if (level <= 1) return 0;
    float exp = BASE_XP_TO_LEVEL;
    for (int i = 2; i <= level; ++i) {
        exp *= XP_MULTIPLIER_PER_LEVEL;
    }
    return static_cast<int>(exp);
}

int calculateSellValue(const Item& item) {
    int baseValue = static_cast<int>(item.value * SELL_PRICE_MULTIPLIER);
    return (baseValue < 1) ? 1 : baseValue;
}

int calculateScore(const Player& player) {
    int score = 0;
    
    // Base score from survival
    score += player.totalKills * 10;
    score += player.secretsFound * 50;
    score += player.puzzlesSolved * 25;
    score += player.bossesDefeated * 100;
    score += static_cast<int>(player.completedQuests.size()) * 50;
    score += player.gold;
    
    // Bonus for efficiency
    if (player.totalTurns > 0) {
        score += 5000 / player.totalTurns;  // Faster = more points
    }
    
    // Bonus for health remaining
    score += player.stats.currentHP;
    
    // Level bonus
    score += player.level * 100;
    
    return score;
}

}  // namespace AQE
