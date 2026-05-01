





#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Types.h"
#include "MapGenerator.h"
#include "Combat.h"
#include "Inventory.h"
#include "Events.h"
#include "FileIO.h"
#include "Crafting.h"
#include "SkillTree.h"
#include "Utils.h"
#include "Constants.h"

#include <string>
#include <vector>
#include <iostream>
#include <memory>

namespace AQE {


class GameEngine {
public:
    
    
    
    
    
    GameEngine();
    
    
    ~GameEngine();
    
    
    
    
    
    
    int run();
    
    
    void showIntro();
    
    
    int showMainMenu();
    
    
    void startNewGame();
    
    
    void gameLoop();
    
    
    void endGame();
    
    
    void printEnhancedMap();
    
    
    
    
    
    
    void characterCreation();
    
    
    std::string getPlayerName();
    
    
    CharacterClass chooseClass();
    
    
    void applyClassStats(CharacterClass charClass);
    
    
    void displayClassInfo(CharacterClass charClass);
    
    
    
    
    
    
    void displayCurrentRoom();
    
    
    std::string getExplorationAction();
    
    
    bool processAction(const std::string& action);
    
    
    bool movePlayer(char direction);
    
    
    void searchRoom();
    
    
    void interactWithRoom();
    
    
    bool pickupItem(int itemIndex);
    
    
    
    
    
    
    int showGameMenu();
    
    
    void showPlayerStatus();
    
    
    void openInventory();
    
    
    void openQuestLog();
    
    
    void openMap();
    
    
    void showHelp();
    
    
    void displaySkillsMenu();
    
    
    void displayTalentTree();
    
    
    void showSettings();
    
    
    void displayTextBox(const std::string& title, const std::string& text);
    
    
    void displayDivider(int length = 60, char character = '=');
    
    
    void clearScreen();
    
    
    void waitForEnter();
    
    
    
    
    
    
    EndingType determineEnding();
    
    
    void displayEnding(EndingType ending);
    
    
    int calculateFinalScore();
    
    
    void displayGameStats();
    
    
    bool checkLeaderboard(int score);
    
    
    void displayLeaderboard();
    
    
    
    
    
    
    bool saveGame(const std::string& filename);
    
    
    bool loadGame(const std::string& filename);
    
    
    std::vector<std::string> listSaveFiles();

    
    std::string selectSaveFileForLoad();

    
    std::string selectSaveFileForSave();

    
    std::string sanitizeSaveSlotName(const std::string& slotName) const;
    
    
    
    
    
    
    void logEvent(const std::string& message);
    
    
    std::string getClassName(CharacterClass charClass);
    
    
    std::string getRoomTypeName(RoomType type);
    
    
    std::string getRarityName(ItemRarity rarity);
    
    
    void initializeRandom(unsigned int seed = 0);
    
    
    
    
    
    
    GameState& getGameState();
    
    
    const GameState& getGameState() const;
    
    
    bool isRunning() const;

private:
    
    
    
    
    GameState m_gameState;          
    MapGenerator m_mapGenerator;    
    MapGenerator m_mawMapGenerator; 
    CombatSystem m_combatSystem;    
    InventorySystem m_inventorySystem; 
    EventSystem m_eventSystem;      
    FileIO m_fileIO;                
    MerchantSystem m_merchantSystem; 
    CraftingSystem m_craftingSystem; 
    std::unique_ptr<SkillTreeManager> m_skillTreeManager; 
    
    bool m_isRunning;               
    bool m_debugMode;               
    bool m_isInMawRealm;            
    bool m_leaveMawRequested;       
    int m_currentMawStratum;        
    std::string m_activeSaveFile;   
    
    
    
    
    
    
    bool loadGameData();
    
    
    void initializeGame();
    
    
    void checkRoomEvents();
    
    
    void handleShopEncounter();
    
    
    void handleLibraryEncounter();

    
    void handleForgeEncounter();

    
    void handleShrineEncounter();

    
    void handleLaboratoryEncounter();

    
    void handlePrisonEncounter();

    
    void handleGraveyardEncounter();

    
    void handleArmoryEncounter();
    
    
    void updateStatusEffects();
    
    
    void checkQuestProgress();
    
    
    void checkLevelUp();
    
    
    Room& getCurrentRoom();
    
    
    const Room& getCurrentRoom() const;

    
    int getActiveExitRoomId() const;

    
    int getActiveShadowLordRoomId() const;
    
    
    void showStoryIntro();
    
    
    void giveStarterQuests();
    
    
    void addStarterItems();
    
    
    void openCheatMenu();
    
    
    bool triggerFinalBoss();

    
    void triggerShadowLordEncounterIfNeeded();

    
    void unlockMawPortal();

    
    void handleMawPortal();

    
    bool runMawExpedition();

    
    bool generateMawStratum();

    
    bool advanceMawStratumIfNeeded();

    
    Enemy createMawEnemy(int depth, bool bossVariant) const;

    
    void completeMainQuestAfterShadowLord();
    
    
    void showVictoryCelebration();
};

}  

#endif 
