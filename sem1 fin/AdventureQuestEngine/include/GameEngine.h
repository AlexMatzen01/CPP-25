//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Main game engine coordinating all game systems

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Types.h"
#include "MapGenerator.h"
#include "Combat.h"
#include "Inventory.h"
#include "Events.h"
#include "FileIO.h"
#include "Crafting.h"
#include "Utils.h"
#include "Constants.h"

#include <string>
#include <vector>
#include <iostream>

namespace AQE {

/**
 * @class GameEngine
// Main game engine that coordinates all game systems
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
// Constructs the game engine
     */
    GameEngine();
    
    /**
// Destructor
     */
    ~GameEngine();
    
    //==========================================================================
    // MAIN GAME FLOW
    //==========================================================================
    
    /**
// Runs the main game loop
     * @return Exit code (0 for success)
     */
    int run();
    
    /**
// Displays the intro/title screen with ASCII art
     */
    void showIntro();
    
    /**
// Displays the main menu
     * @return Menu choice (1=New Game, 2=Load, 3=Options, 4=Credits, 5=Quit)
     */
    int showMainMenu();
    
    /**
// Starts a new game
     */
    void startNewGame();
    
    /**
// Main gameplay loop (explore, combat, events)
     */
    void gameLoop();
    
    /**
// Ends the game and shows appropriate ending
     */
    void endGame();
    
    /**
// Print enhanced map with player position and fog of war
     */
    void printEnhancedMap();
    
    //==========================================================================
    // CHARACTER CREATION
    //==========================================================================
    
    /**
// Runs the full character creation process
     * Gets player name, class, and sets initial stats
     */
    void characterCreation();
    
    /**
// Gets a valid player name from input
     * @return The validated player name
     */
    std::string getPlayerName();
    
    /**
// Displays class options and gets player choice
     * @return The chosen character class
     */
    CharacterClass chooseClass();
    
    /**
// Applies base stats for the chosen class
     * @param charClass The chosen class
     */
    void applyClassStats(CharacterClass charClass);
    
    /**
// Displays class info (stats, abilities, playstyle)
     * @param charClass The class to display info for
     */
    void displayClassInfo(CharacterClass charClass);
    
    //==========================================================================
    // EXPLORATION
    //==========================================================================
    
    /**
// Shows the current room and available actions
     */
    void displayCurrentRoom();
    
    /**
// Gets exploration action from player
     * @return Action command (move, look, inventory, etc.)
     */
    std::string getExplorationAction();
    
    /**
// Processes the player's exploration action
     * @param action The action command
     * @return True if action was valid, false otherwise
     */
    bool processAction(const std::string& action);
    
    /**
// Attempts to move player in a direction
     * @param direction N/S/E/W
     * @return True if movement successful
     */
    bool movePlayer(char direction);
    
    /**
// Searches the current room for hidden items/secrets
     */
    void searchRoom();
    
    /**
// Interacts with room contents (NPCs, objects)
     */
    void interactWithRoom();
    
    /**
// Picks up an item from the current room
     * @param itemIndex Index of item in room's item list
     * @return True if pickup successful
     */
    bool pickupItem(int itemIndex);
    
    //==========================================================================
    // MENUS & UI
    //==========================================================================
    
    /**
// Displays the in-game menu
     * @return Menu choice
     */
    int showGameMenu();
    
    /**
// Shows player status (HP, stats, effects)
     */
    void showPlayerStatus();
    
    /**
// Opens the inventory menu
     */
    void openInventory();
    
    /**
// Opens the quest log
     */
    void openQuestLog();
    
    /**
// Opens the map view
     */
    void openMap();
    
    /**
// Shows help/controls
     */
    void showHelp();
    
    /**
// Shows game settings menu
     */
    void showSettings();
    
    /**
// Displays a text box with story/event text
     * @param title Box title
     * @param text Content to display
     */
    void displayTextBox(const std::string& title, const std::string& text);
    
    /**
// Displays a divider line
     * @param length Length of the divider
     * @param character Character to use
     */
    void displayDivider(int length = 60, char character = '=');
    
    /**
// Clears the screen (cross-platform)
     */
    void clearScreen();
    
    /**
// Pauses and waits for player to press enter
     */
    void waitForEnter();
    
    //==========================================================================
    // ENDING SYSTEM
    //==========================================================================
    
    /**
// Determines the ending based on player's journey
     * Uses choices, stats, quests, survival, and morality
     * @return The determined ending type
     */
    EndingType determineEnding();
    
    /**
// Displays the ending sequence
     * @param ending The ending to display
     */
    void displayEnding(EndingType ending);
    
    /**
// Calculates and displays final score
     * @return The final score
     */
    int calculateFinalScore();
    
    /**
// Displays game statistics at the end
     */
    void displayGameStats();
    
    /**
// Checks if player qualifies for the leaderboard
     * @param score The player's score
     * @return True if qualifies
     */
    bool checkLeaderboard(int score);
    
    /**
// Displays the leaderboard
     */
    void displayLeaderboard();
    
    //==========================================================================
    // SAVE / LOAD
    //==========================================================================
    
    /**
// Saves the current game state
     * @param filename The save file name
     * @return True if save successful
     */
    bool saveGame(const std::string& filename);
    
    /**
// Loads a saved game
     * @param filename The save file to load
     * @return True if load successful
     */
    bool loadGame(const std::string& filename);
    
    /**
// Lists available save files
     * @return Vector of save file names
     */
    std::vector<std::string> listSaveFiles();

    /**
// Prompts player to choose which save to load
     * @return Selected save file path, or empty string if cancelled/none
     */
    std::string selectSaveFileForLoad();

    /**
// Prompts player to choose where to save
     * @return Selected save file path, or empty string if cancelled
     */
    std::string selectSaveFileForSave();

    /**
// Sanitizes a user-provided save slot name
     * @param slotName Raw slot name
     * @return Safe slot name for filesystem use
     */
    std::string sanitizeSaveSlotName(const std::string& slotName) const;
    
    //==========================================================================
    // UTILITY METHODS
    //==========================================================================
    
    /**
// Logs an event to the game log
     * @param message The message to log
     */
    void logEvent(const std::string& message);
    
    /**
// Gets the class name as a string
     * @param charClass The character class
     * @return The class name
     */
    std::string getClassName(CharacterClass charClass);
    
    /**
// Gets the room type name as a string
     * @param type The room type
     * @return The room type name
     */
    std::string getRoomTypeName(RoomType type);
    
    /**
// Gets the rarity name and color code
     * @param rarity The item rarity
     * @return The rarity name
     */
    std::string getRarityName(ItemRarity rarity);
    
    /**
// Initializes the random seed
     * @param seed The seed to use (0 for time-based)
     */
    void initializeRandom(unsigned int seed = 0);
    
    //==========================================================================
    // GETTERS
    //==========================================================================
    
    /**
// Gets the current game state
     * @return Reference to the game state
     */
    GameState& getGameState();
    
    /**
// Gets the current game state (const)
     * @return Const reference to the game state
     */
    const GameState& getGameState() const;
    
    /**
// Checks if the game is running
     * @return True if game is running
     */
    bool isRunning() const;

private:
    //==========================================================================
    // PRIVATE MEMBERS
    //==========================================================================
    
    GameState m_gameState;          // Complete game state
    MapGenerator m_mapGenerator;    // Dungeon generator
    MapGenerator m_mawMapGenerator; // Dedicated Maw generator
    CombatSystem m_combatSystem;    // Combat handler
    InventorySystem m_inventorySystem; // Inventory handler
    EventSystem m_eventSystem;      // Event handler
    FileIO m_fileIO;                // File I/O handler
    MerchantSystem m_merchantSystem; // Merchant/shop handler
    CraftingSystem m_craftingSystem; // Crafting handler
    
    bool m_isRunning;               // Game running flag
    bool m_debugMode;               // Debug mode flag
    bool m_isInMawRealm;            // True while traversing a generated Maw dungeon
    bool m_leaveMawRequested;       // Set when player chooses to extract from Maw
    int m_currentMawStratum;        // Current Maw floor depth during expedition
    std::string m_activeSaveFile;   // Current save file path for quick saves
    
    //==========================================================================
    // PRIVATE HELPER METHODS
    //==========================================================================
    
    /**
// Loads all game data files
     * @return True if all files loaded successfully
     */
    bool loadGameData();
    
    /**
// Initializes the game state
     */
    void initializeGame();
    
    /**
// Checks for room events and triggers them
     */
    void checkRoomEvents();
    
    /**
// Handles shop encounters
     */
    void handleShopEncounter();
    
    /**
// Handles library encounters
     */
    void handleLibraryEncounter();

    /**
// Handles forge encounters
     */
    void handleForgeEncounter();

    /**
// Handles shrine encounters
     */
    void handleShrineEncounter();

    /**
// Handles laboratory encounters
     */
    void handleLaboratoryEncounter();

    /**
// Handles prison encounters
     */
    void handlePrisonEncounter();

    /**
// Handles graveyard encounters
     */
    void handleGraveyardEncounter();

    /**
// Handles armory encounters
     */
    void handleArmoryEncounter();
    
    /**
// Updates player status effects
     */
    void updateStatusEffects();
    
    /**
// Checks quest completion
     */
    void checkQuestProgress();
    
    /**
// Checks and handles player level ups
     */
    void checkLevelUp();
    
    /**
// Gets current room reference
     * @return Reference to current room
     */
    Room& getCurrentRoom();
    
    /**
// Gets current room reference (const)
     * @return Const reference to current room
     */
    const Room& getCurrentRoom() const;

    /**
// Resolves the active exit room ID from dungeon state with generator fallback
     * @return Exit room ID, or -1 if unavailable
     */
    int getActiveExitRoomId() const;

    /**
// Resolves the active Shadow Lord chamber ID, with exit-room fallback for legacy saves
     * @return Shadow Lord chamber room ID, or -1 if unavailable
     */
    int getActiveShadowLordRoomId() const;
    
    /**
// Shows the story introduction
     */
    void showStoryIntro();
    
    /**
// Gives starter quests to the player
     */
    void giveStarterQuests();
    
    /**
// Adds starter items to player inventory
     */
    void addStarterItems();
    
    /**
// Opens the cheat menu (debug mode only)
     */
    void openCheatMenu();
    
    /**
// Triggers the final boss fight
     * @return True if player wins
     */
    bool triggerFinalBoss();

    /**
// Handles the Shadow Lord encounter if the player is standing in the Shadow Lord chamber
     */
    void triggerShadowLordEncounterIfNeeded();

    /**
// Transforms the final chamber into the Portal Room and unlocks Maw access
     */
    void unlockMawPortal();

    /**
// Handles interaction menu inside the Portal Room
     */
    void handleMawPortal();

    /**
// Runs The Maw endless challenge loop
     * @return True if player returned alive, false if slain
     */
    bool runMawExpedition();

    /**
// Generates a fresh Maw stratum and places the player at its breach
     * @return True on success, false if generation fails
     */
    bool generateMawStratum();

    /**
// Advances the run to the next Maw stratum when stepping into a descent rift
     * @return True if movement should continue, false if Maw run should terminate
     */
    bool advanceMawStratumIfNeeded();

    /**
// Builds a scaled Maw enemy for the provided floor depth
     */
    Enemy createMawEnemy(int depth, bool bossVariant) const;

    /**
// Completes the main quest and grants rewards after Shadow Lord defeat
     */
    void completeMainQuestAfterShadowLord();
    
    /**
// Shows the victory celebration sequence
     */
    void showVictoryCelebration();
};

}  // namespace AQE

#endif // GAMEENGINE_H
