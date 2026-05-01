





#ifndef EVENTS_H
#define EVENTS_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {






struct EventResult {
    bool occurred;           
    bool success;            
    int hpChange;            
    int goldChange;          
    int moralityChange;      
    int experienceGained;    
    std::vector<Item> itemsGained;   
    std::vector<int> itemsLost;      
    std::string message;             
    bool triggersCombat;             
    int combatEnemyId;               
    bool changesRoom;                
    int newRoomId;                   
    
    EventResult() : occurred(false), success(false), hpChange(0), goldChange(0),
                    moralityChange(0), experienceGained(0), triggersCombat(false),
                    combatEnemyId(-1), changesRoom(false), newRoomId(-1) {}
};


struct PuzzleResult {
    bool attempted;
    bool solved;
    std::string message;
    Item reward;
    int damageOnFail;
    
    PuzzleResult() : attempted(false), solved(false), damageOnFail(0) {}
};






class EventSystem {
public:
    
    EventSystem(Player& player, GameState& gameState);
    
    
    
    
    
    
    std::vector<EventResult> processRoomEvents(Room& room);
    
    
    EventResult processEvent(Event& event);
    
    
    EventResult checkTrap(Room& room);
    
    
    
    
    
    
    EventResult processChoiceEvent(Event& event);
    
    
    EventResult processStoryChoice(int choiceId);
    
    
    std::vector<int> getAvailableChoices(const Event& event) const;
    
    
    
    
    
    
    PuzzleResult presentPuzzle(Room& room);
    
    
    bool presentRiddle(const std::string& question, 
                       const std::string& answer,
                       int maxAttempts = 3);
    
    
    bool presentNumberPuzzle();
    
    
    
    
    
    
    bool startQuest(int questId);
    
    
    void updateQuestProgress(int questId, int progress = 1);
    
    
    bool checkQuestCompletion(int questId);
    
    
    EventResult completeQuest(int questId);
    
    
    void displayActiveQuests() const;
    
    
    void displayCompletedQuests() const;
    
    
    Quest* getQuest(int questId);
    
    
    
    
    
    
    EventResult rollRandomEvent();
    
    
    EventResult generateMerchantEvent();
    
    
    EventResult generateTreasureEvent();
    
    
    EventResult generateAmbushEvent();

private:
    Player& playerRef;
    GameState& gameRef;
    std::map<int, bool> triggeredEvents;  
    
    
    void applyEventEffects(const EventResult& result);
    
    
    void logEvent(const std::string& message);
    
    
    bool meetsEventRequirements(const Event& event) const;
};






EventResult storyChoice_WoundedTraveler(Player& player);


EventResult storyChoice_CursedTreasure(Player& player);


EventResult storyChoice_Rebellion(Player& player);


EventResult storyChoice_DarkRitual(Player& player, GameState& gameState);


EventResult storyChoice_SecretKeeper(Player& player);


EventResult storyChoice_BossMercy(Player& player);






std::vector<Event> createDefaultEvents();


std::vector<Quest> createDefaultQuests();


std::vector<Event> loadEvents(const std::string& filename);


std::vector<Quest> loadQuests(const std::string& filename);

}  

#endif 
