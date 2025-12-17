/*******************************************************************************
 * C++ Adventure Quest Engine
 * GameEngine.h - Main Game Engine Header
 * 
 * This file declares the GameEngine class which coordinates all game systems:
 * - Main menu and settings
 * - Character creation
 * - Game loop (explore, combat, events)
 * - Ending determination
 * 
 * Author: Adventure Quest Engine Team
 * Version: 1.0
 ******************************************************************************/

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Types.h"
#include "MapGenerator.h"
#include "Combat.h"
#include "Inventory.h"
#include "Events.h"
#include "FileIO.h"
#include "Utils.h"
#include "Constants.h"

#include <string>
#include <vector>
#include <iostream>

namespace AQE {

/**
 * @class GameEngine
 * @brief Main game engine that coordinates all game systems
 * 
 * This class is the central hub of the adventure game. It manages:
 * - Game state and flow
 * - Player input and output
 * - Menu systems
 * - Character creation
 * - Exploration and movement
 * - Event triggering
 * - Ending determination
 */
class GameEngine {
public:
    //==========================================================================
    // CONSTRUCTOR / DESTRUCTOR
    //==========================================================================
    
    /**
     * @brief Constructs the game engine
     */
    GameEngine();
    
    /**
     * @brief Destructor
     */
    ~GameEngine();
    
    //==========================================================================
    // MAIN GAME FLOW
    //==========================================================================
    
    /**
     * @brief Runs the main game loop
     * @return Exit code (0 for success)
     */
    int run();
    
    /**
     * @brief Displays the intro/title screen with ASCII art
     */
    void showIntro();
    
    /**
     * @brief Displays the main menu
     * @return Menu choice (1=New Game, 2=Load, 3=Options, 4=Credits, 5=Quit)
     */
    int showMainMenu();
    
    /**
     * @brief Starts a new game
     */
    void startNewGame();
    
    /**
     * @brief Main gameplay loop (explore, combat, events)
     */
    void gameLoop();
    
    /**
     * @brief Ends the game and shows appropriate ending
     */
    void endGame();
    
    //==========================================================================
    // CHARACTER CREATION
    //==========================================================================
    
    /**
     * @brief Runs the full character creation process
     * Gets player name, class, and sets initial stats
     */
    void characterCreation();
    
    /**
     * @brief Gets a valid player name from input
     * @return The validated player name
     */
    std::string getPlayerName();
    
    /**
     * @brief Displays class options and gets player choice
     * @return The chosen character class
     */
    CharacterClass chooseClass();
    
    /**
     * @brief Applies base stats for the chosen class
     * @param charClass The chosen class
     */
    void applyClassStats(CharacterClass charClass);
    
    /**
     * @brief Displays class info (stats, abilities, playstyle)
     * @param charClass The class to display info for
     */
    void displayClassInfo(CharacterClass charClass);
    
    //==========================================================================
    // EXPLORATION
    //==========================================================================
    
    /**
     * @brief Shows the current room and available actions
     */
    void displayCurrentRoom();
    
    /**
     * @brief Gets exploration action from player
     * @return Action command (move, look, inventory, etc.)
     */
    std::string getExplorationAction();
    
    /**
     * @brief Processes the player's exploration action
     * @param action The action command
     * @return True if action was valid, false otherwise
     */
    bool processAction(const std::string& action);
    
    /**
     * @brief Attempts to move player in a direction
     * @param direction N/S/E/W
     * @return True if movement successful
     */
    bool movePlayer(char direction);
    
    /**
     * @brief Searches the current room for hidden items/secrets
     */
    void searchRoom();
    
    /**
     * @brief Interacts with room contents (NPCs, objects)
     */
    void interactWithRoom();
    
    /**
     * @brief Picks up an item from the current room
     * @param itemIndex Index of item in room's item list
     * @return True if pickup successful
     */
    bool pickupItem(int itemIndex);
    
    //==========================================================================
    // MENUS & UI
    //==========================================================================
    
    /**
     * @brief Displays the in-game menu
     * @return Menu choice
     */
    int showGameMenu();
    
    /**
     * @brief Shows player status (HP, stats, effects)
     */
    void showPlayerStatus();
    
    /**
     * @brief Opens the inventory menu
     */
    void openInventory();
    
    /**
     * @brief Opens the quest log
     */
    void openQuestLog();
    
    /**
     * @brief Opens the map view
     */
    void openMap();
    
    /**
     * @brief Shows help/controls
     */
    void showHelp();
    
    /**
     * @brief Shows game settings menu
     */
    void showSettings();
    
    /**
     * @brief Displays a text box with story/event text
     * @param title Box title
     * @param text Content to display
     */
    void displayTextBox(const std::string& title, const std::string& text);
    
    /**
     * @brief Displays a divider line
     * @param length Length of the divider
     * @param character Character to use
     */
    void displayDivider(int length = 60, char character = '=');
    
    /**
     * @brief Clears the screen (cross-platform)
     */
    void clearScreen();
    
    /**
     * @brief Pauses and waits for player to press enter
     */
    void waitForEnter();
    
    //==========================================================================
    // ENDING SYSTEM
    //==========================================================================
    
    /**
     * @brief Determines the ending based on player's journey
     * Uses choices, stats, quests, survival, and morality
     * @return The determined ending type
     */
    EndingType determineEnding();
    
    /**
     * @brief Displays the ending sequence
     * @param ending The ending to display
     */
    void displayEnding(EndingType ending);
    
    /**
     * @brief Calculates and displays final score
     * @return The final score
     */
    int calculateFinalScore();
    
    /**
     * @brief Displays game statistics at the end
     */
    void displayGameStats();
    
    /**
     * @brief Checks if player qualifies for the leaderboard
     * @param score The player's score
     * @return True if qualifies
     */
    bool checkLeaderboard(int score);
    
    /**
     * @brief Displays the leaderboard
     */
    void displayLeaderboard();
    
    //==========================================================================
    // SAVE / LOAD
    //==========================================================================
    
    /**
     * @brief Saves the current game state
     * @param filename The save file name
     * @return True if save successful
     */
    bool saveGame(const std::string& filename);
    
    /**
     * @brief Loads a saved game
     * @param filename The save file to load
     * @return True if load successful
     */
    bool loadGame(const std::string& filename);
    
    /**
     * @brief Lists available save files
     * @return Vector of save file names
     */
    std::vector<std::string> listSaveFiles();
    
    //==========================================================================
    // UTILITY METHODS
    //==========================================================================
    
    /**
     * @brief Logs an event to the game log
     * @param message The message to log
     */
    void logEvent(const std::string& message);
    
    /**
     * @brief Gets the class name as a string
     * @param charClass The character class
     * @return The class name
     */
    std::string getClassName(CharacterClass charClass);
    
    /**
     * @brief Gets the room type name as a string
     * @param type The room type
     * @return The room type name
     */
    std::string getRoomTypeName(RoomType type);
    
    /**
     * @brief Gets the rarity name and color code
     * @param rarity The item rarity
     * @return The rarity name
     */
    std::string getRarityName(ItemRarity rarity);
    
    /**
     * @brief Initializes the random seed
     * @param seed The seed to use (0 for time-based)
     */
    void initializeRandom(unsigned int seed = 0);
    
    //==========================================================================
    // GETTERS
    //==========================================================================
    
    /**
     * @brief Gets the current game state
     * @return Reference to the game state
     */
    GameState& getGameState();
    
    /**
     * @brief Gets the current game state (const)
     * @return Const reference to the game state
     */
    const GameState& getGameState() const;
    
    /**
     * @brief Checks if the game is running
     * @return True if game is running
     */
    bool isRunning() const;

private:
    //==========================================================================
    // PRIVATE MEMBERS
    //==========================================================================
    
    GameState m_gameState;          // Complete game state
    MapGenerator m_mapGenerator;    // Dungeon generator
    CombatSystem m_combatSystem;    // Combat handler
    InventorySystem m_inventorySystem; // Inventory handler
    EventSystem m_eventSystem;      // Event handler
    FileIO m_fileIO;                // File I/O handler
    
    bool m_isRunning;               // Game running flag
    bool m_debugMode;               // Debug mode flag
    
    //==========================================================================
    // PRIVATE HELPER METHODS
    //==========================================================================
    
    /**
     * @brief Loads all game data files
     * @return True if all files loaded successfully
     */
    bool loadGameData();
    
    /**
     * @brief Initializes the game state
     */
    void initializeGame();
    
    /**
     * @brief Checks for room events and triggers them
     */
    void checkRoomEvents();
    
    /**
     * @brief Checks for random encounters
     */
    void checkRandomEncounter();
    
    /**
     * @brief Updates player status effects
     */
    void updateStatusEffects();
    
    /**
     * @brief Checks quest completion
     */
    void checkQuestProgress();
    
    /**
     * @brief Gets current room reference
     * @return Reference to current room
     */
    Room& getCurrentRoom();
    
    /**
     * @brief Gets current room reference (const)
     * @return Const reference to current room
     */
    const Room& getCurrentRoom() const;
};

}  // namespace AQE

#endif // GAMEENGINE_H
