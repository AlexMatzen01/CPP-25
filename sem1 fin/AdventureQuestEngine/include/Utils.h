





#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <random>
#include "Types.h"
#include "Constants.h"

namespace AQE {






std::string colorize(const std::string& text, const std::string& colorCode);


void printColor(const std::string& text, const std::string& colorCode, bool newLine = true);






void initRandom(unsigned int seed = 0);


int randomInt(int min, int max);


float randomFloat();


bool randomChance(int percentChance);


template<typename T>
const T& randomElement(const std::vector<T>& vec);






std::string toLower(const std::string& str);


std::string toUpper(const std::string& str);


std::string trim(const std::string& str);


std::vector<std::string> split(const std::string& str, char delimiter);


std::string centerString(const std::string& str, int width, char fill = ' ');


std::string wrapText(const std::string& text, int width);






int getValidatedInt(const std::string& prompt, int min, int max);


bool getYesNo(const std::string& prompt);


std::string getValidatedString(const std::string& prompt, int minLength = 1, int maxLength = 50);


int displayMenu(const std::string& title, const std::vector<std::string>& options);






void clearScreen();


void pause(const std::string& message = "Press Enter to continue...");


void displayProgressBar(int current, int max, int width = 20, const std::string& label = "");


void displaySeparator(int width = 80, char character = '=');


void displayColored(const std::string& text, int colorCode);


void delay(int milliseconds);






std::string rarityToString(ItemRarity rarity);


std::string itemTypeToString(ItemType type);


std::string roomTypeToString(RoomType type);


std::string classToString(CharacterClass charClass);


std::string questStatusToString(QuestStatus status);


std::string endingToString(EndingType ending);






int calculateDamage(int rawDamage, int defense);


int calculateExpForLevel(int level);


int estimateEnemyPowerLevel(const Enemy& enemy);


int rollItemLevel(int contextLevel, ItemRarity rarity, bool isBoss = false);


int getItemLevelTier(int itemLevel);


std::string itemLevelTierToString(int itemLevel);


void applyItemLevelScaling(Item& item, int targetItemLevel, bool rollPerk = true);


int estimateItemLevelFromLegacyStats(const Item& item);


void rebuildItemStatsFromLevel(Item& item);


int calculateSellValue(const Item& item);


int calculateScore(const Player& player);


int calculateAverageItemLevel(const Player& player);


void refreshAverageItemLevel(Player& player);





template<typename T>
const T& randomElement(const std::vector<T>& vec) {
    if (vec.empty()) {
        static T defaultValue;
        return defaultValue;
    }
    return vec[randomInt(0, static_cast<int>(vec.size()) - 1)];
}

}  

#endif 
