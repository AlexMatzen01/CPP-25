//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Main game engine implementation

#include "../include/GameEngine.h"
#include "../include/Constants.h"
#include "../include/UIEnhanced.h"
#include "../include/Magic.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cmath>
#include <cctype>

namespace AQE {

namespace {

std::string getFileNameOnly(const std::string& path) {
    const size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

std::string getSaveDisplayName(const std::string& path) {
    std::string name = getFileNameOnly(path);
    const std::string prefix = "save_";
    const std::string suffix = ".txt";

    if (name.rfind(prefix, 0) == 0 && name.size() > prefix.size() + suffix.size() &&
        name.substr(name.size() - suffix.size()) == suffix) {
        return name.substr(prefix.size(), name.size() - prefix.size() - suffix.size());
    }

    if (name == "savegame.txt") {
        return "legacy";
    }

    return name;
}

int getRoomLootLevelBonus(RoomType roomType) {
    switch (roomType) {
        case RoomType::TREASURE:
        case RoomType::SECRET:
            return 2;
        case RoomType::BOSS:
        case RoomType::SHADOWLORD:
            return 3;
        case RoomType::PUZZLE:
        case RoomType::FORGE:
        case RoomType::LABORATORY:
        case RoomType::SHRINE:
            return 1;
        default:
            return 0;
    }
}

}

// ---
// CONSTRUCTOR / DESTRUCTOR
// ---

GameEngine::GameEngine()
    : m_mapGenerator(DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT),
    m_mawMapGenerator(DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT),
      m_combatSystem(m_gameState.player, m_gameState.difficulty),
      m_inventorySystem(m_gameState.player),
      m_eventSystem(m_gameState.player, m_gameState),
      m_fileIO(DATA_FOLDER),
      m_merchantSystem(m_gameState.player),
      m_craftingSystem(m_gameState.player),
      m_isRunning(false),
    m_debugMode(false),
    m_isInMawRealm(false),
    m_leaveMawRequested(false),
    m_currentMawStratum(0),
    m_activeSaveFile(SAVE_FILE) {
    
    initializeRandom(0);  // Use time-based seed
}

GameEngine::~GameEngine() {
    // Cleanup if needed
}

// ---
// MAIN GAME FLOW
// ---

int GameEngine::run() {
    m_isRunning = true;
    
    showIntro();
    
    while (m_isRunning) {
        int choice = showMainMenu();
        
        switch (choice) {
            case 1:  // New Game
                startNewGame();
                gameLoop();
                endGame();
                break;
            case 2:  // Load Game
            {
                std::string selectedFile = selectSaveFileForLoad();
                if (!selectedFile.empty() && loadGame(selectedFile)) {
                    m_activeSaveFile = selectedFile;
                    gameLoop();
                    endGame();
                } else {
                    std::cout << "\nNo save selected or load failed.\n";
                    waitForEnter();
                }
                break;
            }
            case 3:  // Settings
                showSettings();
                break;
            case 4:  // Leaderboard
                displayLeaderboard();
                break;
            case 5:  // Quit
                std::cout << "\nThanks for playing Adventure Quest Engine!\n";
                m_isRunning = false;
                break;
            default:
                std::cout << "\nInvalid choice!\n";
                waitForEnter();
        }
    }
    
    return 0;
}

void GameEngine::showIntro() {
    clearScreen();
    std::cout << TITLE_ART << std::endl;
    std::cout << "\n  A C++ Text-Based Dungeon Crawler\n";
    std::cout << "  Version 1.0\n\n";
    waitForEnter();
}

int GameEngine::showMainMenu() {
    clearScreen();

    std::cout << UIManager::createTitle("ADVENTURE QUEST ENGINE", 80, UIColor::BRIGHT_CYAN);
    std::cout << "\n";
    std::cout << UIManager::centerText("A C++ Text-Based Dungeon Crawler", 80) << "\n";
    std::cout << UIManager::centerText("Version 1.0", 80) << "\n\n";

    if (!m_activeSaveFile.empty()) {
        std::cout << UIManager::colorize(" Active Save Slot: ", UIColor::BRIGHT_BLACK)
                  << UIManager::colorize(getSaveDisplayName(m_activeSaveFile), UIColor::BRIGHT_YELLOW, UIEffect::BOLD)
                  << "\n\n";
    }

    Menu menu;
    menu.title = "Main Menu";
    menu.titleColor = UIColor::BRIGHT_CYAN;
    menu.selectedColor = UIColor::BRIGHT_GREEN;

    MenuItem newGame;
    newGame.icon = "*";
    newGame.text = "New Game";
    newGame.description = "Start a fresh adventure from the entrance.";

    MenuItem loadGame;
    loadGame.icon = "S";
    loadGame.text = "Load Game";
    loadGame.description = "Continue from an existing save slot.";

    MenuItem settings;
    settings.icon = "#";
    settings.text = "Settings";
    settings.description = "Adjust difficulty and display options.";

    MenuItem leaderboard;
    leaderboard.icon = "T";
    leaderboard.text = "Leaderboard";
    leaderboard.description = "View top heroes and final scores.";

    MenuItem quit;
    quit.icon = "X";
    quit.text = "Quit";
    quit.description = "Exit Adventure Quest Engine.";

    menu.items = {newGame, loadGame, settings, leaderboard, quit};

    return UIManager::displayMenu(menu);
}

void GameEngine::startNewGame() {
    clearScreen();
    std::cout << "\n=== Starting New Adventure ===\n\n";
    
    // Load game data
    if (!loadGameData()) {
        std::cout << "\nWarning: Could not load all game data files.\n";
        std::cout << "Proceeding with default data...\n";
        waitForEnter();
    }
    
    // Character creation
    characterCreation();
    
    // Generate dungeon
    std::cout << "\nGenerating dungeon...\n";
    m_gameState.randomSeed = static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    if (m_gameState.randomSeed == 0) {
        m_gameState.randomSeed = 1;
    }
    initializeRandom(m_gameState.randomSeed);

    if (!m_mapGenerator.generateMap(m_gameState)) {
        std::cout << "ERROR: Failed to generate map!\n";
        waitForEnter();
        return;
    }
    
    // Set starting position
    m_gameState.player.currentRoomId = m_mapGenerator.getEntranceRoomId();
    
    // Initialize game
    initializeGame();
    
    // Show epic story introduction
    showStoryIntro();
    
    // Give starter quest
    giveStarterQuests();
    
    std::cout << "\nYour adventure begins...\n";
    waitForEnter();
}

void GameEngine::showStoryIntro() {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString("THE SHADOW LORD'S DUNGEON", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    
    std::cout << Color::BRIGHT_CYAN;
    std::cout << wrapText("Long ago, the Shadow Lord was sealed within this ancient dungeon "
                         "by the legendary heroes of old. But the seals are weakening, and "
                         "dark creatures now roam these halls once more.", 78) << "\n\n";
    std::cout << Color::RESET;
    
    std::cout << wrapText("You, " + m_gameState.player.name + ", have been chosen by the "
                         "Council of Light to venture into the depths, defeat the Shadow Lord, "
                         "and restore peace to the realm.", 78) << "\n\n";
    
    std::cout << Color::YELLOW;
    std::cout << "YOUR MISSION:\n";
    std::cout << Color::RESET;
    std::cout << "  1. Navigate through the dungeon, fighting monsters and solving puzzles\n";
    std::cout << "  2. Grow stronger by collecting loot and gaining experience\n";
    std::cout << "  3. Find the Shadow Lord Chamber hidden deep in the dungeon\n";
    std::cout << "  4. Defeat the Shadow Lord to complete your quest!\n\n";
    
    std::cout << Color::GREEN;
    std::cout << "TIPS FOR SUCCESS:\n";
    std::cout << Color::RESET;
    std::cout << "  - Type 'help' at any time to see available commands\n";
    std::cout << "  - Search rooms for hidden treasures\n";
    std::cout << "  - Use potions wisely - save some for the final battle!\n";
    std::cout << "  - Explore thoroughly to level up before facing the boss\n";
    std::cout << "  - Check your 'map' to see the dungeon layout\n\n";
    
    displayDivider(80, '-');
    std::cout << Color::BRIGHT_WHITE << "\nMay the light guide your path, brave hero!\n" << Color::RESET;
    waitForEnter();
}

void GameEngine::giveStarterQuests() {
    // Create and assign starter quests
    Quest mainQuest;
    mainQuest.id = 1;
    mainQuest.name = "Defeat the Shadow Lord";
    mainQuest.description = "Navigate through the dungeon, reach the Shadow Lord Chamber, and defeat the Shadow Lord to save the realm!";
    mainQuest.completionText = "You have vanquished the Shadow Lord! The realm is saved!";
    mainQuest.isMainQuest = true;
    mainQuest.requiresBoss = true;
    mainQuest.goldReward = 500;
    mainQuest.experienceReward = 1000;
    mainQuest.status = QuestStatus::IN_PROGRESS;
    m_gameState.player.activeQuests.push_back(mainQuest);
    
    Quest killQuest;
    killQuest.id = 2;
    killQuest.name = "Monster Slayer";
    killQuest.description = "Defeat 5 enemies to prove your combat prowess.";
    killQuest.completionText = "You've proven yourself a capable warrior!";
    killQuest.targetEnemyCount = 5;
    killQuest.goldReward = 75;
    killQuest.experienceReward = 150;
    killQuest.status = QuestStatus::IN_PROGRESS;
    m_gameState.player.activeQuests.push_back(killQuest);
    
    Quest exploreQuest;
    exploreQuest.id = 3;
    exploreQuest.name = "Explorer";
    exploreQuest.description = "Explore 10 different rooms in the dungeon.";
    exploreQuest.completionText = "Your exploration has revealed many secrets!";
    exploreQuest.targetEnemyCount = 10;  // Using for room count
    exploreQuest.goldReward = 50;
    exploreQuest.experienceReward = 100;
    exploreQuest.status = QuestStatus::IN_PROGRESS;
    m_gameState.player.activeQuests.push_back(exploreQuest);
    
    // Give starting items
    addStarterItems();
    
    clearScreen();
    std::cout << "\n" << Color::GREEN << "*** QUESTS RECEIVED! ***" << Color::RESET << "\n\n";
    std::cout << "Main Quest: " << Color::YELLOW << mainQuest.name << Color::RESET << "\n";
    std::cout << "  " << mainQuest.description << "\n\n";
    std::cout << "Side Quest: " << Color::CYAN << killQuest.name << Color::RESET << "\n";
    std::cout << "  " << killQuest.description << "\n\n";
    std::cout << "Side Quest: " << Color::CYAN << exploreQuest.name << Color::RESET << "\n";
    std::cout << "  " << exploreQuest.description << "\n\n";
    
    std::cout << Color::GREEN << "You've also received some starting supplies!" << Color::RESET << "\n";
    waitForEnter();
}

void GameEngine::addStarterItems() {
    // Add health potions
    Item healthPotion;
    healthPotion.id = 1001;
    healthPotion.name = "Health Potion";
    healthPotion.description = "A red potion that restores 40 HP.";
    healthPotion.type = ItemType::CONSUMABLE;
    healthPotion.rarity = ItemRarity::COMMON;
    healthPotion.healAmount = 40;
    healthPotion.quantity = 5;
    healthPotion.value = 25;
    healthPotion.isUsable = true;
    healthPotion.isStackable = true;
    m_gameState.player.inventory.push_back(healthPotion);

    // Add a starter weapon based on class
    Item weapon;
    weapon.id = 2001;
    weapon.type = ItemType::WEAPON;
    weapon.rarity = ItemRarity::UNCOMMON;
    weapon.quantity = 1;
    weapon.isUsable = false;
    weapon.isStackable = false;
    
    switch (m_gameState.player.charClass) {
        case CharacterClass::WARRIOR:
            weapon.name = "Steel Longsword";
            weapon.description = "A sturdy blade forged for battle.";
            // Lowered starter weapon damage for easier early game balance
            weapon.attackBonus = 16;
            weapon.value = 100;
            break;
        case CharacterClass::MAGE:
            weapon.name = "Enchanted Staff";
            weapon.description = "A staff humming with magical energy.";
            // Reduced attack and crit to make early encounters less punishing
            weapon.attackBonus = 9;
            weapon.critBonus = 12;
            weapon.value = 120;
            break;
        case CharacterClass::ROGUE:
            weapon.name = "Shadow Daggers";
            weapon.description = "Twin daggers perfect for swift strikes.";
            // Slightly reduced damage/crit and speed bonus to keep rogue viable but not overpowering
            weapon.attackBonus = 7;
            weapon.critBonus = 10;
            weapon.speedBonus = 3;
            weapon.value = 110;
            break;
        case CharacterClass::PALADIN:
            weapon.name = "Blessed Warhammer";
            weapon.description = "A holy weapon that smites evil.";
            // Toned down both attack and defense bonuses for starter paladin
            weapon.attackBonus = 13;
            weapon.defenseBonus = 10;
            weapon.value = 170;
            break;
        case CharacterClass::RANGER:
            weapon.name = "Ranger's Longbow";
            weapon.description = "A precise bow for ranged attacks.";
            weapon.attackBonus = 10;
            weapon.critBonus = 15;
            weapon.speedBonus = 2;
            weapon.value = 130;
            break;
        case CharacterClass::NECROMANCER:
            weapon.name = "Bone Staff";
            weapon.description = "A staff carved from ancient bones.";
            weapon.attackBonus = 8;
            weapon.critBonus = 10;
            weapon.value = 140;
            break;
        case CharacterClass::BARBARIAN:
            weapon.name = "Brutal Battle Axe";
            weapon.description = "A massive axe for devastating attacks.";
            weapon.attackBonus = 18;
            weapon.value = 150;
            break;
        case CharacterClass::MONK:
            weapon.name = "Worn Fighting Gloves";
            weapon.description = "Reinforced gloves for martial arts.";
            weapon.attackBonus = 11;
            weapon.speedBonus = 4;
            weapon.value = 125;
            break;
    }
    
    m_gameState.player.inventory.push_back(weapon);
    // Auto-equip starter weapon
    m_inventorySystem.equipItem(static_cast<int>(m_gameState.player.inventory.size()) - 1);
    
    // Add starting armor
    Item armor;
    armor.id = 3001;
    armor.name = "Adventurer's Armor";
    armor.description = "Basic protective gear for dungeon delving.";
    armor.type = ItemType::ARMOR;
    armor.rarity = ItemRarity::COMMON;
    // Slightly reduced starting armor to match lowered weapon stats
    armor.defenseBonus = 3;
    armor.quantity = 1;
    armor.value = 75;
    armor.isUsable = false;
    armor.isStackable = false;
    m_gameState.player.inventory.push_back(armor);
    // Auto-equip starter armor
    m_inventorySystem.equipItem(static_cast<int>(m_gameState.player.inventory.size()) - 1);
}

void GameEngine::gameLoop() {
    while (m_isRunning && m_gameState.player.isAlive && !m_gameState.player.hasWon) {
        // Update status effects
        updateStatusEffects();
        
        // Check quest progress
        checkQuestProgress();
        
        // Increment exploration turn counter
        m_gameState.player.explorationTurns++;
        m_gameState.player.totalTurns = m_gameState.player.explorationTurns + m_gameState.player.combatTurns;
        
        // Display current room
        displayCurrentRoom();
        
        // Get player action
        std::string action = getExplorationAction();
        
        // Process action
        if (!processAction(action)) {
            std::cout << "\nInvalid action. Type 'help' for commands.\n";
            waitForEnter();
        }
        
        // Check for room events
        checkRoomEvents();
    }
}

void GameEngine::endGame() {
    clearScreen();
    
    if (!m_gameState.player.isAlive) {
        std::cout << GAME_OVER_ART << std::endl;
        std::cout << "\nYou have died...\n";
        std::cout << "\nTurns survived: " << m_gameState.player.totalTurns << std::endl;
    } else if (m_gameState.player.hasWon) {
        std::cout << VICTORY_ART << std::endl;
        std::cout << "\nCongratulations! You have completed the adventure!\n";
    }
    
    // Determine and display ending
    EndingType ending = determineEnding();
    displayEnding(ending);
    
    // Calculate and display final score
    int score = calculateFinalScore();
    
    // Display stats
    displayGameStats();
    
    // Add to leaderboard
    LeaderboardEntry entry;
    entry.playerName = m_gameState.player.name;
    entry.score = score;
    entry.turns = m_gameState.player.totalTurns;
    entry.characterClass = getClassName(m_gameState.player.charClass);
    
    // Determine ending string
    std::string endingStr;
    switch (ending) {
        case EndingType::DEFEAT_DEATH: endingStr = "Death"; break;
        case EndingType::DEFEAT_TRAP: endingStr = "Trapped"; break;
        case EndingType::VICTORY_NORMAL: endingStr = "Victory"; break;
        case EndingType::ENDING_HERO: endingStr = "Hero"; break;
        case EndingType::ENDING_EVIL: endingStr = "Villain"; break;
        case EndingType::VICTORY_SPEEDRUN: endingStr = "Speedrun"; break;
        case EndingType::ENDING_COMPLETIONIST: endingStr = "Complete"; break;
        case EndingType::ENDING_PACIFIST: endingStr = "Pacifist"; break;
        case EndingType::ENDING_NEUTRAL: endingStr = "Neutral"; break;
        case EndingType::VICTORY_SECRET: endingStr = "Secret"; break;
        default: endingStr = "Unknown"; break;
    }
    entry.ending = endingStr;
    
    int position = m_fileIO.addToLeaderboard(entry, "data/leaderboard.txt", 20);
    
    if (position >= 0 && position < 10) {
        std::cout << "\n" << UIManager::createBox("★ NEW HIGH SCORE! You ranked #" + std::to_string(position + 1) + " on the leaderboard! ★", 60) << "\n";
    } else if (position >= 0) {
        std::cout << "\nYou made the leaderboard at position #" << (position + 1) << "!\n";
    }
    
    waitForEnter();
}

// ---
// CHARACTER CREATION
// ---

void GameEngine::characterCreation() {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString("CHARACTER CREATION", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    
    // Get player name
    m_gameState.player.name = getPlayerName();
    
    // Choose class
    CharacterClass chosenClass = chooseClass();
    m_gameState.player.charClass = chosenClass;
    
    // Apply class stats
    applyClassStats(chosenClass);
    
    // Show final character
    clearScreen();
    std::cout << "\n=== Character Created ===\n\n";
    std::cout << "Name: " << m_gameState.player.name << std::endl;
    std::cout << "Class: " << getClassName(chosenClass) << std::endl;
    std::cout << "\nStarting Stats:\n";
    std::cout << "  HP: " << m_gameState.player.stats.maxHP << std::endl;
    std::cout << "  Attack: " << m_gameState.player.stats.attack << std::endl;
    std::cout << "  Defense: " << m_gameState.player.stats.defense << std::endl;
    std::cout << "  Crit Chance: " << m_gameState.player.stats.critChance << "%" << std::endl;
    std::cout << "  Dodge Chance: " << m_gameState.player.stats.dodgeChance << "%" << std::endl;
    std::cout << "  Speed: " << m_gameState.player.stats.speed << std::endl;
    std::cout << "  Magic Power: " << m_gameState.player.stats.magicPower << std::endl;
    std::cout << "  Magic Resist: " << m_gameState.player.stats.magicResist << std::endl;
    std::cout << "  Gold: " << m_gameState.player.gold << std::endl;
    
    waitForEnter();
}

std::string GameEngine::getPlayerName() {
    return getValidatedString("\nEnter your character name: ", 2, 20);
}

CharacterClass GameEngine::chooseClass() {
    clearScreen();
    std::cout << UIManager::createTitle("⚔ CHOOSE YOUR CLASS ⚔", 70, UIColor::BRIGHT_YELLOW);
    std::cout << "\n";
    std::cout << "  ⚔  " << UIManager::colorize("1. Warrior", UIColor::BRIGHT_RED) << " - High HP and Attack\n";
    std::cout << "  ✨  " << UIManager::colorize("2. Mage", UIColor::BRIGHT_BLUE) << " - High Magic Power\n";
    std::cout << "  🗡  " << UIManager::colorize("3. Rogue", UIColor::BRIGHT_GREEN) << " - High Crit and Dodge\n";
    std::cout << "  ✨  " << UIManager::colorize("4. Paladin", UIColor::BRIGHT_YELLOW) << " - Balanced Stats\n";
    std::cout << "  🏹  " << UIManager::colorize("5. Ranger", UIColor::BRIGHT_CYAN) << " - High Speed and Crit\n";
    std::cout << "  💀  " << UIManager::colorize("6. Necromancer", UIColor::MAGENTA) << " - Dark Magic Specialist\n";
    std::cout << "  🪓  " << UIManager::colorize("7. Barbarian", UIColor::RED) << " - High HP and Attack\n";
    std::cout << "  👊  " << UIManager::colorize("8. Monk", UIColor::WHITE) << " - High Speed and Dodge\n\n";
    
    int choice = getValidatedInt("Enter class choice (1-8): ", 1, 8);
    
    CharacterClass chosenClass = static_cast<CharacterClass>(choice - 1);
    
    // Show class info
    displayClassInfo(chosenClass);
    
    if (getYesNo("\nConfirm this class? (y/n): ")) {
        return chosenClass;
    } else {
        return chooseClass();  // Recurse to choose again
    }
}

void GameEngine::applyClassStats(CharacterClass charClass) {
    switch (charClass) {
        case CharacterClass::WARRIOR:
            m_gameState.player.stats.maxHP = WARRIOR_HP;
            m_gameState.player.stats.currentHP = WARRIOR_HP;
            m_gameState.player.stats.maxMana = 30;
            m_gameState.player.stats.currentMana = 30;
            m_gameState.player.stats.attack = WARRIOR_ATK;
            m_gameState.player.stats.defense = WARRIOR_DEF;
            m_gameState.player.stats.critChance = WARRIOR_CRIT;
            m_gameState.player.stats.dodgeChance = WARRIOR_DODGE;
            m_gameState.player.stats.speed = WARRIOR_SPEED;
            m_gameState.player.stats.magicPower = WARRIOR_MAGIC;
            m_gameState.player.stats.magicResist = WARRIOR_MAGRES;
            break;
        case CharacterClass::MAGE:
            m_gameState.player.stats.maxHP = MAGE_HP;
            m_gameState.player.stats.currentHP = MAGE_HP;
            m_gameState.player.stats.maxMana = 100;
            m_gameState.player.stats.currentMana = 100;
            m_gameState.player.stats.attack = MAGE_ATK;
            m_gameState.player.stats.defense = MAGE_DEF;
            m_gameState.player.stats.critChance = MAGE_CRIT;
            m_gameState.player.stats.dodgeChance = MAGE_DODGE;
            m_gameState.player.stats.speed = MAGE_SPEED;
            m_gameState.player.stats.magicPower = MAGE_MAGIC;
            m_gameState.player.stats.magicResist = MAGE_MAGRES;
            break;
        case CharacterClass::ROGUE:
            m_gameState.player.stats.maxHP = ROGUE_HP;
            m_gameState.player.stats.currentHP = ROGUE_HP;
            m_gameState.player.stats.maxMana = 50;
            m_gameState.player.stats.currentMana = 50;
            m_gameState.player.stats.attack = ROGUE_ATK;
            m_gameState.player.stats.defense = ROGUE_DEF;
            m_gameState.player.stats.critChance = ROGUE_CRIT;
            m_gameState.player.stats.dodgeChance = ROGUE_DODGE;
            m_gameState.player.stats.speed = ROGUE_SPEED;
            m_gameState.player.stats.magicPower = ROGUE_MAGIC;
            m_gameState.player.stats.magicResist = ROGUE_MAGRES;
            break;
        case CharacterClass::PALADIN:
            m_gameState.player.stats.maxHP = PALADIN_HP;
            m_gameState.player.stats.currentHP = PALADIN_HP;
            m_gameState.player.stats.maxMana = 70;
            m_gameState.player.stats.currentMana = 70;
            m_gameState.player.stats.attack = PALADIN_ATK;
            m_gameState.player.stats.defense = PALADIN_DEF;
            m_gameState.player.stats.critChance = PALADIN_CRIT;
            m_gameState.player.stats.dodgeChance = PALADIN_DODGE;
            m_gameState.player.stats.speed = PALADIN_SPEED;
            m_gameState.player.stats.magicPower = PALADIN_MAGIC;
            m_gameState.player.stats.magicResist = PALADIN_MAGRES;
            break;
        case CharacterClass::RANGER:
            m_gameState.player.stats.maxHP = RANGER_HP;
            m_gameState.player.stats.currentHP = RANGER_HP;
            m_gameState.player.stats.maxMana = 50;
            m_gameState.player.stats.currentMana = 50;
            m_gameState.player.stats.attack = RANGER_ATK;
            m_gameState.player.stats.defense = RANGER_DEF;
            m_gameState.player.stats.critChance = RANGER_CRIT;
            m_gameState.player.stats.dodgeChance = RANGER_DODGE;
            m_gameState.player.stats.speed = RANGER_SPEED;
            m_gameState.player.stats.magicPower = RANGER_MAGIC;
            m_gameState.player.stats.magicResist = RANGER_MAGRES;
            break;
        case CharacterClass::NECROMANCER:
            m_gameState.player.stats.maxHP = NECROMANCER_HP;
            m_gameState.player.stats.currentHP = NECROMANCER_HP;
            m_gameState.player.stats.maxMana = 90;
            m_gameState.player.stats.currentMana = 90;
            m_gameState.player.stats.attack = NECROMANCER_ATK;
            m_gameState.player.stats.defense = NECROMANCER_DEF;
            m_gameState.player.stats.critChance = NECROMANCER_CRIT;
            m_gameState.player.stats.dodgeChance = NECROMANCER_DODGE;
            m_gameState.player.stats.speed = NECROMANCER_SPEED;
            m_gameState.player.stats.magicPower = NECROMANCER_MAGIC;
            m_gameState.player.stats.magicResist = NECROMANCER_MAGRES;
            break;
        case CharacterClass::BARBARIAN:
            m_gameState.player.stats.maxHP = BARBARIAN_HP;
            m_gameState.player.stats.currentHP = BARBARIAN_HP;
            m_gameState.player.stats.maxMana = 40;
            m_gameState.player.stats.currentMana = 40;
            m_gameState.player.stats.attack = BARBARIAN_ATK;
            m_gameState.player.stats.defense = BARBARIAN_DEF;
            m_gameState.player.stats.critChance = BARBARIAN_CRIT;
            m_gameState.player.stats.dodgeChance = BARBARIAN_DODGE;
            m_gameState.player.stats.speed = BARBARIAN_SPEED;
            m_gameState.player.stats.magicPower = BARBARIAN_MAGIC;
            m_gameState.player.stats.magicResist = BARBARIAN_MAGRES;
            break;
        case CharacterClass::MONK:
            m_gameState.player.stats.maxHP = MONK_HP;
            m_gameState.player.stats.currentHP = MONK_HP;
            m_gameState.player.stats.maxMana = 60;
            m_gameState.player.stats.currentMana = 60;
            m_gameState.player.stats.attack = MONK_ATK;
            m_gameState.player.stats.defense = MONK_DEF;
            m_gameState.player.stats.critChance = MONK_CRIT;
            m_gameState.player.stats.dodgeChance = MONK_DODGE;
            m_gameState.player.stats.speed = MONK_SPEED;
            m_gameState.player.stats.magicPower = MONK_MAGIC;
            m_gameState.player.stats.magicResist = MONK_MAGRES;
            break;
    }
    
    m_gameState.player.gold = STARTING_GOLD;
    
    // Add starting skills
    Skill skill;
    m_gameState.player.skills.clear();
    
    switch (charClass) {
        case CharacterClass::WARRIOR:
            skill.name = "Power Strike";
            skill.description = "A heavy blow that deals 150% damage.";
            skill.manaCost = 10;
            skill.cooldown = 4;
            skill.damageMultiplier = 1.5f;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill(); // Reset
            skill.name = "Shield Bash";
            skill.description = "Bash the enemy with your shield.";
            skill.manaCost = 15;
            skill.cooldown = 5;
            skill.damageMultiplier = 1.2f;
            m_gameState.player.skills.push_back(skill);
            break;
            
        case CharacterClass::MAGE:
            skill.name = "Fireball";
            skill.description = "Launch a ball of fire.";
            skill.manaCost = 15;
            skill.cooldown = 3;
            skill.damageMultiplier = 2.0f;
            skill.isMagical = true;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Ice Shard";
            skill.description = "Pierce enemy with ice.";
            skill.manaCost = 10;
            skill.cooldown = 2;
            skill.damageMultiplier = 1.5f;
            skill.isMagical = true;
            m_gameState.player.skills.push_back(skill);
            break;
            
        case CharacterClass::ROGUE:
            skill.name = "Backstab";
            skill.description = "Strike from the shadows.";
            skill.manaCost = 20;
            skill.cooldown = 5;
            skill.damageMultiplier = 2.5f;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Quick Slash";
            skill.description = "A fast attack.";
            skill.manaCost = 5;
            // Quick Slash now has a short cooldown so frequent spamming is punished
            skill.cooldown = 1;
            skill.damageMultiplier = 1.2f;
            m_gameState.player.skills.push_back(skill);
            break;
            
        case CharacterClass::PALADIN:
            skill.name = "Holy Smite";
            skill.description = "Smite the enemy with holy light.";
            skill.manaCost = 15;
            skill.cooldown = 4;
            skill.damageMultiplier = 3.5f;
            skill.isMagical = true;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Heal";
            skill.description = "Restore health.";
            skill.manaCost = 20;
            // Increase heal cooldown to make resource management more meaningful
            skill.cooldown = 6;
            skill.damageMultiplier = 4.0f; // Heals 2x Magic Power
            skill.isHeal = true;
            skill.isMagical = true;
            m_gameState.player.skills.push_back(skill);
            break;
        case CharacterClass::RANGER:
            skill.name = "Aimed Shot";
            skill.description = "A precise shot that deals 180% damage.";
            skill.manaCost = 12;
            skill.cooldown = 3;
            skill.damageMultiplier = 1.8f;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Multi-Shot";
            skill.description = "Fire multiple arrows at once.";
            skill.manaCost = 20;
            skill.cooldown = 5;
            skill.damageMultiplier = 1.4f;
            m_gameState.player.skills.push_back(skill);
            break;
        case CharacterClass::NECROMANCER:
            skill.name = "Life Tap";
            skill.description = "Drain life from the enemy.";
            skill.manaCost = 15;
            skill.cooldown = 4;
            skill.damageMultiplier = 1.5f;
            skill.isMagical = true;
            skill.isHeal = true; // Heals based on damage
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Bone Spear";
            skill.description = "Launch a spear of bone.";
            skill.manaCost = 25;
            skill.cooldown = 5;
            skill.damageMultiplier = 2.2f;
            skill.isMagical = true;
            m_gameState.player.skills.push_back(skill);
            break;
        case CharacterClass::BARBARIAN:
            skill.name = "Whirlwind";
            skill.description = "Spin with your weapon.";
            skill.manaCost = 15;
            skill.cooldown = 4;
            skill.damageMultiplier = 1.6f;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Berserk";
            skill.description = "A reckless attack.";
            skill.manaCost = 10;
            skill.cooldown = 3;
            skill.damageMultiplier = 2.0f;
            m_gameState.player.skills.push_back(skill);
            break;
        case CharacterClass::MONK:
            skill.name = "Tiger Strike";
            skill.description = "A powerful palm strike.";
            skill.manaCost = 10;
            skill.cooldown = 2;
            skill.damageMultiplier = 1.4f;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Meditation";
            skill.description = "Restore some mana and health.";
            skill.manaCost = 0;
            skill.cooldown = 8;
            skill.damageMultiplier = 2.0f;
            skill.isHeal = true;
            m_gameState.player.skills.push_back(skill);
            break;
    }
    
    // Initialize spells for magic-capable classes
    m_gameState.player.learnedSpells.clear();
    m_gameState.player.spellsLearned = 0;
    
    switch (charClass) {
        case CharacterClass::MAGE:
            // Mage gets access to multiple schools
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createFirebolt());
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createMagicShield());
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createArcaneMissiles());
            m_gameState.player.spellsLearned = 3;
            break;
            
        case CharacterClass::PALADIN:
            // Paladin gets healing and protection
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createHeal());
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createMagicShield());
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createStrengthBoost());
            m_gameState.player.spellsLearned = 3;
            break;
            
        case CharacterClass::NECROMANCER:
            // Necromancer gets dark magic
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createDrainLife());
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createCurseOfWeakness());
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createEnfeeblement());
            m_gameState.player.spellsLearned = 3;
            break;
            
        case CharacterClass::RANGER:
            // Ranger gets support and healing magic
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createHeal());
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createAnalyzeWeakness());
            m_gameState.player.learnedSpells.push_back(SpellLibrary::createHaste());
            m_gameState.player.spellsLearned = 3;
            break;
            
        default:
            // Warriors, Rogues, Barbarians, and Monks cannot cast spells
            break;
    }
}

void GameEngine::displayClassInfo(CharacterClass charClass) {
    clearScreen();
    std::cout << "\n=== " << getClassName(charClass) << " ===\n\n";
    
    switch (charClass) {
        case CharacterClass::WARRIOR:
            std::cout << WARRIOR_ART << std::endl;
            std::cout << "A mighty warrior with high HP and devastating attacks.\n";
            std::cout << "Best for players who prefer direct combat.\n";
            break;
        case CharacterClass::MAGE:
            std::cout << MAGE_ART << std::endl;
            std::cout << "A wielder of arcane magic with powerful spells.\n";
            std::cout << "Fragile but deals massive magical damage.\n";
            break;
        case CharacterClass::ROGUE:
            std::cout << ROGUE_ART << std::endl;
            std::cout << "A nimble fighter with high critical hit and dodge chances.\n";
            std::cout << "Excels at avoiding damage and striking weak points.\n";
            break;
        case CharacterClass::PALADIN:
            std::cout << PALADIN_ART << std::endl;
            std::cout << "A holy warrior with balanced stats and self-healing.\n";
            std::cout << "Versatile and durable for long adventures.\n";
            break;
        case CharacterClass::RANGER:
            std::cout << "A swift ranged specialist with high speed and critical hits.\n";
            std::cout << "Excels at precision damage and battlefield control.\n";
            break;
        case CharacterClass::NECROMANCER:
            std::cout << "A dark spellcaster who weakens foes and drains life.\n";
            std::cout << "Powerful magic with fragile defenses.\n";
            break;
        case CharacterClass::BARBARIAN:
            std::cout << "A relentless berserker with overwhelming strength and HP.\n";
            std::cout << "Built to crush enemies in direct combat.\n";
            break;
        case CharacterClass::MONK:
            std::cout << "A disciplined martial artist focused on speed and evasion.\n";
            std::cout << "Balanced offense with agile survivability.\n";
            break;
    }
}

// ---
// EXPLORATION
// ---

void GameEngine::displayCurrentRoom() {
    clearScreen();
    
    Room& room = getCurrentRoom();
    const bool firstVisit = !room.isExplored;

    if (!m_isInMawRealm && room.id == getActiveShadowLordRoomId()) {
        if (!m_gameState.player.shadowLordDefeated) {
            triggerShadowLordEncounterIfNeeded();
            return;
        }

        if (m_gameState.player.mawUnlocked) {
            handleMawPortal();
            return;
        }
    }
    
    // Mark room as explored
    if (firstVisit) {
        room.isExplored = true;
        m_gameState.player.visitedRooms.push_back(room.id);

        if (!room.items.empty()) {
            const int contextLevel = std::max(1, m_gameState.player.level + getRoomLootLevelBonus(room.type));
            const bool bossScaled = (room.type == RoomType::BOSS || room.type == RoomType::SHADOWLORD);

            for (Item& item : room.items) {
                const int rolledItemLevel = rollItemLevel(contextLevel, item.rarity, bossScaled);
                applyItemLevelScaling(item, rolledItemLevel, true);
            }
        }
    }
    
    std::cout << UIManager::createTitle(room.name, 80, UIColor::BRIGHT_CYAN);
    std::cout << "\n";

    std::cout << UIManager::colorize(" Type: ", UIColor::BRIGHT_BLACK)
              << UIManager::colorize(getRoomTypeName(room.type), UIColor::BRIGHT_YELLOW, UIEffect::BOLD)
              << "\n\n";

    if (room.isExplored && !room.shortDesc.empty()) {
        std::cout << wrapText(room.shortDesc, 78) << "\n\n";
    } else {
        std::cout << wrapText(room.description, 78) << "\n\n";
    }

    if (!room.enemies.empty()) {
        std::cout << UIManager::colorize("Enemies", UIColor::BRIGHT_RED, UIEffect::BOLD) << "\n";
        for (const auto& enemy : room.enemies) {
            std::cout << "  - " << UIManager::colorize(enemy.name, UIColor::BRIGHT_RED)
                      << " (HP " << enemy.stats.currentHP << ")\n";
        }
        std::cout << "\n";
    }

    if (!room.items.empty()) {
        std::cout << UIManager::colorize("Items", UIColor::BRIGHT_GREEN, UIEffect::BOLD) << "\n";
        for (size_t i = 0; i < room.items.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << room.items[i].name
                      << " [iLvl " << room.items[i].itemLevel << "]";
            if (room.items[i].hasUniquePerk && !room.items[i].uniquePerkName.empty()) {
                std::cout << " <" << room.items[i].uniquePerkName << ">";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    std::cout << UIManager::colorize("Exits", UIColor::BRIGHT_CYAN, UIEffect::BOLD) << ": ";
    if (room.hasNorth) std::cout << "[N] ";
    if (room.hasSouth) std::cout << "[S] ";
    if (room.hasEast) std::cout << "[E] ";
    if (room.hasWest) std::cout << "[W] ";
    std::cout << "\n";

    displayDivider(80, '-');

    showPlayerStatus();

    displayDivider(80, '-');
}

std::string GameEngine::getExplorationAction() {
    std::string actionPrompt = " [n/s/e/w, look, search, interact, inventory, status, quests, map, menu, help]";
    if (m_isInMawRealm) {
        actionPrompt = " [n/s/e/w, look, search, interact, inventory, status, quests, map, extract, menu, help]";
    }

    std::cout << "\n"
              << UIManager::colorize("Action", UIColor::BRIGHT_CYAN, UIEffect::BOLD)
              << actionPrompt
              << "\n> ";
    std::string action;
    std::getline(std::cin, action);
    return toLower(trim(action));
}

bool GameEngine::processAction(const std::string& action) {
    if (action.empty()) return false;
    
    // Movement commands
    if (action == "n" || action == "north") {
        return movePlayer('N');
    } else if (action == "s" || action == "south") {
        return movePlayer('S');
    } else if (action == "e" || action == "east") {
        return movePlayer('E');
    } else if (action == "w" || action == "west") {
        return movePlayer('W');
    }
    
    // Action commands
    else if (action == "look" || action == "l") {
        displayCurrentRoom();
        return true;
    } else if (action == "search") {
        searchRoom();
        return true;
    } else if (action == "interact" || action == "i") {
        interactWithRoom();
        return true;
    } else if (action == "craft" || action == "forge") {
        Room& room = getCurrentRoom();
        if (room.type == RoomType::FORGE || room.type == RoomType::LABORATORY) {
            m_craftingSystem.runCraftingMenu();
        } else {
            std::cout << "\nYou need a forge or laboratory to craft here.\n";
            waitForEnter();
        }
        return true;
    } else if (action.substr(0, 6) == "pickup" || action.substr(0, 4) == "take") {
        // Try to extract item number
        int itemNum = 1;
        if (action.length() > 7) {
            try {
                itemNum = std::stoi(action.substr(7));
            } catch (...) {}
        }
        return pickupItem(itemNum - 1);
    }
    
    // Menu commands
    else if (action == "inventory" || action == "inv") {
        openInventory();
        return true;
    } else if (action == "status" || action == "stats") {
        clearScreen();
        showPlayerStatus();
        waitForEnter();
        return true;
    } else if (action == "quests" || action == "quest") {
        openQuestLog();
        return true;
    } else if (action == "map" || action == "m") {
        openMap();
        return true;
    } else if (action == "help" || action == "h" || action == "?") {
        showHelp();
        return true;
    } else if (action == "cheats" && m_debugMode) {
        openCheatMenu();
        return true;
    } else if (action == "extract" || action == "leave maw") {
        if (!m_isInMawRealm) {
            std::cout << "\nThere is nothing to extract from here.\n";
            waitForEnter();
            return true;
        }

        if (getYesNo("\nExtract from The Maw and return to the portal room")) {
            m_leaveMawRequested = true;
        }
        return true;
    } else if (action == "menu") {
        int choice = showGameMenu();
        return choice != 0;
    } else if (action == "save") {
        std::string saveFile = selectSaveFileForSave();
        if (!saveFile.empty()) {
            if (saveGame(saveFile)) {
                m_activeSaveFile = saveFile;
                std::cout << "\nGame saved to slot '" << getSaveDisplayName(saveFile) << "'.\n";
            } else {
                std::cout << "\nFailed to save game.\n";
            }
        } else {
            std::cout << "\nSave cancelled.\n";
        }
        waitForEnter();
        return true;
    } else if (action == "quit" || action == "exit") {
        if (getYesNo("\nAre you sure you want to quit? (y/n): ")) {
            m_isRunning = false;
        }
        return true;
    }
    
    return false;
}

bool GameEngine::movePlayer(char direction) {
    Room& currentRoom = getCurrentRoom();
    int newRoomId = -1;
    
    switch (direction) {
        case 'N':
            if (!currentRoom.hasNorth) {
                std::cout << "\nYou cannot go that way.\n";
                waitForEnter();
                return false;
            }
            newRoomId = currentRoom.northRoomId;
            break;
        case 'S':
            if (!currentRoom.hasSouth) {
                std::cout << "\nYou cannot go that way.\n";
                waitForEnter();
                return false;
            }
            newRoomId = currentRoom.southRoomId;
            break;
        case 'E':
            if (!currentRoom.hasEast) {
                std::cout << "\nYou cannot go that way.\n";
                waitForEnter();
                return false;
            }
            newRoomId = currentRoom.eastRoomId;
            break;
        case 'W':
            if (!currentRoom.hasWest) {
                std::cout << "\nYou cannot go that way.\n";
                waitForEnter();
                return false;
            }
            newRoomId = currentRoom.westRoomId;
            break;
    }
    
    // Check if room is locked
    for (auto& room : m_gameState.dungeon) {
        if (room.id == newRoomId) {
            if (room.isLocked) {
                // Check if player has key
                bool hasKey = false;
                for (const auto& item : m_gameState.player.inventory) {
                    if (item.id == room.requiredKeyId) {
                        hasKey = true;
                        break;
                    }
                }
                
                if (!hasKey) {
                    std::cout << "\nThe door is locked. You need a key to proceed.\n";
                    waitForEnter();
                    return false;
                } else {
                    std::cout << "\nYou use your key to unlock the door.\n";
                    room.isLocked = false;
                    waitForEnter();
                }
            }
            break;
        }
    }
    
    // Move player
    m_gameState.player.currentRoomId = newRoomId;

    if (m_isInMawRealm) {
        return advanceMawStratumIfNeeded();
    }

    triggerShadowLordEncounterIfNeeded();
    
    return true;
}

void GameEngine::searchRoom() {
    Room& room = getCurrentRoom();
    
    std::cout << "\nYou search the room carefully...\n";
    delay(1000);
    
    bool foundSomething = false;
    
    // Chance to find hidden items
    if (randomChance(30)) {
        std::cout << "\nYou found a hidden item!\n";
        const int contextLevel = std::max(1, m_gameState.player.level +
            ((room.type == RoomType::TREASURE || room.type == RoomType::SECRET) ? 2 : 0));

        Item hiddenItem;
        std::vector<Item> nonMawTemplates;
        for (const Item& templateItem : m_gameState.itemTemplates) {
            if (!templateItem.mawOnly) {
                nonMawTemplates.push_back(templateItem);
            }
        }

        if (!nonMawTemplates.empty() && randomChance(60)) {
            hiddenItem = generateItemFromTemplate(
                randomElement(nonMawTemplates),
                contextLevel,
                true
            );
        } else {
            ItemRarity rarity = ItemRarity::UNCOMMON;
            if (room.type == RoomType::TREASURE || room.type == RoomType::SECRET) {
                int roll = randomInt(1, 100);
                if (roll <= 12) rarity = ItemRarity::LEGENDARY;
                else if (roll <= 30) rarity = ItemRarity::EPIC;
                else if (roll <= 60) rarity = ItemRarity::RARE;
            }
            hiddenItem = generateRandomItem(rarity, contextLevel);
        }

        room.items.push_back(hiddenItem);
        std::cout << "It shimmers at iLvl " << hiddenItem.itemLevel;
        if (hiddenItem.hasUniquePerk && !hiddenItem.uniquePerkName.empty()) {
            std::cout << " with perk '" << hiddenItem.uniquePerkName << "'";
        }
        std::cout << ".\n";
        foundSomething = true;
    }
    
    // Chance to discover secret
    if (room.isSecret && randomChance(40)) {
        std::cout << "\nYou discovered this is a secret room!\n";
        m_gameState.player.secretsFound++;
        foundSomething = true;
    }

    // Chance to gather crafting materials based on room type.
    if (randomChance(45)) {
        if (room.type == RoomType::FORGE || room.type == RoomType::ARMORY) {
            m_craftingSystem.addMaterial(1, randomInt(1, 3));  // Iron Ore
            if (randomChance(35)) {
                m_craftingSystem.addMaterial(5, 1);  // Rough Gem
            }
        } else if (room.type == RoomType::LABORATORY || room.type == RoomType::SHRINE) {
            m_craftingSystem.addMaterial(7, randomInt(1, 3));  // Herbs
            if (randomChance(40)) {
                m_craftingSystem.addMaterial(6, 1);  // Essence
            }
        } else {
            m_craftingSystem.addMaterial(2, randomInt(1, 2));  // Wood
            if (randomChance(30)) {
                m_craftingSystem.addMaterial(3, 1);  // Leather
            }
        }
        foundSomething = true;
    }
    
    if (!foundSomething) {
        std::cout << "You didn't find anything special.\n";
    }
    
    waitForEnter();
}

void GameEngine::interactWithRoom() {
    Room& room = getCurrentRoom();
    
    // Check for enemies first
    if (!room.enemies.empty()) {
        std::cout << "\nYou cannot interact while enemies are present!\n";
        waitForEnter();
        return;
    }
    
    // Check for puzzle
    if (room.type == RoomType::PUZZLE && !room.puzzleSolved) {
        std::cout << "\n=== Puzzle Room ===\n";
        std::cout << room.puzzleQuestion << "\n\n";
        
        std::string answer = getValidatedString("Your answer: ", 1, 50);
        
        if (toLower(trim(answer)) == toLower(room.puzzleAnswer)) {
            std::cout << "\nCorrect! The puzzle is solved!\n";
            room.puzzleSolved = true;
            m_gameState.player.puzzlesSolved++;
            
            // Give reward
            if (room.puzzleRewardItemId != -1) {
                Item reward;
                reward.id = room.puzzleRewardItemId;
                reward.name = "Puzzle Reward";
                reward.description = "A reward for solving the puzzle";
                reward.value = 100;
                m_gameState.player.inventory.push_back(reward);
                std::cout << "You received: " << reward.name << "\n";
            }
        } else {
            std::cout << "\nIncorrect! Nothing happens.\n";
        }
        
        waitForEnter();
        return;
    }

    if (room.type == RoomType::FORGE) {
        handleForgeEncounter();
        return;
    }

    if (room.type == RoomType::LABORATORY) {
        handleLaboratoryEncounter();
        return;
    }

    if (room.type == RoomType::SHOP) {
        handleShopEncounter();
        return;
    }

    if (room.type == RoomType::LIBRARY) {
        handleLibraryEncounter();
        return;
    }
    
    std::cout << "\nThere's nothing to interact with here.\n";
    waitForEnter();
}

bool GameEngine::pickupItem(int itemIndex) {
    Room& room = getCurrentRoom();
    
    if (itemIndex < 0 || itemIndex >= static_cast<int>(room.items.size())) {
        std::cout << "\nThere's no item with that number.\n";
        waitForEnter();
        return false;
    }
    
    Item item = room.items[itemIndex];
    
    // Check inventory space
    if (m_gameState.player.inventory.size() >= static_cast<size_t>(m_gameState.player.maxInventorySize)) {
        std::cout << "\nYour inventory is full!\n";
        waitForEnter();
        return false;
    }
    
    // Add to inventory
    m_gameState.player.inventory.push_back(item);
    room.items.erase(room.items.begin() + itemIndex);
    
    std::cout << "\nYou picked up: " << item.name << " (iLvl " << item.itemLevel << ")";
    if (item.hasUniquePerk && !item.uniquePerkName.empty()) {
        std::cout << " [Perk: " << item.uniquePerkName << "]";
    }
    std::cout << "\n";
    waitForEnter();
    
    return true;
}

// ---
// MENUS & UI
// ---

int GameEngine::showGameMenu() {
    clearScreen();
    std::cout << UIManager::createTitle("Game Menu", 70, UIColor::BRIGHT_CYAN) << "\n";

    Menu menu;
    menu.title = "Session Options";
    menu.titleColor = UIColor::CYAN;
    menu.selectedColor = UIColor::BRIGHT_GREEN;

    MenuItem continueItem;
    continueItem.icon = ">";
    continueItem.text = "Continue";
    continueItem.description = "Return to the dungeon.";

    MenuItem saveItem;
    saveItem.icon = "S";
    saveItem.text = "Save Game";
    saveItem.description = "Write progress to a save slot.";

    MenuItem settingsItem;
    settingsItem.icon = "#";
    settingsItem.text = "Settings";
    settingsItem.description = "Change gameplay/display options.";

    MenuItem quitItem;
    quitItem.icon = "X";
    quitItem.text = "Quit to Main Menu";
    quitItem.description = "Leave the current run.";

    menu.items = {continueItem, saveItem, settingsItem, quitItem};
    int choice = UIManager::displayMenu(menu);
    
    if (choice == 2) {
        std::string saveFile = selectSaveFileForSave();
        if (!saveFile.empty() && saveGame(saveFile)) {
            m_activeSaveFile = saveFile;
            std::cout << "\nGame saved to slot '" << getSaveDisplayName(saveFile) << "'.\n";
        } else if (saveFile.empty()) {
            std::cout << "\nSave cancelled.\n";
        } else {
            std::cout << "\nFailed to save game.\n";
        }
        waitForEnter();
    } else if (choice == 3) {
        showSettings();
    } else if (choice == 4) {
        if (getYesNo("\nAre you sure? Unsaved progress will be lost. (y/n): ")) {
            m_isRunning = false;
        }
    }
    
    return choice;
}

void GameEngine::showPlayerStatus() {
    const Player& p = m_gameState.player;

    StatusDisplay status;
    status.name = p.name + " (" + getClassName(p.charClass) + ")";
    status.hp = p.stats.currentHP;
    status.maxHP = p.stats.maxHP;
    status.mana = p.stats.currentMana;
    status.maxMana = p.stats.maxMana;
    status.level = p.level;
    status.gold = p.gold;
    status.location = getCurrentRoom().name;

    for (const auto& effect : p.activeEffects) {
        const std::string entry = effect.name + " (" + std::to_string(effect.turnsRemaining) + "t)";
        if (effect.isDebuff) {
            status.activeDebuffs.push_back(entry);
        } else {
            status.activeBuffs.push_back(entry);
        }
    }

    UIManager::displayPlayerStatus(status);

    std::cout << " " << UIManager::renderXPBar(p.experience, p.experienceToLevel, 32) << "\n";
    std::cout << " Avg iLvl: " << p.averageItemLevel << " | Total Turns: " << p.totalTurns << "\n";
}

void GameEngine::openInventory() {
    m_inventorySystem.displayInventoryMenu();
}

void GameEngine::openQuestLog() {
    clearScreen();
    std::cout << UIManager::createTitle("Quest Log", 80, UIColor::BRIGHT_MAGENTA) << "\n";

    if (m_gameState.player.activeQuests.empty()) {
        std::cout << UIManager::colorize("No active quests.", UIColor::BRIGHT_BLACK) << "\n";
    } else {
        int index = 1;
        for (const auto& quest : m_gameState.player.activeQuests) {
            std::cout << UIManager::colorize(std::to_string(index) + ". " + quest.name, UIColor::BRIGHT_YELLOW, UIEffect::BOLD) << "\n";
            std::cout << "   " << wrapText(quest.description, 74) << "\n";
            std::cout << "   Status: ";
            switch (quest.status) {
                case QuestStatus::IN_PROGRESS:
                    std::cout << UIManager::colorize("In Progress", UIColor::BRIGHT_CYAN) << "\n";
                    break;
                case QuestStatus::COMPLETED:
                    std::cout << UIManager::colorize("Completed", UIColor::BRIGHT_GREEN) << "\n";
                    break;
                case QuestStatus::FAILED:
                    std::cout << UIManager::colorize("Failed", UIColor::BRIGHT_RED) << "\n";
                    break;
                default:
                    std::cout << UIManager::colorize("Not Started", UIColor::BRIGHT_BLACK) << "\n";
                    break;
            }
            std::cout << "\n";
            ++index;
        }
    }

    std::cout << UIManager::colorize("Completed Quests: ", UIColor::BRIGHT_GREEN)
              << m_gameState.player.completedQuests.size() << "\n";

    displayDivider(80, '-');
    waitForEnter();
}

void GameEngine::openMap() {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString(m_isInMawRealm ? "MAW MAP" : "MAP", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";

    if (m_isInMawRealm) {
        m_mawMapGenerator.printMawMap(m_gameState.player.currentRoomId, m_currentMawStratum, true);
    } else {
        m_mapGenerator.printMap(m_gameState.player.currentRoomId);
    }
    
    displayDivider(80, '-');
    waitForEnter();
}

void GameEngine::showHelp() {
    clearScreen();
    std::cout << UIManager::createTitle("Adventure Guide", 80, UIColor::BRIGHT_YELLOW) << "\n";

    std::cout << UIManager::colorize("Objective", UIColor::BRIGHT_YELLOW, UIEffect::BOLD) << "\n";
    std::cout << "  Reach the Shadow Lord Chamber and win the final battle.\n";
    std::cout << "  The chamber is marked with the skull symbol on the map.\n\n";

    std::cout << UIManager::colorize("Movement", UIColor::BRIGHT_GREEN, UIEffect::BOLD) << "\n";
    std::cout << "  n / north   Move north\n";
    std::cout << "  s / south   Move south\n";
    std::cout << "  e / east    Move east\n";
    std::cout << "  w / west    Move west\n\n";

    std::cout << UIManager::colorize("Exploration", UIColor::BRIGHT_CYAN, UIEffect::BOLD) << "\n";
    std::cout << "  look        Reprint room details\n";
    std::cout << "  search      Find hidden loot and resources\n";
    std::cout << "  interact    Use room-specific interactions\n";
    std::cout << "  craft       Craft at forge/laboratory\n";
    std::cout << "  pickup [#]  Take a numbered item\n\n";

    std::cout << UIManager::colorize("Menus", UIColor::BRIGHT_WHITE, UIEffect::BOLD) << "\n";
    std::cout << "  inventory   Open inventory and equipment\n";
    std::cout << "  status      View character overview\n";
    std::cout << "  quests      Open quest log\n";
    std::cout << "  map         View dungeon map\n";
    std::cout << "  menu        Open game menu\n";
    std::cout << "  help        Show this guide\n\n";

    std::cout << UIManager::colorize("System", UIColor::BRIGHT_YELLOW, UIEffect::BOLD) << "\n";
    std::cout << "  save        Save current run\n";
    std::cout << "  quit        Exit current run\n\n";

    if (m_debugMode) {
        std::cout << UIManager::colorize("Debug", UIColor::BRIGHT_RED, UIEffect::BOLD) << "\n";
        std::cout << "  cheats      Open cheat menu (debug only)\n\n";
    }

    std::cout << UIManager::colorize("Room Symbols", UIColor::BRIGHT_YELLOW, UIEffect::BOLD) << "\n";
    std::cout << "  E Entrance  X Exit/Boss  B Boss Room  T Treasure\n";
    std::cout << "  ! Trap      ? Puzzle     S Secret     R Safe Room\n";
    std::cout << "  . Normal Room\n";
    
    displayDivider(80, '-');
    waitForEnter();
}

void GameEngine::showSettings() {
    bool inSettings = true;

    while (inSettings) {
        clearScreen();
        std::cout << UIManager::createTitle("Settings", 72, UIColor::BRIGHT_CYAN) << "\n";

        std::string difficultyLabel = "Normal";
        if (m_gameState.difficulty == 1) {
            difficultyLabel = "Easy";
        } else if (m_gameState.difficulty == 3) {
            difficultyLabel = "Hard";
        }

        std::cout << "  Difficulty      : " << UIManager::colorize(difficultyLabel, UIColor::BRIGHT_YELLOW, UIEffect::BOLD) << "\n";
        std::cout << "  ASCII Art       : "
                  << UIManager::colorize(m_gameState.showAsciiArt ? "Enabled" : "Disabled",
                                          m_gameState.showAsciiArt ? UIColor::BRIGHT_GREEN : UIColor::BRIGHT_BLACK)
                  << "\n";
        std::cout << "  Debug Mode      : "
                  << UIManager::colorize(m_debugMode ? "Enabled" : "Disabled",
                                          m_debugMode ? UIColor::BRIGHT_RED : UIColor::BRIGHT_BLACK)
                  << "\n\n";

        Menu menu;
        menu.title = "Settings Menu";
        menu.titleColor = UIColor::CYAN;
        menu.selectedColor = UIColor::BRIGHT_GREEN;

        MenuItem item1;
        item1.icon = "1";
        item1.text = "Change Difficulty";
        item1.description = "Set difficulty to Easy, Normal, or Hard.";

        MenuItem item2;
        item2.icon = "2";
        item2.text = "Toggle ASCII Art";
        item2.description = "Enable or disable large ASCII visuals.";

        MenuItem item3;
        item3.icon = "3";
        item3.text = "Toggle Debug Mode";
        item3.description = "Enable cheat/debug commands.";

        MenuItem item4;
        item4.icon = "4";
        item4.text = "Back";
        item4.description = "Return to the previous menu.";

        menu.items = {item1, item2, item3, item4};

        int choice = UIManager::displayMenu(menu);

        if (choice == 1) {
            m_gameState.difficulty = getValidatedInt("Difficulty (1=Easy, 2=Normal, 3=Hard): ", 1, 3);
        } else if (choice == 2) {
            m_gameState.showAsciiArt = !m_gameState.showAsciiArt;
        } else if (choice == 3) {
            m_debugMode = !m_debugMode;
            m_gameState.debugMode = m_debugMode;
        } else if (choice == 4) {
            inSettings = false;
        }
    }
}


void GameEngine::openCheatMenu() {
    if (!m_debugMode) {
        std::cout << "\nCheats are only available in debug mode!\n";
        waitForEnter();
        return;
    }
    
    bool inMenu = true;
    
    while (inMenu) {
        clearScreen();
        std::cout << UIManager::createTitle("⚠️ DEBUG CHEAT MENU ⚠️", 80, UIColor::BRIGHT_RED) << "\n\n";
        
        std::cout << Color::BRIGHT_RED << "WARNING: Using cheats may break game balance!" << Color::RESET << "\n\n";
        
        std::cout << Color::BRIGHT_YELLOW << "PLAYER CHEATS:" << Color::RESET << "\n";
        std::cout << "  1. Full Heal (HP + Mana)\n";
        std::cout << "  2. Add 1000 Gold\n";
        std::cout << "  3. Add 5000 Experience\n";
        std::cout << "  4. Max Out Stats (+20 to all)\n";
        std::cout << "  5. Full Inventory of Potions\n\n";
        
        std::cout << Color::BRIGHT_CYAN << "ROOM CHEATS:" << Color::RESET << "\n";
        std::cout << "  6. Clear All Enemies in Room\n";
        std::cout << "  7. Reveal All Map Rooms\n";
        std::cout << "  8. Teleport to Shadow Lord Chamber\n\n";
        
        std::cout << Color::BRIGHT_GREEN << "QUEST CHEATS:" << Color::RESET << "\n";
        std::cout << "  9. Complete All Active Quests\n";
        std::cout << " 10. Reset All Quests\n\n";
        
        std::cout << Color::BRIGHT_MAGENTA << "ITEM CHEATS:" << Color::RESET << "\n";
        std::cout << " 11. God Mode Weapon (+999 Attack)\n";
        std::cout << " 12. God Mode Armor (+999 Defense)\n\n";
        
        std::cout << Color::WHITE << " 0. Exit Cheat Menu\n\n" << Color::RESET;
        
        int choice = getValidatedInt("Select cheat: ", 0, 12);
        
        switch (choice) {
            case 0:
                inMenu = false;
                break;
                
            case 1: // Full Heal
                m_gameState.player.stats.currentHP = m_gameState.player.stats.maxHP;
                m_gameState.player.stats.currentMana = m_gameState.player.stats.maxMana;
                std::cout << "\n" << Color::BRIGHT_GREEN << "❤️ Full heal applied! HP and Mana restored!" << Color::RESET << "\n";
                waitForEnter();
                break;
                
            case 2: // Add Gold
                m_gameState.player.gold += 1000;
                std::cout << "\n" << Color::BRIGHT_YELLOW << "💰 +1000 gold added! Total: " << m_gameState.player.gold << Color::RESET << "\n";
                waitForEnter();
                break;
                
            case 3: // Add XP
                m_gameState.player.experience += 5000;
                std::cout << "\n" << Color::BRIGHT_CYAN << "✨ +5000 XP added! Total: " << m_gameState.player.experience << Color::RESET << "\n";
                checkLevelUp();
                waitForEnter();
                break;
                
            case 4: // Max Stats
                m_gameState.player.stats.maxHP += 200;
                m_gameState.player.stats.currentHP = m_gameState.player.stats.maxHP;
                m_gameState.player.stats.maxMana += 200;
                m_gameState.player.stats.currentMana = m_gameState.player.stats.maxMana;
                m_gameState.player.stats.attack += 20;
                m_gameState.player.stats.defense += 20;
                m_gameState.player.stats.speed += 20;
                m_gameState.player.stats.critChance += 20;
                m_gameState.player.stats.dodgeChance += 20;
                std::cout << "\n" << Color::BRIGHT_GREEN << "💪 All stats massively boosted!" << Color::RESET << "\n";
                waitForEnter();
                break;
                
            case 5: // Full Inventory of Potions
                {
                    Item healthPotion;
                    healthPotion.id = 9001;
                    healthPotion.name = "Cheat Health Potion";
                    healthPotion.description = "A debug potion that restores 100 HP.";
                    healthPotion.type = ItemType::CONSUMABLE;
                    healthPotion.rarity = ItemRarity::LEGENDARY;
                    healthPotion.healAmount = 100;
                    healthPotion.quantity = 99;
                    healthPotion.value = 0;
                    healthPotion.isUsable = true;
                    healthPotion.isStackable = true;
                    m_gameState.player.inventory.push_back(healthPotion);
                    
                    std::cout << "\n" << Color::BRIGHT_MAGENTA << "🧪 99 Health potions added!" << Color::RESET << "\n";
                    waitForEnter();
                }
                break;
                
            case 6: // Clear Enemies
                {
                    Room& room = getCurrentRoom();
                    int enemyCount = room.enemies.size();
                    room.enemies.clear();
                    std::cout << "\n" << Color::BRIGHT_RED << "⚔️ Cleared " << enemyCount << " enemies from the room!" << Color::RESET << "\n";
                    waitForEnter();
                }
                break;
                
            case 7: // Reveal Map
                for (auto& room : m_gameState.dungeon) {
                    room.isExplored = true;
                }
                std::cout << "\n" << Color::BRIGHT_CYAN << "🗺️ All rooms revealed on the map!" << Color::RESET << "\n";
                waitForEnter();
                break;
                
            case 8: // Teleport to Shadow Lord chamber
                m_gameState.player.currentRoomId = getActiveShadowLordRoomId();
                std::cout << "\n" << Color::BRIGHT_MAGENTA << "✨ Teleported to the Shadow Lord chamber!" << Color::RESET << "\n";
                waitForEnter();
                triggerShadowLordEncounterIfNeeded();
                displayCurrentRoom();
                break;
                
            case 9: // Complete Quests
                {
                    int completed = 0;
                    for (auto& quest : m_gameState.player.activeQuests) {
                        if (quest.status != QuestStatus::COMPLETED) {
                            quest.status = QuestStatus::COMPLETED;
                            quest.currentEnemyCount = quest.targetEnemyCount;
                            m_gameState.player.completedQuests.push_back(quest);
                            completed++;
                        }
                    }
                    m_gameState.player.activeQuests.clear();
                    std::cout << "\n" << Color::BRIGHT_GREEN << "✔️ Completed " << completed << " quests!" << Color::RESET << "\n";
                    waitForEnter();
                }
                break;
                
            case 10: // Reset Quests
                m_gameState.player.activeQuests.clear();
                m_gameState.player.completedQuests.clear();
                giveStarterQuests();
                std::cout << "\n" << Color::BRIGHT_YELLOW << "🔄 All quests reset! Starter quests given." << Color::RESET << "\n";
                waitForEnter();
                break;
                
            case 11: // God Weapon
                {
                    Item godWeapon;
                    godWeapon.id = 9999;
                    godWeapon.name = "[DEBUG] God Slayer Blade";
                    godWeapon.description = "A debug weapon with godlike power. Destroys balance.";
                    godWeapon.type = ItemType::WEAPON;
                    godWeapon.rarity = ItemRarity::LEGENDARY;
                    godWeapon.attackBonus = 999;
                    godWeapon.critBonus = 99;
                    godWeapon.speedBonus = 50;
                    godWeapon.quantity = 1;
                    godWeapon.value = 999999;
                    godWeapon.isUsable = false;
                    godWeapon.isStackable = false;
                    m_gameState.player.inventory.push_back(godWeapon);
                    std::cout << "\n" << Color::BRIGHT_RED << "⚔️ God Slayer Blade added to inventory!" << Color::RESET << "\n";
                    waitForEnter();
                }
                break;
                
            case 12: // God Armor
                {
                    Item godArmor;
                    godArmor.id = 9998;
                    godArmor.name = "[DEBUG] Invincible Plate";
                    godArmor.description = "Debug armor that makes you nearly invincible.";
                    godArmor.type = ItemType::ARMOR;
                    godArmor.rarity = ItemRarity::LEGENDARY;
                    godArmor.defenseBonus = 999;
                    godArmor.quantity = 1;
                    godArmor.value = 999999;
                    godArmor.isUsable = false;
                    godArmor.isStackable = false;
                    m_gameState.player.inventory.push_back(godArmor);
                    std::cout << "\n" << Color::BRIGHT_BLUE << "🛡️ Invincible Plate added to inventory!" << Color::RESET << "\n";
                    waitForEnter();
                }
                break;
        }
    }
}

void GameEngine::displayTextBox(const std::string& title, const std::string& text) {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString(title, 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    std::cout << wrapText(text, 78) << "\n\n";
    displayDivider(80, '=');
}

void GameEngine::displayDivider(int length, char character) {
    displaySeparator(length, character);
}

void GameEngine::clearScreen() {
    AQE::clearScreen();
}

void GameEngine::waitForEnter() {
    pause();
}

// ---
// ENDING SYSTEM
// ---

EndingType GameEngine::determineEnding() {
    const Player& p = m_gameState.player;
    
    // Death endings
    if (!p.isAlive) {
        return EndingType::DEFEAT_DEATH;
    }
    
    // Victory endings
    if (p.hasWon) {
        // Surviving deep Maw runs counts as a secret-tier accomplishment.
        if (p.mawDepthReached >= 10) {
            return EndingType::VICTORY_SECRET;
        }

        // Check for secret ending
        if (p.hasSecretEnding || p.secretsFound >= 3) {
            return EndingType::VICTORY_SECRET;
        }
        
        // Check for speedrun
        if (p.totalTurns <= SPEEDRUN_TURN_LIMIT) {
            return EndingType::VICTORY_SPEEDRUN;
        }
        
        // Check for completionist
        if (p.completedQuests.size() >= COMPLETIONIST_QUEST_COUNT) {
            return EndingType::ENDING_COMPLETIONIST;
        }
        
        // Check morality endings
        if (p.moralityScore >= MORALITY_HERO_THRESHOLD) {
            return EndingType::ENDING_HERO;
        } else if (p.moralityScore <= MORALITY_EVIL_THRESHOLD) {
            return EndingType::ENDING_EVIL;
        }
        
        // Check for pacifist
        if (p.totalKills == 0) {
            return EndingType::ENDING_PACIFIST;
        }
        
        // Default victory
        return EndingType::VICTORY_NORMAL;
    }
    
    return EndingType::ENDING_NEUTRAL;
}

void GameEngine::displayEnding(EndingType ending) {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString("YOUR ENDING", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    
    switch (ending) {
        case EndingType::VICTORY_NORMAL:
            std::cout << "NORMAL VICTORY\n\n";
            std::cout << "You have successfully navigated the dungeon and emerged victorious!\n";
            std::cout << "Your courage and skill have carried you through many challenges.\n";
            break;
            
        case EndingType::VICTORY_SECRET:
            std::cout << "SECRET ENDING UNLOCKED\n\n";
            std::cout << "Through your exceptional perception and curiosity, you have\n";
            std::cout << "uncovered the dungeon's greatest secret! Few adventurers ever\n";
            std::cout << "achieve what you have accomplished.\n";
            break;
            
        case EndingType::VICTORY_SPEEDRUN:
            std::cout << "SPEED DEMON\n\n";
            std::cout << "With incredible efficiency, you blazed through the dungeon\n";
            std::cout << "in record time! Your quick thinking and decisive actions\n";
            std::cout << "led to a swift victory.\n";
            break;
            
        case EndingType::ENDING_HERO:
            std::cout << "HERO'S ENDING\n\n";
            std::cout << "Your noble choices and heroic deeds have made you a legend.\n";
            std::cout << "The people will sing songs of your valor for generations to come.\n";
            break;
            
        case EndingType::ENDING_EVIL:
            std::cout << "DARK PATH\n\n";
            std::cout << "Your choices have led you down a dark path. Power came at a price,\n";
            std::cout << "and the dungeon's corruption has taken hold of your soul.\n";
            break;
            
        case EndingType::ENDING_COMPLETIONIST:
            std::cout << "MASTER ADVENTURER\n\n";
            std::cout << "You have completed every quest and explored every corner!\n";
            std::cout << "Your dedication and thoroughness are unmatched.\n";
            break;
            
        case EndingType::ENDING_PACIFIST:
            std::cout << "PEACEFUL RESOLUTION\n\n";
            std::cout << "Through cunning and diplomacy, you navigated the dungeon\n";
            std::cout << "without taking a single life. A true achievement!\n";
            break;
            
        case EndingType::DEFEAT_DEATH:
            std::cout << "FALLEN HERO\n\n";
            std::cout << "Your adventure has come to an end. Though you fell in battle,\n";
            std::cout << "your bravery will not be forgotten.\n";
            break;
            
        default:
            std::cout << "JOURNEY'S END\n\n";
            std::cout << "Your adventure in the dungeon has concluded.\n";
            break;
    }
    
    std::cout << "\n";
    displayDivider(80, '-');
    waitForEnter();
}

int GameEngine::calculateFinalScore() {
    const Player& p = m_gameState.player;
    
    int score = 0;
    
    // Base score from level and gold
    score += p.level * 100;
    score += p.gold;
    
    // Combat achievements
    score += p.totalKills * 10;
    score += p.bossesDefeated * 100;
    
    // Exploration
    score += p.secretsFound * 50;
    score += p.visitedRooms.size() * 5;
    score += p.puzzlesSolved * 75;
    
    // Quests
    score += p.completedQuests.size() * 150;

    // Endgame challenge bonus
    score += p.mawDepthReached * 200;
    
    // Time bonus (fewer turns = higher bonus)
    if (p.totalTurns < 100) {
        score += (100 - p.totalTurns) * 5;
    }
    
    // Survival bonus
    if (p.isAlive) {
        score += 500;
    }
    
    // Special ending bonuses
    if (p.hasSecretEnding) {
        score += 1000;
    }

    clearScreen();
    std::cout << UIManager::createTitle("Final Score", 70, UIColor::BRIGHT_YELLOW) << "\n";
    std::cout << "  Base (Level + Gold): " << (p.level * 100 + p.gold) << "\n";
    std::cout << "  Combat:              " << (p.totalKills * 10 + p.bossesDefeated * 100) << "\n";
    std::cout << "  Exploration:         " << (p.secretsFound * 50 + static_cast<int>(p.visitedRooms.size()) * 5 + p.puzzlesSolved * 75) << "\n";
    std::cout << "  Quests:              " << (static_cast<int>(p.completedQuests.size()) * 150) << "\n";
    std::cout << "  The Maw:             " << (p.mawDepthReached * 200) << "\n\n";
    std::cout << UIManager::colorize("  TOTAL SCORE: " + std::to_string(score), UIColor::BRIGHT_GREEN, UIEffect::BOLD) << "\n\n";
    
    return score;
}

void GameEngine::displayGameStats() {
    const Player& p = m_gameState.player;

    std::cout << "\n" << UIManager::createTitle("Game Statistics", 70, UIColor::BRIGHT_CYAN) << "\n";
    std::cout << "  Character:        " << p.name << " (" << getClassName(p.charClass) << ")\n";
    std::cout << "  Final Level:      " << p.level << "\n";
    std::cout << "  Total Turns:      " << p.totalTurns << "\n";
    std::cout << "  Rooms Visited:    " << p.visitedRooms.size() << "\n";
    std::cout << "  Enemies Defeated: " << p.totalKills << "\n";
    std::cout << "  Bosses Defeated:  " << p.bossesDefeated << "\n";
    std::cout << "  Maw Depth:        " << p.mawDepthReached << "\n";
    std::cout << "  Secrets Found:    " << p.secretsFound << "\n";
    std::cout << "  Puzzles Solved:   " << p.puzzlesSolved << "\n";
    std::cout << "  Quests Completed: " << p.completedQuests.size() << "\n";
    std::cout << "  Morality:         " << p.moralityScore << "\n";
    std::cout << "  Average iLvl:     " << p.averageItemLevel << "\n";
    std::cout << "  Final Gold:       " << p.gold << "\n\n";
}

bool GameEngine::checkLeaderboard(int score) {
    // Load leaderboard and check if score qualifies
    std::vector<LeaderboardEntry> entries = m_fileIO.loadLeaderboard("data/leaderboard.txt");
    
    // If less than 20 entries, always qualifies
    if (entries.size() < 20) {
        return true;
    }
    
    // Check if score beats the lowest entry
    return score > entries.back().score;
}

void GameEngine::displayLeaderboard() {
    clearScreen();
    
    std::cout << UIManager::createTitle("🏆 HALL OF LEGENDS 🏆", 80) << "\n\n";
    
    // Use FileIO's display function
    m_fileIO.displayLeaderboard("data/leaderboard.txt", 15);
    
    std::cout << "\n";
    waitForEnter();
}

// ---
// SAVE / LOAD
// ---

bool GameEngine::saveGame(const std::string& filename) {
    return m_fileIO.saveGame(m_gameState, filename);
}

bool GameEngine::loadGame(const std::string& filename) {
    if (!loadGameData()) {
        std::cerr << "Warning: Failed to load all game data before loading save.\n";
    }

    if (!m_fileIO.loadGame(m_gameState, filename)) {
        return false;
    }

    if (!m_gameState.dungeon.empty()) {
        const int width = (m_gameState.mapWidth > 0) ? m_gameState.mapWidth : DEFAULT_MAP_WIDTH;
        const int height = (m_gameState.mapHeight > 0) ? m_gameState.mapHeight : DEFAULT_MAP_HEIGHT;

        m_mapGenerator = MapGenerator(width, height);
        m_mapGenerator.syncFromDungeon(m_gameState.dungeon);

        if (m_gameState.roomMap.size() != m_gameState.dungeon.size()) {
            m_gameState.roomMap.clear();
            for (Room& room : m_gameState.dungeon) {
                m_gameState.roomMap[room.id] = &room;
            }
        }
    }

    bool hasCurrentRoom = false;
    for (const Room& room : m_gameState.dungeon) {
        if (room.id == m_gameState.player.currentRoomId) {
            hasCurrentRoom = true;
            break;
        }
    }

    if (!hasCurrentRoom && !m_gameState.dungeon.empty()) {
        m_gameState.player.currentRoomId = m_mapGenerator.getEntranceRoomId();
    }

    if (m_gameState.player.mawUnlocked) {
        unlockMawPortal();
    }

    triggerShadowLordEncounterIfNeeded();

    return true;
}

std::vector<std::string> GameEngine::listSaveFiles() {
    return m_fileIO.listSaveFiles();
}

std::string GameEngine::selectSaveFileForLoad() {
    std::vector<std::string> saveFiles = listSaveFiles();

    if (saveFiles.empty()) {
        std::cout << "\nNo save files found.\n";
        return "";
    }

    std::cout << "\n=== Load Save ===\n\n";
    for (size_t i = 0; i < saveFiles.size(); ++i) {
        std::cout << (i + 1) << ". " << getSaveDisplayName(saveFiles[i])
                  << " (" << saveFiles[i] << ")\n";
    }
    std::cout << (saveFiles.size() + 1) << ". Cancel\n\n";

    int choice = getValidatedInt("Choose a save: ", 1, static_cast<int>(saveFiles.size() + 1));
    if (choice == static_cast<int>(saveFiles.size() + 1)) {
        return "";
    }

    return saveFiles[choice - 1];
}

std::string GameEngine::selectSaveFileForSave() {
    std::vector<std::string> saveFiles = listSaveFiles();

    std::cout << "\n=== Save Game ===\n";
    std::cout << "Current slot: " << getSaveDisplayName(m_activeSaveFile)
              << " (" << m_activeSaveFile << ")\n";

    if (!saveFiles.empty()) {
        std::cout << "\nExisting slots:\n";
        for (size_t i = 0; i < saveFiles.size(); ++i) {
            std::cout << "  - " << getSaveDisplayName(saveFiles[i]) << "\n";
        }
    }

    std::cout << "\nEnter save slot name (blank = current, 'cancel' to abort): ";
    std::string slotName;
    std::getline(std::cin, slotName);
    slotName = trim(slotName);

    if (toLower(slotName) == "cancel") {
        return "";
    }

    if (slotName.empty()) {
        return m_activeSaveFile.empty() ? SAVE_FILE : m_activeSaveFile;
    }

    std::string targetFile = DATA_FOLDER + "save_" + sanitizeSaveSlotName(slotName) + ".txt";

    if (m_fileIO.saveExists(targetFile)) {
        std::cout << "Save slot already exists. Overwrite?\n";
        if (!getYesNo("Confirm overwrite")) {
            return "";
        }
    }

    return targetFile;
}

std::string GameEngine::sanitizeSaveSlotName(const std::string& slotName) const {
    std::string cleaned;
    cleaned.reserve(slotName.size());

    for (char ch : slotName) {
        unsigned char c = static_cast<unsigned char>(ch);
        if (std::isalnum(c) != 0 || ch == '_' || ch == '-') {
            cleaned.push_back(static_cast<char>(std::tolower(c)));
        } else if (std::isspace(c) != 0) {
            cleaned.push_back('_');
        }
    }

    if (cleaned.empty()) {
        return "adventure";
    }

    return cleaned;
}

// ---
// UTILITY METHODS
// ---

void GameEngine::logEvent(const std::string& message) {
    m_gameState.player.gameLog.push_back(message);
}

std::string GameEngine::getClassName(CharacterClass charClass) {
    switch (charClass) {
        case CharacterClass::WARRIOR: return "Warrior";
        case CharacterClass::MAGE: return "Mage";
        case CharacterClass::ROGUE: return "Rogue";
        case CharacterClass::PALADIN: return "Paladin";
        case CharacterClass::RANGER: return "Ranger";
        case CharacterClass::NECROMANCER: return "Necromancer";
        case CharacterClass::BARBARIAN: return "Barbarian";
        case CharacterClass::MONK: return "Monk";
        default: return "Unknown";
    }
}

std::string GameEngine::getRoomTypeName(RoomType type) {
    switch (type) {
        case RoomType::NORMAL: return "Normal Room";
        case RoomType::TREASURE: return "Treasure Room";
        case RoomType::BOSS: return "Boss Room";
        case RoomType::TRAP: return "Trap Room";
        case RoomType::PUZZLE: return "Puzzle Room";
        case RoomType::SECRET: return "Secret Room";
        case RoomType::SAFE: return "Safe Room";
        case RoomType::ENTRANCE: return "Entrance";
        case RoomType::EXIT: return "Exit";
        case RoomType::LIBRARY: return "Library";
        case RoomType::FORGE: return "Forge";
        case RoomType::SHRINE: return "Shrine";
        case RoomType::LABORATORY: return "Laboratory";
        case RoomType::GRAVEYARD: return "Graveyard";
        case RoomType::ARMORY: return "Armory";
        case RoomType::PRISON: return "Prison";
        case RoomType::SHADOWLORD: return "Shadow Lord Chamber";
        default: return "Unknown";
    }
}

std::string GameEngine::getRarityName(ItemRarity rarity) {
    return rarityToString(rarity);
}

void GameEngine::initializeRandom(unsigned int seed) {
    initRandom(seed);
}

// ---
// GETTERS
// ---

GameState& GameEngine::getGameState() {
    return m_gameState;
}

const GameState& GameEngine::getGameState() const {
    return m_gameState;
}

bool GameEngine::isRunning() const {
    return m_isRunning;
}

// ---
// PRIVATE HELPER METHODS
// ---

bool GameEngine::loadGameData() {
    return m_fileIO.loadAllGameData(m_gameState);
}

void GameEngine::initializeGame() {
    m_gameState.player.isAlive = true;
    m_gameState.player.hasWon = false;
    m_gameState.player.shadowLordDefeated = false;
    m_gameState.player.mawUnlocked = false;
    m_gameState.player.mawDepthReached = 0;
    m_gameState.player.totalTurns = 0;
}

void GameEngine::checkRoomEvents() {
    Room& room = getCurrentRoom();
    
    // Check for shop encounter
    if (room.type == RoomType::SHOP && !room.isCleared) {
        handleShopEncounter();
        room.isCleared = true;
        return;
    }

    if (room.type == RoomType::FORGE && !room.isCleared) {
        handleForgeEncounter();
        room.isCleared = true;
        return;
    }

    if (room.type == RoomType::LABORATORY && !room.isCleared) {
        handleLaboratoryEncounter();
        room.isCleared = true;
        return;
    }

    if (room.type == RoomType::LIBRARY && !room.isCleared) {
        handleLibraryEncounter();
        room.isCleared = true;
        return;
    }
    
    // Process room events
    if (!room.eventIds.empty()) {
        std::vector<EventResult> results = m_eventSystem.processRoomEvents(room);
        
        for (const auto& result : results) {
            if (result.occurred) {
                std::cout << "\n" << result.message << "\n";
                
                // Apply effects
                m_gameState.player.stats.currentHP += result.hpChange;
                if (m_gameState.player.stats.currentHP > m_gameState.player.stats.maxHP) {
                    m_gameState.player.stats.currentHP = m_gameState.player.stats.maxHP;
                }
                if (m_gameState.player.stats.currentHP <= 0) {
                    m_gameState.player.isAlive = false;
                }
                
                m_gameState.player.gold += result.goldChange;
                m_gameState.player.moralityScore += result.moralityChange;
                m_gameState.player.experience += result.experienceGained;
                
                // Add items
                for (const auto& item : result.itemsGained) {
                    m_gameState.player.inventory.push_back(item);
                }
                
                // Trigger combat if needed
                if (result.triggersCombat) {
                    // Find enemy and start combat
                    for (auto& enemy : m_gameState.enemyTemplates) {
                        if (enemy.id == result.combatEnemyId) {
                            std::vector<Enemy> enemies = {enemy};
                            CombatResult combatResult = m_combatSystem.startCombat(enemies);
                            
                            if (combatResult.playerDied) {
                                m_gameState.player.isAlive = false;
                            } else if (combatResult.playerWon) {
                                m_gameState.player.experience += combatResult.experienceGained;
                                m_gameState.player.gold += combatResult.goldGained;
                                m_gameState.player.totalKills++;
                            }
                            break;
                        }
                    }
                }
                
                waitForEnter();
            }
        }
    }
    
    // Start combat if enemies present
    if (!room.enemies.empty() && !room.isCleared) {
        std::cout << "\nEnemies attack!\n";
        waitForEnter();
        
        CombatResult result = m_combatSystem.startCombat(room.enemies);
        
        if (result.playerDied) {
            m_gameState.player.isAlive = false;
        } else if (result.playerWon) {
            room.isCleared = true;
            room.enemies.clear();
            m_gameState.player.experience += result.experienceGained;
            m_gameState.player.gold += result.goldGained;
            m_gameState.player.totalKills += result.experienceGained / 10;  // Rough estimate

            // Reward crafting materials for cleared combat encounters.
            m_craftingSystem.addMaterial(1, randomInt(1, 2));  // Iron Ore
            if (randomChance(40)) {
                m_craftingSystem.addMaterial(8, 1);  // Monster Bone
            }
            if (randomChance(20)) {
                m_craftingSystem.addMaterial(6, 1);  // Magical Essence
            }
            
            // Add loot to room
            for (const auto& item : result.loot) {
                room.items.push_back(item);
            }
            
            std::cout << "\nYou gained " << result.experienceGained << " XP and " 
                      << result.goldGained << " gold!\n";
            waitForEnter();
        }
    }
}

void GameEngine::handleLibraryEncounter() {
    clearScreen();
    std::cout << "\n=== Ancient Library ===\n\n";
    std::cout << "You scan dusty tomes filled with forgotten crafting techniques.\n";

    std::vector<int> recipePool = {1003, 1011, 1021, 1031, 1041, 1103, 1110, 1131, 1302, 1310};
    int recipeId = recipePool[randomInt(0, static_cast<int>(recipePool.size()) - 1)];
    m_craftingSystem.discoverRecipe(recipeId);

    m_craftingSystem.addMaterial(4, randomInt(1, 3));  // Cloth scraps
    if (randomChance(35)) {
        m_craftingSystem.addMaterial(6, 1);  // Essence
    }

    waitForEnter();
}

void GameEngine::handleForgeEncounter() {
    clearScreen();
    std::cout << "\n=== Ancient Forge ===\n\n";
    std::cout << "The forge still burns. You salvage useful materials and prepare to craft.\n";

    m_craftingSystem.addMaterial(1, randomInt(2, 5));  // Iron Ore
    m_craftingSystem.addMaterial(2, randomInt(1, 3));  // Wood
    if (randomChance(30)) {
        m_craftingSystem.addMaterial(5, 1);  // Gem
    }

    m_craftingSystem.runCraftingMenu();
}

void GameEngine::handleLaboratoryEncounter() {
    clearScreen();
    std::cout << "\n=== Arcane Laboratory ===\n\n";
    std::cout << "Shelves of herbs and reagents line the walls.\n";

    m_craftingSystem.addMaterial(7, randomInt(2, 5));  // Herbs
    if (randomChance(50)) {
        m_craftingSystem.addMaterial(6, 1);  // Essence
    }

    m_craftingSystem.discoverRecipe(1302);  // Greater Health Potion
    m_craftingSystem.runCraftingMenu();
}

void GameEngine::handleShopEncounter() {
    clearScreen();
    
    std::cout << "\n";
    std::cout << UIManager::createTitle("🛍 YOU DISCOVERED A SHOP! 🛍", 70, UIColor::BRIGHT_YELLOW);
    std::cout << "\n";
    
    Room& currentRoom = getCurrentRoom();
    VendorType vendorType;
    int dungeonLevel = m_gameState.player.level;

    if (currentRoom.name == "Basic Equipment Shop") {
        vendorType = VendorType::GENERAL_STORE;
        dungeonLevel = 1; // Only basic equipment
    } else {
        // Determine vendor type based on dungeon level and randomness
        int roll = randomInt(1, 100);
        
        if (roll <= 40) {
            vendorType = VendorType::GENERAL_STORE;
        } else if (roll <= 60) {
            vendorType = VendorType::APOTHECARY;
        } else if (roll <= 75) {
            vendorType = VendorType::WEAPONSMITH;
        } else if (roll <= 85) {
            vendorType = VendorType::ARMORER;
        } else if (roll <= 93) {
            vendorType = VendorType::ENCHANTER;
        } else if (roll <= 97) {
            vendorType = VendorType::TRAVELING;
        } else {
            vendorType = VendorType::BLACK_MARKET;
        }
    }
    
    // Create vendor inventory
    MerchantInventory merchant = m_merchantSystem.createVendor(vendorType, dungeonLevel);
    
    // Run shop menu
    m_merchantSystem.runShopMenu(merchant);
    
    clearScreen();
}

void GameEngine::updateStatusEffects() {
    auto& effects = m_gameState.player.activeEffects;
    
    for (auto it = effects.begin(); it != effects.end(); ) {
        it->turnsRemaining--;
        
        if (it->turnsRemaining <= 0) {
            std::cout << "\n" << it->name << " has worn off.\n";
            it = effects.erase(it);
        } else {
            ++it;
        }
    }
}

void GameEngine::checkQuestProgress() {
    // Quest ID constants for maintainability
    constexpr int MAIN_QUEST_ID = 1;            // Defeat the Shadow Lord
    constexpr int MONSTER_SLAYER_QUEST_ID = 2;  // Kill enemy count quest
    constexpr int EXPLORER_QUEST_ID = 3;        // Room exploration quest
    
    (void)MAIN_QUEST_ID;  // Main quest tracked separately via boss defeat
    
    // Check kill quest progress
    for (auto& quest : m_gameState.player.activeQuests) {
        if (quest.status != QuestStatus::IN_PROGRESS) continue;
        
        // Monster Slayer quest - track kills
        if (quest.id == MONSTER_SLAYER_QUEST_ID && quest.targetEnemyCount > 0) {
            quest.currentEnemyCount = m_gameState.player.totalKills;
            if (quest.currentEnemyCount >= quest.targetEnemyCount && 
                quest.status == QuestStatus::IN_PROGRESS) {
                quest.status = QuestStatus::COMPLETED;
                m_gameState.player.gold += quest.goldReward;
                m_gameState.player.experience += quest.experienceReward;
                
                std::cout << "\n" << Color::GREEN << "*** QUEST COMPLETE: " << quest.name << " ***" << Color::RESET << "\n";
                std::cout << "Rewards: " << quest.goldReward << " gold, " << quest.experienceReward << " XP!\n";
                waitForEnter();
            }
        }
        
        // Explorer quest - track rooms visited
        if (quest.id == EXPLORER_QUEST_ID && quest.targetEnemyCount > 0) {
            quest.currentEnemyCount = static_cast<int>(m_gameState.player.visitedRooms.size());
            if (quest.currentEnemyCount >= quest.targetEnemyCount && 
                quest.status == QuestStatus::IN_PROGRESS) {
                quest.status = QuestStatus::COMPLETED;
                m_gameState.player.gold += quest.goldReward;
                m_gameState.player.experience += quest.experienceReward;
                
                std::cout << "\n" << Color::GREEN << "*** QUEST COMPLETE: " << quest.name << " ***" << Color::RESET << "\n";
                std::cout << "Rewards: " << quest.goldReward << " gold, " << quest.experienceReward << " XP!\n";
                waitForEnter();
            }
        }
    }
    
    // Check for level up
    checkLevelUp();
}

void GameEngine::checkLevelUp() {
    while (m_gameState.player.experience >= m_gameState.player.experienceToLevel) {
        m_gameState.player.experience -= m_gameState.player.experienceToLevel;
        m_gameState.player.level++;
        
        // Increase experience needed for next level
        m_gameState.player.experienceToLevel = static_cast<int>(
            BASE_XP_TO_LEVEL * std::pow(XP_MULTIPLIER_PER_LEVEL, m_gameState.player.level - 1));
        
        // Stat increases based on class
        int hpGain = 10;
        int manaGain = 5;
        int atkGain = 2;
        int defGain = 1;
        
        switch (m_gameState.player.charClass) {
            case CharacterClass::WARRIOR:
                hpGain = 15;
                atkGain = 3;
                defGain = 2;
                break;
            case CharacterClass::MAGE:
                manaGain = 15;
                m_gameState.player.stats.magicPower += 3;
                break;
            case CharacterClass::ROGUE:
                m_gameState.player.stats.critChance += 2;
                m_gameState.player.stats.speed += 2;
                break;
            case CharacterClass::PALADIN:
                hpGain = 12;
                manaGain = 8;
                defGain = 2;
                break;
            case CharacterClass::RANGER:
                hpGain = 11;
                manaGain = 7;
                atkGain = 3;
                m_gameState.player.stats.critChance += 1;
                m_gameState.player.stats.speed += 2;
                break;
            case CharacterClass::NECROMANCER:
                hpGain = 9;
                manaGain = 14;
                m_gameState.player.stats.magicPower += 3;
                break;
            case CharacterClass::BARBARIAN:
                hpGain = 18;
                manaGain = 3;
                atkGain = 4;
                defGain = 2;
                break;
            case CharacterClass::MONK:
                hpGain = 12;
                manaGain = 9;
                atkGain = 2;
                defGain = 2;
                m_gameState.player.stats.dodgeChance += 2;
                m_gameState.player.stats.speed += 2;
                break;
        }
        
        m_gameState.player.stats.maxHP += hpGain;
        m_gameState.player.stats.currentHP += hpGain;
        m_gameState.player.stats.maxMana += manaGain;
        m_gameState.player.stats.currentMana += manaGain;
        m_gameState.player.stats.attack += atkGain;
        m_gameState.player.stats.defense += defGain;
        
        // Show level up message
        clearScreen();
        std::cout << Color::BRIGHT_YELLOW;
        std::cout << R"(
    ╔═══════════════════════════════════════╗
    ║          LEVEL UP!                    ║
    ╚═══════════════════════════════════════╝
        )" << "\n";
        std::cout << Color::RESET;
        
        std::cout << "  Congratulations! You are now level " << m_gameState.player.level << "!\n\n";
        std::cout << "  Stat Increases:\n";
        std::cout << "    Max HP: +" << hpGain << " (Now: " << m_gameState.player.stats.maxHP << ")\n";
        std::cout << "    Max Mana: +" << manaGain << " (Now: " << m_gameState.player.stats.maxMana << ")\n";
        std::cout << "    Attack: +" << atkGain << " (Now: " << m_gameState.player.stats.attack << ")\n";
        std::cout << "    Defense: +" << defGain << " (Now: " << m_gameState.player.stats.defense << ")\n";
        std::cout << "\n  Your health and mana have been restored!\n\n";
        
        // Restore some health on level up
        m_gameState.player.stats.currentHP = m_gameState.player.stats.maxHP;
        m_gameState.player.stats.currentMana = m_gameState.player.stats.maxMana;
        
        waitForEnter();
    }
}

Room& GameEngine::getCurrentRoom() {
    for (auto& room : m_gameState.dungeon) {
        if (room.id == m_gameState.player.currentRoomId) {
            return room;
        }
    }

    static Room fallbackRoom;
    return fallbackRoom;
}

const Room& GameEngine::getCurrentRoom() const {
    for (const auto& room : m_gameState.dungeon) {
        if (room.id == m_gameState.player.currentRoomId) {
            return room;
        }
    }

    static Room fallbackRoom;
    return fallbackRoom;
}

int GameEngine::getActiveExitRoomId() const {
    for (const auto& room : m_gameState.dungeon) {
        if (room.type == RoomType::SHADOWLORD) {
            return room.id;
        }
    }

    for (const auto& room : m_gameState.dungeon) {
        if (room.type == RoomType::EXIT) {
            return room.id;
        }
    }

    return m_mapGenerator.getExitRoomId();
}

int GameEngine::getActiveShadowLordRoomId() const {
    for (const auto& room : m_gameState.dungeon) {
        if (room.type == RoomType::SHADOWLORD) {
            return room.id;
        }
    }

    return getActiveExitRoomId();
}

bool GameEngine::triggerFinalBoss() {
    // Create the Shadow Lord - an epic final boss!
    Enemy shadowLord;
    shadowLord.id = 9999;
    shadowLord.name = "The Shadow Lord";
    shadowLord.description = "The ancient evil that has terrorized the realm for centuries.";
    shadowLord.dialogue = "Your soul will join my collection of fallen heroes!";
    shadowLord.isBoss = true;
    
    // Scale boss stats based on player level for a fair but challenging fight
    int playerLevel = m_gameState.player.level;
    int baseMult = std::max(1, playerLevel);
    
    shadowLord.stats.maxHP = 150 + (baseMult * 30);
    shadowLord.stats.currentHP = shadowLord.stats.maxHP;
    shadowLord.stats.attack = 15 + (baseMult * 3);
    shadowLord.stats.defense = 10 + (baseMult * 2);
    shadowLord.stats.critChance = 15;
    shadowLord.stats.dodgeChance = 10;
    shadowLord.stats.speed = 12 + playerLevel;
    shadowLord.stats.magicPower = 20 + (baseMult * 2);
    shadowLord.stats.magicResist = 15 + baseMult;
    
    shadowLord.experienceReward = 500 + (playerLevel * 100);
    shadowLord.goldReward = 300 + (playerLevel * 50);
    
    // Add boss abilities
    EnemyAbility shadowBolt;
    shadowBolt.name = "Shadow Bolt";
    shadowBolt.description = "Launches a bolt of pure darkness";
    shadowBolt.damage = 25 + (playerLevel * 5);
    shadowBolt.cooldown = 3;
    shadowBolt.currentCooldown = 0;
    shadowBolt.triggerChance = 0.4f;
    shadowLord.abilities.push_back(shadowBolt);
    
    EnemyAbility darkHeal;
    darkHeal.name = "Dark Regeneration";
    darkHeal.description = "Absorbs dark energy to heal";
    darkHeal.heals = true;
    darkHeal.healAmount = 30 + (playerLevel * 5);
    darkHeal.cooldown = 5;
    darkHeal.currentCooldown = 2;
    darkHeal.triggerChance = 0.3f;
    shadowLord.abilities.push_back(darkHeal);
    
    // Show boss intro with ASCII art
    clearScreen();
    std::cout << Color::BRIGHT_RED;
    std::cout << R"(
                          ___
                      .-'`   `'-.
                     /           \
                    |  .-------.  |
                    | |  X   X  | |
                    |  `._____.'  |
                     \   | | |   /
                      \  |===|  /
                       `._____.`
                        /|   |\
                       / |   | \
                      /  |   |  \
                         |   |
                        /|   |\
                       / |   | \
                      /__|   |__\
    )" << "\n";
    std::cout << Color::RESET;
    
    displayDivider(60, '=');
    std::cout << Color::RED << centerString("THE SHADOW LORD", 60) << Color::RESET << "\n";
    displayDivider(60, '=');
    std::cout << "\n";
    std::cout << "HP: " << shadowLord.stats.currentHP << "/" << shadowLord.stats.maxHP << "\n";
    std::cout << "ATK: " << shadowLord.stats.attack << " | DEF: " << shadowLord.stats.defense << "\n";
    std::cout << "\n" << Color::YELLOW << "This is the FINAL BATTLE! Prepare yourself!" << Color::RESET << "\n\n";
    waitForEnter();
    
    // Start the epic boss battle!
    std::vector<Enemy> bossBattle = { shadowLord };
    CombatResult result = m_combatSystem.startCombat(bossBattle);
    
    if (result.playerDied) {
        m_gameState.player.isAlive = false;
        return false;
    }
    
    if (result.playerWon) {
        m_gameState.player.experience += result.experienceGained;
        m_gameState.player.gold += result.goldGained;
        m_gameState.player.bossesDefeated++;
        m_gameState.player.totalKills++;
        m_gameState.player.shadowLordDefeated = true;
        m_gameState.player.mawUnlocked = true;
        
        // Give legendary loot!
        Item legendaryWeapon;
        legendaryWeapon.id = 99001;
        legendaryWeapon.name = "Shadow Lord's Blade";
        legendaryWeapon.description = "The dark blade of the fallen Shadow Lord, now purified by your victory.";
        legendaryWeapon.type = ItemType::WEAPON;
        legendaryWeapon.rarity = ItemRarity::LEGENDARY;
        legendaryWeapon.attackBonus = 25;
        legendaryWeapon.critBonus = 15;
        legendaryWeapon.speedBonus = 5;
        legendaryWeapon.value = 5000;
        legendaryWeapon.quantity = 1;
        const int legendaryTargetLevel = rollItemLevel(
            std::max(1, m_gameState.player.level + 6),
            ItemRarity::LEGENDARY,
            true
        );
        applyItemLevelScaling(legendaryWeapon, legendaryTargetLevel, true);
        m_gameState.player.inventory.push_back(legendaryWeapon);
        
        clearScreen();
        std::cout << "\n" << Color::BRIGHT_GREEN << "*** LEGENDARY ITEM OBTAINED! ***" << Color::RESET << "\n";
        std::cout << Color::YELLOW << legendaryWeapon.name << Color::RESET << "\n";
        std::cout << legendaryWeapon.description << "\n\n";
        waitForEnter();

        completeMainQuestAfterShadowLord();
        unlockMawPortal();
        
        return true;
    }
    
    return false;
}

void GameEngine::triggerShadowLordEncounterIfNeeded() {
    if (m_isInMawRealm) {
        return;
    }

    const int shadowRoomId = getActiveShadowLordRoomId();
    if (m_gameState.player.currentRoomId != shadowRoomId) {
        return;
    }

    if (m_gameState.player.shadowLordDefeated) {
        if (m_gameState.player.mawUnlocked) {
            std::cout << "\nThe anomaly portal hums in the center of the chamber.\n";
            waitForEnter();
        }
        return;
    }

    clearScreen();
    displayDivider(80, '=');
    std::cout << Color::BRIGHT_RED << centerString("THE SHADOW LORD'S CHAMBER", 80) << Color::RESET << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";

    std::cout << Color::YELLOW;
    std::cout << wrapText("The air grows cold and heavy as you enter the final chamber. "
                         "Dark energy swirls around a massive throne, and upon it sits the "
                         "Shadow Lord himself - a towering figure cloaked in darkness, "
                         "eyes burning with ancient malice.", 78) << "\n\n";
    std::cout << Color::RESET;

    std::cout << Color::RED << "\"So, a new fool dares to challenge me? Very well... "
              << "LET US END THIS!\"" << Color::RESET << "\n\n";

    waitForEnter();

    if (triggerFinalBoss()) {
        unlockMawPortal();
        handleMawPortal();
    }
}

void GameEngine::unlockMawPortal() {
    if (!m_gameState.player.mawUnlocked) {
        return;
    }

    const int shadowRoomId = getActiveShadowLordRoomId();
    for (Room& room : m_gameState.dungeon) {
        if (room.id != shadowRoomId) {
            continue;
        }

        room.type = RoomType::SHADOWLORD;
        room.name = "Shadow Lord Chamber - Portal Unsealed";
        room.shortDesc = "A torn rift to The Maw pulses where the Shadow Lord once stood.";
        room.description = "With the Shadow Lord slain, the chamber should be at peace - but a jagged "
                           "anomaly hangs in the air, devouring light and sound. Ancient runes along the "
                           "floor now point toward the breach. Beyond it lies The Maw, an endless death realm.";
        room.isCleared = true;
        break;
    }

    m_mapGenerator.syncFromDungeon(m_gameState.dungeon);
}

Enemy GameEngine::createMawEnemy(int depth, bool bossVariant) const {
    std::vector<Enemy> pool;

    for (const Enemy& candidate : m_gameState.enemyTemplates) {
        if (!candidate.mawOnly) {
            continue;
        }

        if (bossVariant && candidate.isBoss) {
            pool.push_back(candidate);
        } else if (!bossVariant && !candidate.isBoss) {
            pool.push_back(candidate);
        }
    }

    Enemy mawEnemy;
    if (!pool.empty()) {
        mawEnemy = pool[randomInt(0, static_cast<int>(pool.size()) - 1)];
    } else {
        mawEnemy.name = "Abyss Wraith";
        mawEnemy.description = "A formless predator born from the dark between worlds.";
        mawEnemy.stats.maxHP = 140;
        mawEnemy.stats.currentHP = 140;
        mawEnemy.stats.attack = 28;
        mawEnemy.stats.defense = 10;
        mawEnemy.stats.critChance = 18;
        mawEnemy.stats.dodgeChance = 12;
        mawEnemy.stats.speed = 16;
        mawEnemy.stats.magicPower = 25;
        mawEnemy.stats.magicResist = 12;
        mawEnemy.experienceReward = 220;
        mawEnemy.goldReward = 120;
    }

    const float hpScale = (bossVariant ? 2.2f : 1.45f) + (static_cast<float>(depth) * (bossVariant ? 0.18f : 0.12f));
    const float attackScale = (bossVariant ? 1.8f : 1.3f) + (static_cast<float>(depth) * (bossVariant ? 0.12f : 0.08f));
    const float defenseScale = (bossVariant ? 1.55f : 1.18f) + (static_cast<float>(depth) * (bossVariant ? 0.08f : 0.05f));

    mawEnemy.stats.maxHP = std::max(40, static_cast<int>(mawEnemy.stats.maxHP * hpScale));
    mawEnemy.stats.currentHP = mawEnemy.stats.maxHP;
    mawEnemy.stats.attack = std::max(8, static_cast<int>(mawEnemy.stats.attack * attackScale));
    mawEnemy.stats.defense = std::max(4, static_cast<int>(mawEnemy.stats.defense * defenseScale));
    mawEnemy.stats.magicPower = std::max(6, static_cast<int>(mawEnemy.stats.magicPower * attackScale));
    mawEnemy.stats.magicResist = std::max(3, static_cast<int>(mawEnemy.stats.magicResist * defenseScale));
    mawEnemy.stats.speed = std::max(6, mawEnemy.stats.speed + depth + (bossVariant ? 4 : 1));
    mawEnemy.stats.critChance = std::min(MAX_CRIT_CHANCE, mawEnemy.stats.critChance + depth + (bossVariant ? 8 : 3));
    mawEnemy.stats.dodgeChance = std::min(MAX_DODGE_CHANCE, mawEnemy.stats.dodgeChance + depth / 2 + (bossVariant ? 4 : 1));

    mawEnemy.trueDamage = std::max(2, (depth / 2) + (bossVariant ? 5 : 2));
    mawEnemy.isBoss = bossVariant;
    mawEnemy.experienceReward = std::max(80, mawEnemy.experienceReward + depth * (bossVariant ? 120 : 45));
    mawEnemy.goldReward = std::max(40, mawEnemy.goldReward + depth * (bossVariant ? 70 : 25));

    for (EnemyAbility& ability : mawEnemy.abilities) {
        ability.damage += depth * (bossVariant ? 5 : 2);
        ability.healAmount += depth * (bossVariant ? 3 : 1);
        ability.currentCooldown = 0;
        ability.triggerChance = std::min(0.8f, ability.triggerChance + (bossVariant ? 0.18f : 0.10f));
    }

    if (mawEnemy.abilities.empty()) {
        EnemyAbility mawStrike;
        mawStrike.name = "Abyss Rend";
        mawStrike.description = "Tears through resistance with pure void force.";
        mawStrike.damage = 14 + depth * 4;
        mawStrike.cooldown = 2;
        mawStrike.currentCooldown = 0;
        mawStrike.triggerChance = 0.45f;
        mawEnemy.abilities.push_back(mawStrike);
    }

    mawEnemy.nextIntent = bossVariant ? EnemyIntent::CHARGE : EnemyIntent::CONTROL;
    mawEnemy.intentStrength = mawEnemy.stats.attack + mawEnemy.trueDamage;
    if (toLower(mawEnemy.name).find("maw") == std::string::npos) {
        mawEnemy.name = (bossVariant ? "Maw Sovereign " : "Maw ") + mawEnemy.name;
    }
    mawEnemy.dialogue = "The Maw hungers.";

    return mawEnemy;
}

bool GameEngine::runMawExpedition() {
    const std::vector<Room> originalDungeon = m_gameState.dungeon;
    const int originalRoomId = m_gameState.player.currentRoomId;
    const std::vector<int> originalVisitedRooms = m_gameState.player.visitedRooms;
    const int originalDungeonLevel = m_gameState.currentDungeonLevel;
    const unsigned int originalSeed = m_gameState.randomSeed;

    m_leaveMawRequested = false;
    m_isInMawRealm = true;
    m_currentMawStratum = std::max(1, m_gameState.player.mawDepthReached + 1);

    if (!generateMawStratum()) {
        m_isInMawRealm = false;
        m_leaveMawRequested = false;
        m_currentMawStratum = 0;
        m_gameState.dungeon = originalDungeon;
        m_gameState.player.currentRoomId = originalRoomId;
        m_gameState.player.visitedRooms = originalVisitedRooms;
        m_gameState.currentDungeonLevel = originalDungeonLevel;
        m_gameState.randomSeed = originalSeed;
        m_mapGenerator.syncFromDungeon(m_gameState.dungeon);
        std::cout << "\nThe Maw resists your entry and collapses back into static.\n";
        waitForEnter();
        return true;
    }

    clearScreen();
    displayDivider(80, '=');
    std::cout << Color::BRIGHT_MAGENTA << centerString("THE MAW", 80) << Color::RESET << "\n";
    displayDivider(80, '=');
    std::cout << "\n";
    std::cout << wrapText("The layout shifts into a hostile mirror of the main dungeon. "
                          "Every Descent Rift leads to a deeper stratum, so the Maw can continue forever. "
                          "Type 'extract' at any time to return to the portal room.", 78)
              << "\n\n";
    waitForEnter();

    while (m_isRunning && m_gameState.player.isAlive && !m_gameState.player.hasWon && !m_leaveMawRequested) {
        updateStatusEffects();
        checkQuestProgress();

        m_gameState.player.explorationTurns++;
        m_gameState.player.totalTurns = m_gameState.player.explorationTurns + m_gameState.player.combatTurns;

        displayCurrentRoom();

        std::string action = getExplorationAction();
        if (!processAction(action)) {
            std::cout << "\nInvalid action. Type 'help' for commands.\n";
            waitForEnter();
        }

        if (!m_leaveMawRequested && m_gameState.player.isAlive && !m_gameState.player.hasWon) {
            checkRoomEvents();
        }
    }

    if (m_gameState.player.isAlive) {
        m_gameState.player.mawDepthReached = std::max(
            m_gameState.player.mawDepthReached,
            std::max(1, m_currentMawStratum)
        );
    }

    const bool survived = m_gameState.player.isAlive;

    m_isInMawRealm = false;
    m_leaveMawRequested = false;
    m_currentMawStratum = 0;

    m_gameState.dungeon = originalDungeon;
    m_gameState.player.currentRoomId = originalRoomId;
    m_gameState.player.visitedRooms = originalVisitedRooms;
    m_gameState.currentDungeonLevel = originalDungeonLevel;
    m_gameState.randomSeed = originalSeed;
    m_mapGenerator.syncFromDungeon(m_gameState.dungeon);

    return survived;
}

bool GameEngine::generateMawStratum() {
    unsigned int mawSeed = static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    mawSeed ^= static_cast<unsigned int>(m_currentMawStratum * 2654435761u);
    if (mawSeed == 0) {
        mawSeed = 1;
    }

    m_gameState.randomSeed = mawSeed;
    initializeRandom(mawSeed);
    m_gameState.currentDungeonLevel = std::max(2, m_gameState.player.level + m_currentMawStratum);

    if (!m_mawMapGenerator.generateMap(m_gameState, true)) {
        return false;
    }

    m_gameState.player.currentRoomId = m_mawMapGenerator.getEntranceRoomId();
    m_gameState.player.visitedRooms.clear();

    return true;
}

bool GameEngine::advanceMawStratumIfNeeded() {
    const Room& room = getCurrentRoom();
    if (room.type != RoomType::EXIT) {
        return true;
    }

    clearScreen();
    displayDivider(80, '=');
    std::cout << Color::BRIGHT_MAGENTA
              << centerString("DESCENT RIFT", 80)
              << Color::RESET << "\n";
    displayDivider(80, '=');
    std::cout << "\n";
    std::cout << wrapText("You step into the rift and feel reality shear apart. "
                          "The Maw folds and rewrites itself below you.", 78)
              << "\n\n";

    ++m_currentMawStratum;

    if (!generateMawStratum()) {
        std::cout << "The next stratum fails to stabilize. You are forced to extract.\n\n";
        m_leaveMawRequested = true;
        waitForEnter();
        return false;
    }

    std::cout << "You descend to Maw Stratum " << m_currentMawStratum << ".\n\n";
    waitForEnter();
    return true;
}

void GameEngine::handleMawPortal() {
    if (!m_gameState.player.mawUnlocked || !m_gameState.player.shadowLordDefeated) {
        std::cout << "\nThe portal remains dormant.\n";
        waitForEnter();
        return;
    }

    while (m_gameState.player.isAlive && !m_gameState.player.hasWon) {
        clearScreen();
        displayDivider(80, '=');
        std::cout << Color::BRIGHT_MAGENTA << centerString("PORTAL ROOM", 80) << Color::RESET << "\n";
        displayDivider(80, '=');
        std::cout << "\n";
        std::cout << "The Shadow Lord is gone, but the anomaly has awakened.\n";
        std::cout << "Deepest Maw stratum reached: " << m_gameState.player.mawDepthReached << "\n\n";
        std::cout << "1. Enter The Maw\n";
        std::cout << "2. Leave the dungeon as victor\n";
        std::cout << "3. Step away for now\n\n";

        int choice = getValidatedInt("Choose your action: ", 1, 3);
        if (choice == 1) {
            bool survived = runMawExpedition();
            if (!survived) {
                return;
            }
        } else if (choice == 2) {
            showVictoryCelebration();
            m_gameState.player.hasWon = true;
            return;
        } else {
            return;
        }
    }
}

void GameEngine::completeMainQuestAfterShadowLord() {
    bool rewarded = false;

    for (auto it = m_gameState.player.activeQuests.begin();
         it != m_gameState.player.activeQuests.end();) {
        if (it->id != 1) {
            ++it;
            continue;
        }

        if (it->status != QuestStatus::COMPLETED) {
            it->status = QuestStatus::COMPLETED;
            m_gameState.player.gold += it->goldReward;
            m_gameState.player.experience += it->experienceReward;
            rewarded = true;
        }

        m_gameState.player.completedQuests.push_back(*it);
        it = m_gameState.player.activeQuests.erase(it);
    }

    if (rewarded) {
        std::cout << "\n" << Color::BRIGHT_GREEN
                  << "*** MAIN QUEST COMPLETE: Defeat the Shadow Lord ***"
                  << Color::RESET << "\n";
        std::cout << "Rewards granted and The Maw has awakened.\n";
        waitForEnter();
        checkLevelUp();
    }
}

void GameEngine::showVictoryCelebration() {
    clearScreen();
    
    // Epic victory ASCII art
    std::cout << Color::BRIGHT_YELLOW;
    std::cout << R"(
    ██╗   ██╗██╗ ██████╗████████╗ ██████╗ ██████╗ ██╗   ██╗██╗
    ██║   ██║██║██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗╚██╗ ██╔╝██║
    ██║   ██║██║██║        ██║   ██║   ██║██████╔╝ ╚████╔╝ ██║
    ╚██╗ ██╔╝██║██║        ██║   ██║   ██║██╔══██╗  ╚██╔╝  ╚═╝
     ╚████╔╝ ██║╚██████╗   ██║   ╚██████╔╝██║  ██║   ██║   ██╗
      ╚═══╝  ╚═╝ ╚═════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝
    )" << "\n";
    std::cout << Color::RESET;
    
    displayDivider(80, '=');
    std::cout << Color::BRIGHT_GREEN << centerString("CONGRATULATIONS, CHAMPION!", 80) << Color::RESET << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    
    std::cout << Color::CYAN;
    std::cout << wrapText("With a final, mighty blow, the Shadow Lord falls! The dark energy "
                         "that had plagued the realm for centuries dissipates into nothing. "
                         "The dungeon begins to shake as the ancient evil's power fades away.", 78) << "\n\n";
    std::cout << Color::RESET;
    
    std::cout << wrapText("You, " + m_gameState.player.name + " the " + 
                         getClassName(m_gameState.player.charClass) + 
                         ", have saved the realm! Songs will be sung of your bravery for "
                         "generations to come. The Council of Light welcomes you as a true hero.", 78) << "\n\n";
    
    std::cout << Color::YELLOW;
    displayDivider(60, '*');
    std::cout << centerString("YOUR HEROIC JOURNEY", 60) << "\n";
    displayDivider(60, '*');
    std::cout << Color::RESET;
    
    std::cout << "\n";
    std::cout << "  " << Color::BRIGHT_WHITE << "Level Achieved:" << Color::RESET << " " << m_gameState.player.level << "\n";
    std::cout << "  " << Color::BRIGHT_WHITE << "Enemies Defeated:" << Color::RESET << " " << m_gameState.player.totalKills << "\n";
    std::cout << "  " << Color::BRIGHT_WHITE << "Bosses Slain:" << Color::RESET << " " << m_gameState.player.bossesDefeated << "\n";
    std::cout << "  " << Color::BRIGHT_WHITE << "Rooms Explored:" << Color::RESET << " " << m_gameState.player.visitedRooms.size() << "\n";
    std::cout << "  " << Color::BRIGHT_WHITE << "Puzzles Solved:" << Color::RESET << " " << m_gameState.player.puzzlesSolved << "\n";
    std::cout << "  " << Color::BRIGHT_WHITE << "Secrets Found:" << Color::RESET << " " << m_gameState.player.secretsFound << "\n";
    std::cout << "  " << Color::BRIGHT_WHITE << "Gold Collected:" << Color::RESET << " " << m_gameState.player.gold << "\n";
    std::cout << "  " << Color::BRIGHT_WHITE << "Total Turns:" << Color::RESET << " " << m_gameState.player.totalTurns << "\n";
    std::cout << "\n";
    
    // Calculate a score
    int score = (m_gameState.player.level * 100) + 
                (m_gameState.player.totalKills * 10) + 
                (m_gameState.player.bossesDefeated * 500) +
                (m_gameState.player.secretsFound * 50) +
                (m_gameState.player.puzzlesSolved * 75) +
                m_gameState.player.gold +
                (500 - std::min(500, m_gameState.player.totalTurns));  // Speed bonus
    
    std::cout << Color::BRIGHT_YELLOW << "  FINAL SCORE: " << score << " POINTS!" << Color::RESET << "\n\n";
    
    displayDivider(80, '=');
    std::cout << Color::GREEN << "\nThank you for playing Adventure Quest Engine!" << Color::RESET << "\n";
    std::cout << "Created with love using C++\n\n";
    
    waitForEnter();
}

}  // namespace AQE
