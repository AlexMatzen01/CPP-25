





#include "../include/Utils.h"
#include "../include/Constants.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace AQE {





std::string colorize(const std::string& text, const std::string& colorCode) {
    return colorCode + text + Color::RESET;
}

void printColor(const std::string& text, const std::string& colorCode, bool newLine) {
    std::cout << colorize(text, colorCode);
    if (newLine) {
        std::cout << std::endl;
    }
}





static std::mt19937 rng;
static bool rngInitialized = false;

void initRandom(unsigned int seed) {
    if (seed == 0) {
        
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
    std::cin.ignore(10000, '\n');  
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





void clearScreen() {
    
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause(const std::string& message) {
    std::cout << "\n" << message;
    std::cout << std::flush;

    
#ifdef _WIN32
    
    int ch;
    while (true) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') break;
        
    }
#else
    struct termios oldt, newt;
    if (tcgetattr(STDIN_FILENO, &oldt) == 0) {
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        int c;
        
        while ((c = getchar()) != '\n' && c != EOF) {
            
        }

        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    } else {
        
        std::string tmp;
        std::getline(std::cin, tmp);
    }
#endif
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
    
    std::cout << "\033[" << colorCode << "m" << text << "\033[0m";
}

void delay(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}





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
        case RoomType::SHOP:     return "Shop";
        case RoomType::ENTRANCE: return "Entrance";
        case RoomType::EXIT:     return "Exit";
        case RoomType::SHADOWLORD: return "Shadow Lord's Chamber";
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





int calculateDamage(int rawDamage, int defense) {
    int damage = rawDamage - defense;
    return (damage < 1) ? 1 : damage;  
}

int calculateExpForLevel(int level) {
    if (level <= 1) return 0;
    float exp = BASE_XP_TO_LEVEL;
    for (int i = 2; i <= level; ++i) {
        exp *= XP_MULTIPLIER_PER_LEVEL;
    }
    return static_cast<int>(exp);
}

namespace {

int rarityRank(ItemRarity rarity) {
    switch (rarity) {
        case ItemRarity::COMMON:    return 1;
        case ItemRarity::UNCOMMON:  return 2;
        case ItemRarity::RARE:      return 3;
        case ItemRarity::EPIC:      return 4;
        case ItemRarity::LEGENDARY: return 5;
        default:                    return 1;
    }
}

void grantTierStatBonus(Item& item, int tier) {
    const int magnitude = (tier <= 2) ? 2 : (tier == 3 ? 4 : 6);

    switch (item.type) {
        case ItemType::WEAPON: {
            int roll = randomInt(1, 100);
            if (roll <= 55) {
                item.attackBonus += magnitude;
            } else if (roll <= 85) {
                item.critBonus += std::max(1, magnitude / 2);
            } else {
                item.speedBonus += std::max(1, magnitude / 3);
            }
            break;
        }
        case ItemType::ARMOR: {
            int roll = randomInt(1, 100);
            if (roll <= 65) {
                item.defenseBonus += magnitude;
            } else {
                item.dodgeBonus += std::max(1, magnitude / 2);
            }
            break;
        }
        case ItemType::ACCESSORY: {
            int roll = randomInt(1, 5);
            if (roll == 1) item.attackBonus += magnitude;
            else if (roll == 2) item.defenseBonus += magnitude;
            else if (roll == 3) item.critBonus += std::max(1, magnitude);
            else if (roll == 4) item.dodgeBonus += std::max(1, magnitude);
            else item.speedBonus += std::max(1, magnitude / 2);
            break;
        }
        case ItemType::CONSUMABLE: {
            if (item.healAmount > 0) {
                item.healAmount += magnitude * 3;
            }
            if (item.attackBonus > 0) item.attackBonus += std::max(1, magnitude / 2);
            if (item.defenseBonus > 0) item.defenseBonus += std::max(1, magnitude / 2);
            if (item.critBonus > 0) item.critBonus += std::max(1, magnitude / 2);
            if (item.dodgeBonus > 0) item.dodgeBonus += std::max(1, magnitude / 2);
            if (item.speedBonus > 0) item.speedBonus += std::max(1, magnitude / 3);
            break;
        }
        default:
            break;
    }
}

void rollUniquePerk(Item& item, int tier) {
    if (item.hasUniquePerk || tier < 3) {
        return;
    }

    int chance = (tier >= 4) ? 70 : 35;
    chance += (rarityRank(item.rarity) - 1) * 5;
    if (!randomChance(chance)) {
        return;
    }

    item.hasUniquePerk = true;

    switch (item.type) {
        case ItemType::WEAPON: {
            int roll = randomInt(1, 3);
            if (roll == 1) {
                item.uniquePerkName = "Executioner";
                item.attackBonus += 6;
                item.critBonus += 4;
            } else if (roll == 2) {
                item.uniquePerkName = "Tempo";
                item.speedBonus += 3;
                item.critBonus += 3;
            } else {
                item.uniquePerkName = "Blooddrinker";
                item.attackBonus += 4;
                item.healAmount += 8;
            }
            break;
        }
        case ItemType::ARMOR: {
            int roll = randomInt(1, 3);
            if (roll == 1) {
                item.uniquePerkName = "Bulwark";
                item.defenseBonus += 8;
            } else if (roll == 2) {
                item.uniquePerkName = "Warden";
                item.defenseBonus += 5;
                item.dodgeBonus += 5;
            } else {
                item.uniquePerkName = "Juggernaut";
                item.defenseBonus += 6;
                item.speedBonus -= 1;
            }
            break;
        }
        case ItemType::ACCESSORY: {
            int roll = randomInt(1, 3);
            if (roll == 1) {
                item.uniquePerkName = "Fortunebound";
                item.critBonus += 6;
                item.dodgeBonus += 6;
            } else if (roll == 2) {
                item.uniquePerkName = "Arcane Pulse";
                item.attackBonus += 4;
                item.speedBonus += 4;
            } else {
                item.uniquePerkName = "Guardian Sigil";
                item.defenseBonus += 5;
                item.dodgeBonus += 4;
            }
            break;
        }
        case ItemType::CONSUMABLE: {
            item.uniquePerkName = "Distilled";
            if (item.healAmount > 0) {
                item.healAmount = static_cast<int>(item.healAmount * 1.4f);
            }
            break;
        }
        default:
            item.uniquePerkName = "Empowered";
            break;
    }
}

}  

int estimateEnemyPowerLevel(const Enemy& enemy) {
    const CombatStats& stats = enemy.stats;
    const int offense = stats.attack + (stats.magicPower / 2);
    const int defense = stats.defense + (stats.magicResist / 2);
    const int vitality = std::max(1, stats.maxHP / 12);
    const int mobility = std::max(1, stats.speed / 2);

    int estimated = (offense + defense + vitality + mobility) / 6;
    if (estimated < 1) {
        estimated = 1;
    }

    if (enemy.isBoss) {
        estimated += 4;
    }

    return estimated;
}

int rollItemLevel(int contextLevel, ItemRarity rarity, bool isBoss) {
    const int normalizedContext = std::max(1, std::min(contextLevel, MAX_LEVEL));

    
    const int baseline = (normalizedContext * MAX_ITEM_LEVEL + (MAX_LEVEL / 2)) / MAX_LEVEL;

    int rarityOffset = 0;
    switch (rarity) {
        case ItemRarity::COMMON: rarityOffset = -2; break;
        case ItemRarity::UNCOMMON: rarityOffset = 0; break;
        case ItemRarity::RARE: rarityOffset = 2; break;
        case ItemRarity::EPIC: rarityOffset = 4; break;
        case ItemRarity::LEGENDARY: rarityOffset = 6; break;
    }

    int center = baseline + rarityOffset + (isBoss ? 4 : 0);
    center = std::max(1, std::min(center, MAX_ITEM_LEVEL));

    const int spread = 5 + std::min(3, normalizedContext / 20);
    int minLevel = center - spread;
    int maxLevel = center + spread;

    
    
    const int powerFloor = std::max(1, baseline - 8);
    int powerCeil = baseline + 10 + (isBoss ? 3 : 0);
    
    
    if (normalizedContext <= 5) {
        powerCeil = baseline + 2;  
    } else if (normalizedContext <= 10) {
        powerCeil = baseline + 4;  
    } else if (normalizedContext <= 20) {
        powerCeil = baseline + 6;  
    }
    

    minLevel = std::max(minLevel, powerFloor);
    maxLevel = std::min(maxLevel, powerCeil);

    if (minLevel > maxLevel) {
        minLevel = std::max(1, std::min(powerFloor, MAX_ITEM_LEVEL));
        maxLevel = std::max(minLevel, std::min(powerCeil, MAX_ITEM_LEVEL));
    }

    return randomInt(minLevel, maxLevel);
}

int getItemLevelTier(int itemLevel) {
    if (itemLevel < 30) return 1;
    if (itemLevel < 60) return 2;
    if (itemLevel < 90) return 3;
    return 4;
}

std::string itemLevelTierToString(int itemLevel) {
    switch (getItemLevelTier(itemLevel)) {
        case 1: return "Tier I";
        case 2: return "Tier II";
        case 3: return "Tier III";
        case 4: return "Tier IV";
        default: return "Tier I";
    }
}

void applyItemLevelScaling(Item& item, int targetItemLevel, bool rollPerk) {
    const int currentLevel = std::max(1, item.itemLevel);
    const int desiredLevel = std::max(1, targetItemLevel);

    if (desiredLevel <= currentLevel) {
        item.itemLevel = currentLevel;
        if (item.value < 1) item.value = 1;
        return;
    }

    const int levelDelta = desiredLevel - currentLevel;
    const int rarityMod = rarityRank(item.rarity);
    const int oldTier = getItemLevelTier(currentLevel);

    switch (item.type) {
        case ItemType::WEAPON: {
            item.attackBonus += std::max(1, (levelDelta * (2 + rarityMod)) / 8);
            if (item.critBonus != 0 || randomChance(45)) {
                item.critBonus += std::max(1, levelDelta / 6);
            }
            if (item.speedBonus != 0 && randomChance(30)) {
                item.speedBonus += std::max(1, levelDelta / 8);
            }
            break;
        }
        case ItemType::ARMOR: {
            item.defenseBonus += std::max(1, (levelDelta * (3 + rarityMod)) / 9);
            if (item.dodgeBonus != 0 || randomChance(40)) {
                item.dodgeBonus += std::max(1, levelDelta / 6);
            }
            if (item.speedBonus < 0 && randomChance(30)) {
                item.speedBonus += 1;
            }
            break;
        }
        case ItemType::ACCESSORY: {
            int statPoints = std::max(1, levelDelta / 4);
            while (statPoints-- > 0) {
                int roll = randomInt(1, 5);
                if (roll == 1) item.attackBonus += 1;
                else if (roll == 2) item.defenseBonus += 1;
                else if (roll == 3) item.critBonus += 1;
                else if (roll == 4) item.dodgeBonus += 1;
                else item.speedBonus += 1;
            }
            break;
        }
        case ItemType::CONSUMABLE: {
            if (item.healAmount > 0) {
                item.healAmount += std::max(2, levelDelta * (1 + rarityMod / 3));
            }
            if (item.attackBonus > 0) item.attackBonus += std::max(1, levelDelta / 4);
            if (item.defenseBonus > 0) item.defenseBonus += std::max(1, levelDelta / 4);
            if (item.critBonus > 0) item.critBonus += std::max(1, levelDelta / 5);
            if (item.dodgeBonus > 0) item.dodgeBonus += std::max(1, levelDelta / 5);
            if (item.speedBonus > 0) item.speedBonus += std::max(1, levelDelta / 6);
            break;
        }
        default:
            break;
    }

    if (item.maxDurability > 0) {
        item.maxDurability += std::max(0, levelDelta / 3);
        item.durability = item.maxDurability;
    }

    item.value += levelDelta * (10 + rarityMod * 3);
    if (item.value < 1) item.value = 1;

    item.itemLevel = desiredLevel;
    const int newTier = getItemLevelTier(desiredLevel);

    if (rollPerk && newTier > oldTier) {
        for (int tier = oldTier + 1; tier <= newTier; ++tier) {
            grantTierStatBonus(item, tier);
        }
    }

    if (rollPerk) {
        rollUniquePerk(item, newTier);
    }
}

int estimateItemLevelFromLegacyStats(const Item& item) {
    int rarityBase = 4;
    switch (item.rarity) {
        case ItemRarity::COMMON: rarityBase = 4; break;
        case ItemRarity::UNCOMMON: rarityBase = 9; break;
        case ItemRarity::RARE: rarityBase = 15; break;
        case ItemRarity::EPIC: rarityBase = 24; break;
        case ItemRarity::LEGENDARY: rarityBase = 34; break;
        default: break;
    }

    int power = 0;
    power += std::max(0, item.attackBonus) * 3;
    power += std::max(0, item.defenseBonus) * 3;
    power += std::max(0, item.healAmount) / 4;
    power += std::max(0, item.critBonus) * 2;
    power += std::max(0, item.dodgeBonus) * 2;
    power += std::max(0, item.speedBonus) * 2;
    power += std::max(0, item.duration) * 2;

    
    power += std::min(0, item.attackBonus) * 2;
    power += std::min(0, item.defenseBonus) * 2;
    power += std::min(0, item.speedBonus) * 2;

    int estimated = rarityBase + std::max(0, power) / 10;
    if (item.type == ItemType::KEY || item.type == ItemType::QUEST ||
        item.type == ItemType::SPECIAL) {
        estimated = std::max(estimated, rarityBase);
    }

    if (estimated < 1) estimated = 1;
    if (estimated > MAX_ITEM_LEVEL) estimated = MAX_ITEM_LEVEL;
    return estimated;
}

void rebuildItemStatsFromLevel(Item& item) {
    item.itemLevel = std::max(1, std::min(item.itemLevel, MAX_ITEM_LEVEL));
    item.quantity = std::max(1, item.quantity);

    const int level = item.itemLevel;
    const int rarity = rarityRank(item.rarity);
    const std::string lowerName = toLower(item.name);

    item.healAmount = 0;
    item.attackBonus = 0;
    item.defenseBonus = 0;
    item.critBonus = 0;
    item.dodgeBonus = 0;
    item.speedBonus = 0;

    auto applyValueFloor = [&](int floorValue) {
        if (item.value < floorValue) {
            item.value = floorValue;
        }
    };

    switch (item.type) {
        case ItemType::WEAPON: {
            item.isUsable = false;
            item.isStackable = false;

            item.attackBonus = std::max(1, ((level * (4 + rarity)) / 5) + std::max(0, rarity - 1));
            item.critBonus = std::max(0, (level * rarity) / 12);

            if (lowerName.find("dagger") != std::string::npos ||
                lowerName.find("fang") != std::string::npos) {
                item.attackBonus = std::max(1, item.attackBonus - std::max(1, level / 8));
                item.critBonus += std::max(1, level / 3);
                item.speedBonus += std::max(1, level / 12);
            } else if (lowerName.find("axe") != std::string::npos) {
                item.attackBonus += std::max(1, level / 4);
                item.speedBonus -= std::max(1, level / 16);
            } else if (lowerName.find("staff") != std::string::npos ||
                       lowerName.find("wand") != std::string::npos) {
                item.critBonus += std::max(1, level / 5);
                item.defenseBonus += std::max(0, level / 16);
            } else if (lowerName.find("spear") != std::string::npos ||
                       lowerName.find("pike") != std::string::npos ||
                       lowerName.find("halberd") != std::string::npos) {
                item.defenseBonus += std::max(1, level / 10);
            } else if (lowerName.find("sword") != std::string::npos ||
                       lowerName.find("blade") != std::string::npos) {
                item.critBonus += std::max(1, level / 7);
            }

            item.maxDurability = std::max(item.maxDurability, 100 + (level * 2));
            item.durability = std::max(item.durability, item.maxDurability);
            applyValueFloor(15 + (level * (6 + rarity * 3)));
            break;
        }
        case ItemType::ARMOR: {
            item.isUsable = false;
            item.isStackable = false;

            item.defenseBonus = std::max(1, ((level * (3 + rarity)) / 6) + std::max(0, rarity - 1));
            item.dodgeBonus = std::max(0, level / 16);

            if (lowerName.find("shield") != std::string::npos) {
                item.defenseBonus += std::max(1, level / 4);
                item.speedBonus -= std::max(0, level / 18);
            }

            if (lowerName.find("robe") != std::string::npos ||
                lowerName.find("cloak") != std::string::npos ||
                lowerName.find("leather") != std::string::npos ||
                lowerName.find("hide") != std::string::npos) {
                item.dodgeBonus += std::max(1, level / 10) + std::max(0, rarity - 1);
                item.speedBonus += std::max(0, level / 20);
            }

            if (lowerName.find("plate") != std::string::npos ||
                lowerName.find("mail") != std::string::npos ||
                lowerName.find("armor") != std::string::npos ||
                lowerName.find("steel") != std::string::npos) {
                item.speedBonus -= std::max(0, (level / 14) - 1);
            }

            item.maxDurability = std::max(item.maxDurability, 120 + (level * 3));
            item.durability = std::max(item.durability, item.maxDurability);
            applyValueFloor(15 + (level * (7 + rarity * 3)));
            break;
        }
        case ItemType::ACCESSORY: {
            item.isUsable = false;
            item.isStackable = false;

            const int budget = std::max(2, (level * (1 + rarity)) / 6);

            if (lowerName.find("swiftness") != std::string::npos) {
                item.speedBonus = std::max(2, budget / 2);
                item.dodgeBonus = std::max(1, budget - item.speedBonus);
            } else if (lowerName.find("fortune") != std::string::npos) {
                item.critBonus = std::max(2, budget);
                item.attackBonus = std::max(1, budget / 4);
            } else if (lowerName.find("might") != std::string::npos) {
                item.attackBonus = std::max(2, budget);
                item.critBonus = std::max(1, budget / 4);
            } else if (lowerName.find("protection") != std::string::npos ||
                       lowerName.find("iron skin") != std::string::npos) {
                item.defenseBonus = std::max(2, budget);
                item.dodgeBonus = std::max(1, budget / 5);
                if (lowerName.find("iron skin") != std::string::npos) {
                    item.speedBonus -= std::max(1, level / 12);
                }
            } else if (lowerName.find("vitality") != std::string::npos) {
                item.defenseBonus = std::max(2, (budget * 2) / 3);
                item.dodgeBonus = std::max(1, budget / 3);
            } else if (lowerName.find("shadow") != std::string::npos) {
                item.dodgeBonus = std::max(2, (budget * 2) / 3);
                item.speedBonus = std::max(1, budget / 3);
                item.defenseBonus = std::max(1, budget / 6);
            } else if (lowerName.find("dragon") != std::string::npos) {
                item.attackBonus = std::max(2, budget / 2);
                item.defenseBonus = std::max(2, budget / 3);
                item.critBonus = std::max(1, budget - item.attackBonus - item.defenseBonus);
                item.speedBonus -= std::max(1, level / 14);
            } else if (lowerName.find("assassin") != std::string::npos) {
                item.critBonus = std::max(2, budget / 2);
                item.speedBonus = std::max(1, budget / 3);
                item.attackBonus = std::max(1, budget - item.critBonus - item.speedBonus);
            } else if (lowerName.find("wisdom") != std::string::npos) {
                item.attackBonus = std::max(1, budget / 3);
                item.defenseBonus = std::max(1, budget / 3);
                item.critBonus = std::max(1, budget - item.attackBonus - item.defenseBonus);
            } else if (lowerName.find("vile black hand") != std::string::npos) {
                item.attackBonus = std::max(2, budget + std::max(1, rarity));
                item.critBonus = std::max(1, budget / 3);
                item.defenseBonus = -std::max(1, level / 10);
            } else {
                const int profile = std::abs(item.id) % 4;
                if (profile == 0) {
                    item.attackBonus = std::max(2, budget / 2);
                    item.critBonus = std::max(1, budget - item.attackBonus);
                } else if (profile == 1) {
                    item.defenseBonus = std::max(2, budget / 2);
                    item.dodgeBonus = std::max(1, budget - item.defenseBonus);
                } else if (profile == 2) {
                    item.speedBonus = std::max(1, budget / 3);
                    item.dodgeBonus = std::max(1, budget / 2);
                    item.critBonus = std::max(1, budget - item.speedBonus - item.dodgeBonus);
                } else {
                    item.attackBonus = std::max(1, budget / 3);
                    item.defenseBonus = std::max(1, budget / 3);
                    item.critBonus = std::max(1, budget - item.attackBonus - item.defenseBonus);
                }
            }

            applyValueFloor(20 + (level * (7 + rarity * 3)));
            break;
        }
        case ItemType::CONSUMABLE: {
            item.isUsable = true;

            if (lowerName.find("resurrection") != std::string::npos) {
                item.isStackable = false;
                applyValueFloor(120 + (level * (8 + rarity * 3)));
                break;
            }

            if (item.duration > 0) {
                item.isStackable = false;

                if (lowerName.find("berserker") != std::string::npos) {
                    item.attackBonus = std::max(2, (level * (2 + rarity)) / 4);
                    item.critBonus = std::max(1, level / 8);
                    item.defenseBonus = -std::max(1, level / 8);
                } else if (lowerName.find("strength") != std::string::npos) {
                    item.attackBonus = std::max(2, (level * (2 + rarity)) / 5);
                    item.critBonus = std::max(1, level / 12);
                } else if (lowerName.find("defense") != std::string::npos) {
                    item.defenseBonus = std::max(2, (level * (2 + rarity)) / 5);
                } else if (lowerName.find("agility") != std::string::npos ||
                           lowerName.find("invis") != std::string::npos) {
                    item.dodgeBonus = std::max(2, (level * (2 + rarity)) / 5);
                    item.speedBonus = std::max(1, level / 10);
                } else {
                    item.attackBonus = std::max(1, level / 8);
                    item.defenseBonus = std::max(1, level / 8);
                }
            } else {
                item.isStackable = true;
                item.healAmount = std::min(999, 12 + (level * (6 + rarity * 2)));
            }

            applyValueFloor(8 + (level * (4 + rarity * 2)));
            break;
        }
        case ItemType::KEY:
        case ItemType::QUEST:
        case ItemType::SPECIAL:
        default:
            break;
    }
}

int calculateSellValue(const Item& item) {
    int baseValue = item.value + ((std::max(1, item.itemLevel) - 1) * 3);
    if (item.hasUniquePerk) {
        baseValue += 120;
    }
    baseValue = static_cast<int>(baseValue * SELL_PRICE_MULTIPLIER);
    return (baseValue < 1) ? 1 : baseValue;
}

int calculateScore(const Player& player) {
    int score = 0;
    
    
    score += player.totalKills * 10;
    score += player.secretsFound * 50;
    score += player.puzzlesSolved * 25;
    score += player.bossesDefeated * 100;
    score += static_cast<int>(player.completedQuests.size()) * 50;
    score += player.gold;
    
    
    if (player.totalTurns > 0) {
        score += 5000 / player.totalTurns;  
    }
    
    
    score += player.stats.currentHP;
    
    
    score += player.level * 100;
    
    return score;
}

int calculateAverageItemLevel(const Player& player) {
    int totalItemLevel = 0;
    int equippedCount = 0;

    for (const Item& item : player.inventory) {
        if (!item.isEquipped) {
            continue;
        }

        if (item.type != ItemType::WEAPON &&
            item.type != ItemType::ARMOR &&
            item.type != ItemType::ACCESSORY) {
            continue;
        }

        totalItemLevel += std::max(1, item.itemLevel);
        ++equippedCount;
    }

    if (equippedCount == 0) {
        return 0;
    }

    return (totalItemLevel + (equippedCount / 2)) / equippedCount;
}

void refreshAverageItemLevel(Player& player) {
    player.averageItemLevel = calculateAverageItemLevel(player);
}

}  
