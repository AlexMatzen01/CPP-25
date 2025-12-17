/*******************************************************************************
 * C++ Adventure Quest Engine
 * Events.h - Event and Quest System Header
 * 
 * This file contains declarations for the event system including
 * story events, branching choices, quests, puzzles, and random encounters.
 * 
 * Author: Adventure Quest Engine Team
 * Version: 1.0
 ******************************************************************************/

#ifndef EVENTS_H
#define EVENTS_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {

//==============================================================================
// EVENT RESULT STRUCTURE
//==============================================================================

/**
 * @brief Result of processing an event
 */
struct EventResult {
    bool occurred;           // Did event happen?
    bool success;            // Was it successful (puzzle, choice, etc.)
    int hpChange;            // HP modification
    int goldChange;          // Gold modification
    int moralityChange;      // Morality modification
    int experienceGained;    // XP gained
    std::vector<Item> itemsGained;   // Items received
    std::vector<int> itemsLost;      // Item IDs lost
    std::string message;             // Result message
    bool triggersCombat;             // Does this start a fight?
    int combatEnemyId;               // Enemy ID if combat
    bool changesRoom;                // Does this move player?
    int newRoomId;                   // Destination room
    
    EventResult() : occurred(false), success(false), hpChange(0), goldChange(0),
                    moralityChange(0), experienceGained(0), triggersCombat(false),
                    combatEnemyId(-1), changesRoom(false), newRoomId(-1) {}
};

/**
 * @brief Result of a puzzle attempt
 */
struct PuzzleResult {
    bool attempted;
    bool solved;
    std::string message;
    Item reward;
    int damageOnFail;
    
    PuzzleResult() : attempted(false), solved(false), damageOnFail(0) {}
};

//==============================================================================
// EVENT SYSTEM CLASS
//==============================================================================

/**
 * @brief Manages game events, quests, and story progression
 */
class EventSystem {
public:
    /**
     * @brief Constructor
     * @param player Reference to player
     * @param gameState Reference to game state
     */
    EventSystem(Player& player, GameState& gameState);
    
    //==========================================================================
    // EVENT PROCESSING
    //==========================================================================
    
    /**
     * @brief Process events when entering a room
     * @param room The room being entered
     * @return Vector of event results
     */
    std::vector<EventResult> processRoomEvents(Room& room);
    
    /**
     * @brief Process a specific event
     * @param event The event to process
     * @return Event result
     */
    EventResult processEvent(Event& event);
    
    /**
     * @brief Trigger a random encounter
     * @return Event result (may include combat)
     */
    EventResult triggerRandomEncounter();
    
    /**
     * @brief Check and trigger trap in room
     * @param room The room to check
     * @return Event result
     */
    EventResult checkTrap(Room& room);
    
    //==========================================================================
    // BRANCHING CHOICES
    //==========================================================================
    
    /**
     * @brief Display and process a choice event
     * @param event The choice event
     * @return Event result based on player's choice
     */
    EventResult processChoiceEvent(Event& event);
    
    /**
     * @brief Process a story choice (5+ required branching decisions)
     * @param choiceId Unique choice identifier
     * @return Event result
     */
    EventResult processStoryChoice(int choiceId);
    
    /**
     * @brief Get available choices based on current state
     * @param event The event with choices
     * @return Vector of available choice indices
     */
    std::vector<int> getAvailableChoices(const Event& event) const;
    
    //==========================================================================
    // PUZZLES
    //==========================================================================
    
    /**
     * @brief Present puzzle to player
     * @param room Room containing puzzle
     * @return Puzzle result
     */
    PuzzleResult presentPuzzle(Room& room);
    
    /**
     * @brief Present riddle to player
     * @param question The riddle question
     * @param answer The correct answer
     * @param maxAttempts Maximum attempts allowed
     * @return True if solved
     */
    bool presentRiddle(const std::string& question, 
                       const std::string& answer,
                       int maxAttempts = 3);
    
    /**
     * @brief Present number puzzle
     * @return True if solved
     */
    bool presentNumberPuzzle();
    
    //==========================================================================
    // QUESTS
    //==========================================================================
    
    /**
     * @brief Start a new quest
     * @param questId Quest ID to start
     * @return True if started
     */
    bool startQuest(int questId);
    
    /**
     * @brief Update quest progress
     * @param questId Quest to update
     * @param progress Progress to add
     */
    void updateQuestProgress(int questId, int progress = 1);
    
    /**
     * @brief Check if quest is complete
     * @param questId Quest to check
     * @return True if completed
     */
    bool checkQuestCompletion(int questId);
    
    /**
     * @brief Complete a quest and give rewards
     * @param questId Quest to complete
     * @return Event result with rewards
     */
    EventResult completeQuest(int questId);
    
    /**
     * @brief Display active quests
     */
    void displayActiveQuests() const;
    
    /**
     * @brief Display completed quests
     */
    void displayCompletedQuests() const;
    
    /**
     * @brief Get quest by ID
     * @param questId Quest ID
     * @return Pointer to quest or nullptr
     */
    Quest* getQuest(int questId);
    
    //==========================================================================
    // RANDOM EVENTS
    //==========================================================================
    
    /**
     * @brief Roll for random event during exploration
     * @return Event result if event occurred
     */
    EventResult rollRandomEvent();
    
    /**
     * @brief Generate merchant encounter
     * @return Event result
     */
    EventResult generateMerchantEvent();
    
    /**
     * @brief Generate treasure find event
     * @return Event result
     */
    EventResult generateTreasureEvent();
    
    /**
     * @brief Generate ambush event
     * @return Event result
     */
    EventResult generateAmbushEvent();

private:
    Player& playerRef;
    GameState& gameRef;
    std::map<int, bool> triggeredEvents;  // Track one-time events
    
    /**
     * @brief Apply event effects to player
     * @param result Event result to apply
     */
    void applyEventEffects(const EventResult& result);
    
    /**
     * @brief Log event to player's game log
     * @param message Message to log
     */
    void logEvent(const std::string& message);
    
    /**
     * @brief Check if player meets event requirements
     * @param event Event to check
     * @return True if requirements met
     */
    bool meetsEventRequirements(const Event& event) const;
};

//==============================================================================
// PREDEFINED STORY CHOICE EVENTS
//==============================================================================

/**
 * @brief Story choice 1: Help or rob a wounded traveler
 */
EventResult storyChoice_WoundedTraveler(Player& player);

/**
 * @brief Story choice 2: Take cursed treasure or leave it
 */
EventResult storyChoice_CursedTreasure(Player& player);

/**
 * @brief Story choice 3: Side with rebels or kingdom
 */
EventResult storyChoice_Rebellion(Player& player);

/**
 * @brief Story choice 4: Sacrifice item for power
 */
EventResult storyChoice_DarkRitual(Player& player, GameState& gameState);

/**
 * @brief Story choice 5: Reveal secret or keep it
 */
EventResult storyChoice_SecretKeeper(Player& player);

/**
 * @brief Story choice 6: Mercy or execute boss
 */
EventResult storyChoice_BossMercy(Player& player);

//==============================================================================
// STANDALONE EVENT FUNCTIONS
//==============================================================================

/**
 * @brief Create default event templates
 * @return Vector of event templates
 */
std::vector<Event> createDefaultEvents();

/**
 * @brief Create default quest templates
 * @return Vector of quest templates
 */
std::vector<Quest> createDefaultQuests();

/**
 * @brief Load events from file
 * @param filename Path to events file
 * @return Vector of loaded events
 */
std::vector<Event> loadEvents(const std::string& filename);

/**
 * @brief Load quests from file
 * @param filename Path to quests file
 * @return Vector of loaded quests
 */
std::vector<Quest> loadQuests(const std::string& filename);

}  // namespace AQE

#endif // EVENTS_H
