/*******************************************************************************
 * C++ Adventure Quest Engine
 * GameEngine.cpp - Main Game Engine Implementation
 * 
 * This file contains the implementation of the GameEngine class which
 * coordinates all game systems and provides the main game loop.
 * 
 * Author: Adventure Quest Engine Team
 * Version: 1.0
 ******************************************************************************/

#include "../include/GameEngine.h"
#include "../include/Constants.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace AQE {

//==============================================================================
// CONSTRUCTOR / DESTRUCTOR
//==============================================================================

GameEngine::GameEngine()
    : m_mapGenerator(DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT),
      m_combatSystem(m_gameState.player, m_gameState.difficulty),
      m_inventorySystem(m_gameState.player),
      m_eventSystem(m_gameState.player, m_gameState),
      m_fileIO(DATA_FOLDER),
      m_isRunning(false),
      m_debugMode(false) {
    
    initializeRandom(0);  // Use time-based seed
}

GameEngine::~GameEngine() {
    // Cleanup if needed
}

//==============================================================================
// MAIN GAME FLOW
//==============================================================================

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
                if (loadGame("savegame.txt")) {
                    gameLoop();
                    endGame();
                } else {
                    std::cout << "\nNo save file found or load failed.\n";
                    waitForEnter();
                }
                break;
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
    displayDivider(80, '=');
    std::cout << centerString("MAIN MENU", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    std::cout << "  1. New Game\n";
    std::cout << "  2. Load Game\n";
    std::cout << "  3. Settings\n";
    std::cout << "  4. Leaderboard\n";
    std::cout << "  5. Quit\n\n";
    displayDivider(80, '-');
    
    return getValidatedInt("Enter your choice: ", 1, 5);
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
    if (!m_mapGenerator.generateMap(m_gameState)) {
        std::cout << "ERROR: Failed to generate map!\n";
        waitForEnter();
        return;
    }
    
    // Set starting position
    m_gameState.player.currentRoomId = m_mapGenerator.getEntranceRoomId();
    
    // Initialize game
    initializeGame();
    
    std::cout << "\nYour adventure begins...\n";
    waitForEnter();
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
        
        // Random encounter chance
        if (randomChance(15)) {
            checkRandomEncounter();
        }
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
    
    // Check leaderboard
    if (checkLeaderboard(score)) {
        std::cout << "\nNew high score! You made the leaderboard!\n";
    }
    
    waitForEnter();
}

//==============================================================================
// CHARACTER CREATION
//==============================================================================

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
    std::cout << "\n=== Choose Your Class ===\n\n";
    std::cout << "1. Warrior - High HP and Attack\n";
    std::cout << "2. Mage - High Magic Power\n";
    std::cout << "3. Rogue - High Crit and Dodge\n";
    std::cout << "4. Paladin - Balanced Stats\n\n";
    
    int choice = getValidatedInt("Enter class choice (1-4): ", 1, 4);
    
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
            skill.cooldown = 3;
            skill.damageMultiplier = 1.5f;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill(); // Reset
            skill.name = "Shield Bash";
            skill.description = "Bash the enemy with your shield.";
            skill.manaCost = 15;
            skill.cooldown = 4;
            skill.damageMultiplier = 1.2f;
            m_gameState.player.skills.push_back(skill);
            break;
            
        case CharacterClass::MAGE:
            skill.name = "Fireball";
            skill.description = "Launch a ball of fire.";
            skill.manaCost = 15;
            skill.cooldown = 2;
            skill.damageMultiplier = 2.0f;
            skill.isMagical = true;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Ice Shard";
            skill.description = "Pierce enemy with ice.";
            skill.manaCost = 10;
            skill.cooldown = 1;
            skill.damageMultiplier = 1.5f;
            skill.isMagical = true;
            m_gameState.player.skills.push_back(skill);
            break;
            
        case CharacterClass::ROGUE:
            skill.name = "Backstab";
            skill.description = "Strike from the shadows.";
            skill.manaCost = 20;
            skill.cooldown = 4;
            skill.damageMultiplier = 2.5f;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Quick Slash";
            skill.description = "A fast attack.";
            skill.manaCost = 5;
            skill.cooldown = 0;
            skill.damageMultiplier = 1.2f;
            m_gameState.player.skills.push_back(skill);
            break;
            
        case CharacterClass::PALADIN:
            skill.name = "Holy Smite";
            skill.description = "Smite the enemy with holy light.";
            skill.manaCost = 15;
            skill.cooldown = 3;
            skill.damageMultiplier = 1.5f;
            skill.isMagical = true;
            m_gameState.player.skills.push_back(skill);
            
            skill = Skill();
            skill.name = "Heal";
            skill.description = "Restore health.";
            skill.manaCost = 20;
            skill.cooldown = 4;
            skill.damageMultiplier = 2.0f; // Heals 2x Magic Power
            skill.isHeal = true;
            skill.isMagical = true;
            m_gameState.player.skills.push_back(skill);
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
    }
}

//==============================================================================
// EXPLORATION
//==============================================================================

void GameEngine::displayCurrentRoom() {
    clearScreen();
    
    Room& room = getCurrentRoom();
    
    // Mark room as explored
    if (!room.isExplored) {
        room.isExplored = true;
        m_gameState.player.visitedRooms.push_back(room.id);
    }
    
    displayDivider(80, '=');
    std::cout << centerString(room.name, 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    
    // Display description
    if (room.isExplored && !room.shortDesc.empty()) {
        std::cout << wrapText(room.shortDesc, 78) << "\n\n";
    } else {
        std::cout << wrapText(room.description, 78) << "\n\n";
    }
    
    // Display room type info
    std::cout << "Room Type: " << getRoomTypeName(room.type) << "\n";
    
    // Display enemies
    if (!room.enemies.empty()) {
        std::cout << "\nEnemies present:\n";
        for (const auto& enemy : room.enemies) {
            std::cout << "  - " << enemy.name << " (HP: " << enemy.stats.currentHP << ")\n";
        }
    }
    
    // Display items
    if (!room.items.empty()) {
        std::cout << "\nItems:\n";
        for (size_t i = 0; i < room.items.size(); ++i) {
            std::cout << "  " << (i+1) << ". " << room.items[i].name << "\n";
        }
    }
    
    // Display exits
    std::cout << "\nExits: ";
    if (room.hasNorth) std::cout << "[N]orth ";
    if (room.hasSouth) std::cout << "[S]outh ";
    if (room.hasEast) std::cout << "[E]ast ";
    if (room.hasWest) std::cout << "[W]est ";
    std::cout << "\n";
    
    displayDivider(80, '-');
    
    // Display player status
    showPlayerStatus();
    
    displayDivider(80, '-');
}

std::string GameEngine::getExplorationAction() {
    std::cout << "\nWhat do you do? ";
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
    } else if (action == "menu") {
        int choice = showGameMenu();
        return choice != 0;
    } else if (action == "save") {
        if (saveGame("savegame.txt")) {
            std::cout << "\nGame saved!\n";
        } else {
            std::cout << "\nFailed to save game.\n";
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
    
    // Check if this is the exit
    if (newRoomId == m_mapGenerator.getExitRoomId()) {
        std::cout << "\nYou have found the exit!\n";
        m_gameState.player.hasWon = true;
    }
    
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
        // Add random item (would use item templates if loaded)
        Item hiddenItem;
        hiddenItem.id = randomInt(1000, 2000);
        hiddenItem.name = "Hidden Treasure";
        hiddenItem.description = "A valuable item you found while searching.";
        hiddenItem.value = randomInt(50, 200);
        hiddenItem.type = ItemType::CONSUMABLE;
        room.items.push_back(hiddenItem);
        foundSomething = true;
    }
    
    // Chance to discover secret
    if (room.isSecret && randomChance(40)) {
        std::cout << "\nYou discovered this is a secret room!\n";
        m_gameState.player.secretsFound++;
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
    
    std::cout << "\nYou picked up: " << item.name << "\n";
    waitForEnter();
    
    return true;
}

//==============================================================================
// MENUS & UI
//==============================================================================

int GameEngine::showGameMenu() {
    clearScreen();
    std::cout << "\n=== Game Menu ===\n\n";
    std::cout << "1. Continue\n";
    std::cout << "2. Save Game\n";
    std::cout << "3. Settings\n";
    std::cout << "4. Quit to Main Menu\n\n";
    
    int choice = getValidatedInt("Choice: ", 1, 4);
    
    if (choice == 2) {
        saveGame("savegame.txt");
        std::cout << "\nGame saved!\n";
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
    
    std::cout << "\n" << p.name << " - Level " << p.level << " " << getClassName(p.charClass) << "\n";
    
    // HP bar
    displayProgressBar(p.stats.currentHP, p.stats.maxHP, 30, "HP");
    
    // XP bar
    displayProgressBar(p.experience, p.experienceToLevel, 30, "XP");
    
    std::cout << "Gold: " << p.gold << " | Turns: " << p.totalTurns << "\n";
    
    // Status effects
    if (!p.activeEffects.empty()) {
        std::cout << "Effects: ";
        for (const auto& effect : p.activeEffects) {
            std::cout << effect.name << " (" << effect.turnsRemaining << ") ";
        }
        std::cout << "\n";
    }
}

void GameEngine::openInventory() {
    m_inventorySystem.displayInventoryMenu();
}

void GameEngine::openQuestLog() {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString("QUEST LOG", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    
    if (m_gameState.player.activeQuests.empty()) {
        std::cout << "No active quests.\n";
    } else {
        for (const auto& quest : m_gameState.player.activeQuests) {
            std::cout << "- " << quest.name << "\n";
            std::cout << "  " << quest.description << "\n";
            std::cout << "  Status: ";
            switch (quest.status) {
                case QuestStatus::IN_PROGRESS: std::cout << "In Progress\n"; break;
                case QuestStatus::COMPLETED: std::cout << "Completed\n"; break;
                case QuestStatus::FAILED: std::cout << "Failed\n"; break;
                default: std::cout << "Not Started\n"; break;
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\nCompleted Quests: " << m_gameState.player.completedQuests.size() << "\n";
    
    displayDivider(80, '-');
    waitForEnter();
}

void GameEngine::openMap() {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString("MAP", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    
    m_mapGenerator.printMap();
    
    displayDivider(80, '-');
    waitForEnter();
}

void GameEngine::showHelp() {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString("HELP - COMMANDS", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    std::cout << "Movement:\n";
    std::cout << "  n, north     - Move north\n";
    std::cout << "  s, south     - Move south\n";
    std::cout << "  e, east      - Move east\n";
    std::cout << "  w, west      - Move west\n\n";
    std::cout << "Actions:\n";
    std::cout << "  look         - Examine the room\n";
    std::cout << "  search       - Search for hidden items\n";
    std::cout << "  interact     - Interact with room objects\n";
    std::cout << "  pickup [#]   - Pick up an item\n\n";
    std::cout << "Menus:\n";
    std::cout << "  inventory    - Open inventory\n";
    std::cout << "  status       - View character stats\n";
    std::cout << "  quests       - View quest log\n";
    std::cout << "  map          - View dungeon map\n";
    std::cout << "  menu         - Open game menu\n";
    std::cout << "  help         - Show this help\n\n";
    std::cout << "Other:\n";
    std::cout << "  save         - Save game\n";
    std::cout << "  quit         - Quit game\n";
    
    displayDivider(80, '-');
    waitForEnter();
}

void GameEngine::showSettings() {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString("SETTINGS", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    std::cout << "1. Difficulty: ";
    switch (m_gameState.difficulty) {
        case 1: std::cout << "Easy\n"; break;
        case 2: std::cout << "Normal\n"; break;
        case 3: std::cout << "Hard\n"; break;
    }
    std::cout << "2. Show ASCII Art: " << (m_gameState.showAsciiArt ? "Yes" : "No") << "\n";
    std::cout << "3. Debug Mode: " << (m_debugMode ? "On" : "Off") << "\n";
    std::cout << "4. Back\n\n";
    
    int choice = getValidatedInt("Choice: ", 1, 4);
    
    if (choice == 1) {
        m_gameState.difficulty = getValidatedInt("Difficulty (1=Easy, 2=Normal, 3=Hard): ", 1, 3);
    } else if (choice == 2) {
        m_gameState.showAsciiArt = !m_gameState.showAsciiArt;
    } else if (choice == 3) {
        m_debugMode = !m_debugMode;
        m_gameState.debugMode = m_debugMode;
    }
    
    if (choice != 4) {
        showSettings();  // Recurse to show menu again
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

//==============================================================================
// ENDING SYSTEM
//==============================================================================

EndingType GameEngine::determineEnding() {
    const Player& p = m_gameState.player;
    
    // Death endings
    if (!p.isAlive) {
        return EndingType::DEFEAT_DEATH;
    }
    
    // Victory endings
    if (p.hasWon) {
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
    std::cout << "\n=== FINAL SCORE ===\n\n";
    std::cout << "Base Score (Level & Gold): " << (p.level * 100 + p.gold) << "\n";
    std::cout << "Combat: " << (p.totalKills * 10 + p.bossesDefeated * 100) << "\n";
    std::cout << "Exploration: " << (p.secretsFound * 50 + static_cast<int>(p.visitedRooms.size()) * 5 + p.puzzlesSolved * 75) << "\n";
    std::cout << "Quests: " << (static_cast<int>(p.completedQuests.size()) * 150) << "\n";
    std::cout << "\nTOTAL SCORE: " << score << "\n\n";
    
    return score;
}

void GameEngine::displayGameStats() {
    const Player& p = m_gameState.player;
    
    std::cout << "\n=== GAME STATISTICS ===\n\n";
    std::cout << "Character: " << p.name << " (" << getClassName(p.charClass) << ")\n";
    std::cout << "Final Level: " << p.level << "\n";
    std::cout << "Total Turns: " << p.totalTurns << "\n";
    std::cout << "Rooms Visited: " << p.visitedRooms.size() << "\n";
    std::cout << "Enemies Defeated: " << p.totalKills << "\n";
    std::cout << "Bosses Defeated: " << p.bossesDefeated << "\n";
    std::cout << "Secrets Found: " << p.secretsFound << "\n";
    std::cout << "Puzzles Solved: " << p.puzzlesSolved << "\n";
    std::cout << "Quests Completed: " << p.completedQuests.size() << "\n";
    std::cout << "Morality: " << p.moralityScore << "\n";
    std::cout << "Final Gold: " << p.gold << "\n\n";
}

bool GameEngine::checkLeaderboard(int score) {
    // Simple check - would implement full leaderboard system
    return score > 1000;
}

void GameEngine::displayLeaderboard() {
    clearScreen();
    displayDivider(80, '=');
    std::cout << centerString("LEADERBOARD", 80) << std::endl;
    displayDivider(80, '=');
    std::cout << "\n";
    std::cout << "Top scores will be displayed here.\n";
    std::cout << "(Leaderboard system not yet implemented)\n\n";
    displayDivider(80, '-');
    waitForEnter();
}

//==============================================================================
// SAVE / LOAD
//==============================================================================

bool GameEngine::saveGame(const std::string& filename) {
    return m_fileIO.saveGame(m_gameState, filename);
}

bool GameEngine::loadGame(const std::string& filename) {
    return m_fileIO.loadGame(m_gameState, filename);
}

std::vector<std::string> GameEngine::listSaveFiles() {
    return m_fileIO.listSaveFiles();
}

//==============================================================================
// UTILITY METHODS
//==============================================================================

void GameEngine::logEvent(const std::string& message) {
    m_gameState.player.gameLog.push_back(message);
}

std::string GameEngine::getClassName(CharacterClass charClass) {
    switch (charClass) {
        case CharacterClass::WARRIOR: return "Warrior";
        case CharacterClass::MAGE: return "Mage";
        case CharacterClass::ROGUE: return "Rogue";
        case CharacterClass::PALADIN: return "Paladin";
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
        default: return "Unknown";
    }
}

std::string GameEngine::getRarityName(ItemRarity rarity) {
    return rarityToString(rarity);
}

void GameEngine::initializeRandom(unsigned int seed) {
    initRandom(seed);
}

//==============================================================================
// GETTERS
//==============================================================================

GameState& GameEngine::getGameState() {
    return m_gameState;
}

const GameState& GameEngine::getGameState() const {
    return m_gameState;
}

bool GameEngine::isRunning() const {
    return m_isRunning;
}

//==============================================================================
// PRIVATE HELPER METHODS
//==============================================================================

bool GameEngine::loadGameData() {
    return m_fileIO.loadAllGameData(m_gameState);
}

void GameEngine::initializeGame() {
    m_gameState.player.isAlive = true;
    m_gameState.player.hasWon = false;
    m_gameState.player.totalTurns = 0;
}

void GameEngine::checkRoomEvents() {
    Room& room = getCurrentRoom();
    
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

void GameEngine::checkRandomEncounter() {
    std::cout << "\nA random encounter occurs!\n";
    // Would implement random encounter logic
    waitForEnter();
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
    // Would implement quest progress checking
}

Room& GameEngine::getCurrentRoom() {
    for (auto& room : m_gameState.dungeon) {
        if (room.id == m_gameState.player.currentRoomId) {
            return room;
        }
    }
    
    // Fallback - should never happen
    return m_gameState.dungeon[0];
}

const Room& GameEngine::getCurrentRoom() const {
    for (const auto& room : m_gameState.dungeon) {
        if (room.id == m_gameState.player.currentRoomId) {
            return room;
        }
    }
    
    return m_gameState.dungeon[0];
}

}  // namespace AQE
