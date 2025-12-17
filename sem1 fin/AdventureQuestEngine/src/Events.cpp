/*******************************************************************************
 * C++ Adventure Quest Engine
 * Events.cpp - Event and Quest System Implementation
 * 
 * This file contains the implementation of the event system including
 * story events, branching choices, quests, puzzles, and random encounters.
 * 
 * Author: Adventure Quest Engine Team
 * Version: 1.0
 ******************************************************************************/

#include "../include/Events.h"
#include "../include/Utils.h"
#include "../include/Constants.h"
#include "../include/Inventory.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

namespace {
    bool fetchTemplateItem(int id, const std::vector<AQE::Item>& templates, AQE::Item& out) {
        for (const auto& item : templates) {
            if (item.id == id) {
                out = item;
                return true;
            }
        }
        return false;
    }
}

namespace AQE {

//==============================================================================
// CONSTRUCTOR
//==============================================================================

EventSystem::EventSystem(Player& player, GameState& gameState)
    : playerRef(player), gameRef(gameState) {}

//==============================================================================
// EVENT PROCESSING
//==============================================================================

std::vector<EventResult> EventSystem::processRoomEvents(Room& room) {
    std::vector<EventResult> results;
    
    // Check trap first
    if (room.type == RoomType::TRAP && room.isTrapActive) {
        EventResult trapResult = checkTrap(room);
        results.push_back(trapResult);
    }
    
    // Process room events
    for (int eventId : room.eventIds) {
        for (Event& event : gameRef.eventTemplates) {
            if (event.id == eventId) {
                if (meetsEventRequirements(event)) {
                    EventResult result = processEvent(event);
                    results.push_back(result);
                }
                break;
            }
        }
    }
    
    // Random encounter chance in normal rooms
    if (room.type == RoomType::NORMAL && !room.isCleared) {
        if (randomChance(20)) {
            EventResult encounter = triggerRandomEncounter();
            results.push_back(encounter);
        }
    }
    
    return results;
}

EventResult EventSystem::processEvent(Event& event) {
    EventResult result;
    result.occurred = true;
    
    // Check one-time events
    if (event.oneTimeOnly && event.hasOccurred) {
        result.occurred = false;
        return result;
    }
    
    // Check trigger chance
    if (!randomChance(event.triggerChance)) {
        result.occurred = false;
        return result;
    }
    
    std::cout << "\n";
    displaySeparator(50, '*');
    std::cout << "EVENT: " << event.name << "\n";
    displaySeparator(50, '*');
    std::cout << "\n" << wrapText(event.description, 50) << "\n";
    
    switch (event.type) {
        case EventType::CHOICE:
            result = processChoiceEvent(event);
            break;
            
        case EventType::TRAP:
            result.hpChange = -event.damageDealt;
            result.message = "You take " + std::to_string(event.damageDealt) + " damage!";
            break;
            
        case EventType::TREASURE:
            result.goldChange = event.goldGiven;
            for (int itemId : event.itemsGiven) {
                Item item;
                if (!fetchTemplateItem(itemId, gameRef.itemTemplates, item)) {
                    item = generateRandomItem(ItemRarity::UNCOMMON);
                }
                result.itemsGained.push_back(item);
            }
            result.message = "You found treasure!";
            break;
            
        case EventType::HEAL:
            result.hpChange = event.healingGiven;
            result.message = "You feel refreshed! Restored " + 
                            std::to_string(event.healingGiven) + " HP.";
            break;
            
        case EventType::COMBAT:
            result.triggersCombat = true;
            result.combatEnemyId = event.enemyId;
            result.message = "An enemy appears!";
            break;
            
        case EventType::RIDDLE:
            if (presentRiddle(event.riddleQuestion, event.riddleAnswer)) {
                result.success = true;
                if (event.riddleRewardId > 0) {
                    Item reward;
                    if (!fetchTemplateItem(event.riddleRewardId, gameRef.itemTemplates, reward)) {
                        reward = generateRandomItem(ItemRarity::RARE);
                    }
                    result.itemsGained.push_back(reward);
                }
                result.message = "Correct! The riddle is solved!";
            } else {
                result.success = false;
                result.hpChange = -event.riddleFailDamage;
                result.message = "Wrong answer... You feel a painful shock.";
            }
            break;
            
        case EventType::MERCHANT:
            result = generateMerchantEvent();
            break;
            
        case EventType::AMBUSH:
            result = generateAmbushEvent();
            break;
            
        case EventType::BUFF:
            for (const StatusEffect& buff : event.buffs) {
                playerRef.activeEffects.push_back(buff);
            }
            result.message = "You gain a mystical enhancement!";
            break;
            
        case EventType::STORY:
            result.message = event.description;
            break;
    }
    
    // Mark event as occurred
    event.hasOccurred = true;
    
    // Apply effects
    applyEventEffects(result);
    
    // Display result
    if (!result.message.empty()) {
        std::cout << "\n" << result.message << "\n";
    }
    
    logEvent(event.name + ": " + result.message);
    
    return result;
}

EventResult EventSystem::triggerRandomEncounter() {
    EventResult result;
    
    int encounterType = randomInt(1, 100);
    
    if (encounterType <= 40) {
        // Combat encounter
        result.triggersCombat = true;
        result.combatEnemyId = 0;  // Random enemy
        result.message = "A wild enemy appears!";
    } else if (encounterType <= 60) {
        // Find gold
        int gold = randomInt(5, 25);
        result.goldChange = gold;
        result.message = "You found " + std::to_string(gold) + " gold coins!";
    } else if (encounterType <= 75) {
        // Find item
        Item item = generateRandomItem(ItemRarity::COMMON);
        result.itemsGained.push_back(item);
        result.message = "You found " + item.name + "!";
    } else if (encounterType <= 85) {
        // Healing spring
        int heal = randomInt(10, 30);
        result.hpChange = heal;
        result.message = "You drink from a healing spring. +" + std::to_string(heal) + " HP!";
    } else {
        // Nothing happens
        result.occurred = false;
    }
    
    applyEventEffects(result);
    return result;
}

EventResult EventSystem::checkTrap(Room& room) {
    EventResult result;
    
    if (!room.isTrapActive) {
        result.occurred = false;
        return result;
    }
    
    result.occurred = true;
    
    // Rogue has higher chance to detect traps
    int detectChance = 20;
    if (playerRef.charClass == CharacterClass::ROGUE) {
        detectChance = 50;
    }
    
    // Check if player detects trap
    if (randomChance(detectChance)) {
        std::cout << "\n*** WARNING! You detect a trap! ***\n";
        std::cout << "Do you try to:\n";
        std::cout << "1. Disarm it (risky)\n";
        std::cout << "2. Carefully bypass it\n";
        std::cout << "3. Trigger it intentionally (take reduced damage)\n";
        
        int choice = getValidatedInt("Your choice: ", 1, 3);
        
        switch (choice) {
            case 1: {
                // Disarm attempt
                int disarmChance = 30 + playerRef.stats.speed;
                if (playerRef.charClass == CharacterClass::ROGUE) disarmChance += 30;
                
                if (randomChance(disarmChance)) {
                    result.message = "You successfully disarm the trap!";
                    result.experienceGained = 20;
                    room.isTrapActive = false;
                } else {
                    result.hpChange = -room.trapDamage;
                    result.message = "The trap triggers! You take " + 
                                   std::to_string(room.trapDamage) + " damage!";
                    room.isTrapActive = false;
                }
                break;
            }
            case 2: {
                // Bypass
                int bypassChance = 50 + playerRef.stats.dodgeChance;
                if (randomChance(bypassChance)) {
                    result.message = "You carefully bypass the trap.";
                } else {
                    result.hpChange = -(room.trapDamage / 2);
                    result.message = "You clip the trap edge! Take " + 
                                   std::to_string(room.trapDamage / 2) + " damage.";
                }
                break;
            }
            case 3: {
                // Intentional trigger with preparation
                result.hpChange = -(room.trapDamage / 2);
                result.message = "You brace yourself and trigger the trap. Take " +
                               std::to_string(room.trapDamage / 2) + " damage.";
                room.isTrapActive = false;
                break;
            }
        }
    } else {
        // Trap springs!
        result.hpChange = -room.trapDamage;
        result.message = "TRAP! " + room.description + " You take " + 
                        std::to_string(room.trapDamage) + " damage!";
        room.isTrapActive = false;
    }
    
    applyEventEffects(result);
    logEvent("Trap: " + result.message);
    
    return result;
}

//==============================================================================
// BRANCHING CHOICES
//==============================================================================

EventResult EventSystem::processChoiceEvent(Event& event) {
    EventResult result;
    
    if (event.choices.empty()) {
        result.message = "No choices available.";
        return result;
    }
    
    std::cout << "\nWhat do you do?\n";
    
    std::vector<int> available = getAvailableChoices(event);
    
    for (size_t i = 0; i < available.size(); ++i) {
        int choiceIdx = available[i];
        std::cout << "  " << (i + 1) << ". " << event.choices[choiceIdx].text << "\n";
    }
    
    int selection = getValidatedInt("\nYour choice: ", 1, static_cast<int>(available.size()));
    int choiceIdx = available[selection - 1];
    
    const EventChoice& choice = event.choices[choiceIdx];
    
    result.hpChange = choice.hpChange;
    result.goldChange = choice.goldChange;
    result.moralityChange = choice.moralityChange;
    result.message = choice.resultText;
    
    if (choice.itemRewardId > 0) {
        Item reward;
        if (!fetchTemplateItem(choice.itemRewardId, gameRef.itemTemplates, reward)) {
            reward = generateRandomItem(ItemRarity::UNCOMMON);
        }
        result.itemsGained.push_back(reward);
    }
    
    if (choice.itemLostId > 0) {
        result.itemsLost.push_back(choice.itemLostId);
    }
    
    if (choice.triggersEvent) {
        // Trigger follow-up event
        for (Event& followUp : gameRef.eventTemplates) {
            if (followUp.id == choice.triggeredEventId) {
                EventResult followResult = processEvent(followUp);
                // Combine results
                result.hpChange += followResult.hpChange;
                result.goldChange += followResult.goldChange;
                result.moralityChange += followResult.moralityChange;
                break;
            }
        }
    }
    
    std::cout << "\n" << result.message << "\n";
    
    return result;
}

EventResult EventSystem::processStoryChoice(int choiceId) {
    EventResult result;
    
    switch (choiceId) {
        case 1:
            result = storyChoice_WoundedTraveler(playerRef);
            break;
        case 2:
            result = storyChoice_CursedTreasure(playerRef);
            break;
        case 3:
            result = storyChoice_Rebellion(playerRef);
            break;
        case 4:
            result = storyChoice_DarkRitual(playerRef, gameRef);
            break;
        case 5:
            result = storyChoice_SecretKeeper(playerRef);
            break;
        case 6:
            result = storyChoice_BossMercy(playerRef);
            break;
        default:
            result.message = "Unknown story choice.";
            break;
    }
    
    applyEventEffects(result);
    logEvent("Story Choice " + std::to_string(choiceId) + ": " + result.message);
    
    return result;
}

std::vector<int> EventSystem::getAvailableChoices(const Event& event) const {
    std::vector<int> available;
    
    for (size_t i = 0; i < event.choices.size(); ++i) {
        const EventChoice& choice = event.choices[i];
        
        // Check if player has required item for choice
        bool requirementsMet = true;
        
        if (choice.itemLostId > 0) {
            bool hasItem = false;
            for (const Item& item : playerRef.inventory) {
                if (item.id == choice.itemLostId) {
                    hasItem = true;
                    break;
                }
            }
            if (!hasItem) requirementsMet = false;
        }
        
        // Check if player can afford gold cost
        if (choice.goldChange < 0 && playerRef.gold < -choice.goldChange) {
            requirementsMet = false;
        }
        
        if (requirementsMet) {
            available.push_back(static_cast<int>(i));
        }
    }
    
    return available;
}

//==============================================================================
// PUZZLES
//==============================================================================

PuzzleResult EventSystem::presentPuzzle(Room& room) {
    PuzzleResult result;
    
    if (room.puzzleSolved) {
        std::cout << "This puzzle has already been solved.\n";
        result.attempted = false;
        return result;
    }
    
    result.attempted = true;
    
    std::cout << "\n";
    displaySeparator(60, '?');
    std::cout << centerString("PUZZLE ROOM", 60) << "\n";
    displaySeparator(60, '?');
    
    std::cout << "\n" << wrapText(room.puzzleQuestion, 55) << "\n\n";
    
    bool solved = false;
    int attempts = 3;
    
    while (attempts > 0 && !solved) {
        std::string answer = getValidatedString("Your answer (or 'hint' for a hint, 'skip' to skip): ");
        answer = toLower(trim(answer));
        
        if (answer == "skip") {
            result.message = "You decide to skip this puzzle.";
            result.solved = false;
            return result;
        }
        
        if (answer == "hint") {
            // Find hint from riddle database
            for (const Riddle& r : RIDDLE_DATABASE) {
                if (toLower(r.answer) == room.puzzleAnswer) {
                    std::cout << "Hint: " << r.hint << "\n\n";
                    break;
                }
            }
            continue;
        }
        
        if (answer == room.puzzleAnswer) {
            solved = true;
        } else {
            attempts--;
            if (attempts > 0) {
                std::cout << "That's not correct. " << attempts << " attempts remaining.\n\n";
            }
        }
    }
    
    if (solved) {
        result.solved = true;
        room.puzzleSolved = true;
        playerRef.puzzlesSolved++;
        
        result.message = "Correct! The puzzle is solved!";
        result.reward = generateRandomItem(ItemRarity::RARE);
        
        std::cout << "\n*** PUZZLE SOLVED! ***\n";
        std::cout << "You receive: " << result.reward.name << "\n";
        
        InventorySystem inv(playerRef);
        inv.addItem(result.reward);
        
        logEvent("Solved puzzle: " + room.puzzleQuestion);
    } else {
        result.solved = false;
        result.damageOnFail = 15;
        result.message = "The puzzle remains unsolved. A magical shock hits you!";
        
        playerRef.stats.currentHP -= result.damageOnFail;
        
        std::cout << "\n" << result.message << " -" << result.damageOnFail << " HP\n";
    }
    
    return result;
}

bool EventSystem::presentRiddle(const std::string& question, const std::string& answer,
                                int maxAttempts) {
    std::cout << "\n*** RIDDLE ***\n";
    std::cout << wrapText(question, 60) << "\n\n";
    
    for (int i = 0; i < maxAttempts; ++i) {
        std::string playerAnswer = getValidatedString("Your answer: ");
        
        if (toLower(trim(playerAnswer)) == toLower(answer)) {
            std::cout << "\nCorrect!\n";
            return true;
        }
        
        int remaining = maxAttempts - i - 1;
        if (remaining > 0) {
            std::cout << "Wrong! " << remaining << " attempts left.\n";
        }
    }
    
    std::cout << "\nOut of attempts. The answer was: " << answer << "\n";
    return false;
}

bool EventSystem::presentNumberPuzzle() {
    int secretNumber = randomInt(1, 100);
    int maxGuesses = 7;
    
    std::cout << "\n*** NUMBER PUZZLE ***\n";
    std::cout << "I'm thinking of a number between 1 and 100.\n";
    std::cout << "You have " << maxGuesses << " guesses.\n\n";
    
    for (int i = 0; i < maxGuesses; ++i) {
        int guess = getValidatedInt("Your guess: ", 1, 100);
        
        if (guess == secretNumber) {
            std::cout << "\nCorrect! You found the number!\n";
            return true;
        } else if (guess < secretNumber) {
            std::cout << "Higher!\n";
        } else {
            std::cout << "Lower!\n";
        }
    }
    
    std::cout << "\nOut of guesses! The number was: " << secretNumber << "\n";
    return false;
}

//==============================================================================
// QUESTS
//==============================================================================

bool EventSystem::startQuest(int questId) {
    Quest* quest = getQuest(questId);
    
    if (!quest) {
        // Check templates
        for (Quest& q : gameRef.questTemplates) {
            if (q.id == questId) {
                q.status = QuestStatus::IN_PROGRESS;
                playerRef.activeQuests.push_back(q);
                
                std::cout << "\n*** NEW QUEST: " << q.name << " ***\n";
                std::cout << q.description << "\n";
                
                logEvent("Started quest: " + q.name);
                return true;
            }
        }
        return false;
    }
    
    if (quest->status != QuestStatus::NOT_STARTED) {
        return false;  // Already started
    }
    
    quest->status = QuestStatus::IN_PROGRESS;
    logEvent("Started quest: " + quest->name);
    return true;
}

void EventSystem::updateQuestProgress(int questId, int progress) {
    Quest* quest = getQuest(questId);
    
    if (quest && quest->status == QuestStatus::IN_PROGRESS) {
        quest->currentEnemyCount += progress;
        
        std::cout << "\n[Quest Progress: " << quest->name << " - " 
                  << quest->currentEnemyCount << "/" << quest->targetEnemyCount << "]\n";
        
        // Check if complete
        if (checkQuestCompletion(questId)) {
            completeQuest(questId);
        }
    }
}

bool EventSystem::checkQuestCompletion(int questId) {
    Quest* quest = getQuest(questId);
    
    if (!quest || quest->status != QuestStatus::IN_PROGRESS) {
        return false;
    }
    
    // Check enemy kill count
    if (quest->targetEnemyCount > 0 && 
        quest->currentEnemyCount >= quest->targetEnemyCount) {
        return true;
    }
    
    // Check item collection
    if (quest->targetItemId > 0) {
        for (const Item& item : playerRef.inventory) {
            if (item.id == quest->targetItemId) {
                return true;
            }
        }
    }
    
    // Check room reached
    if (quest->targetRoomId > 0 && 
        playerRef.currentRoomId == quest->targetRoomId) {
        return true;
    }
    
    return false;
}

EventResult EventSystem::completeQuest(int questId) {
    EventResult result;
    Quest* quest = getQuest(questId);
    
    if (!quest || quest->status != QuestStatus::IN_PROGRESS) {
        result.message = "Quest not found or not in progress.";
        return result;
    }
    
    quest->status = QuestStatus::COMPLETED;
    result.occurred = true;
    result.success = true;
    
    // Give rewards
    result.goldChange = quest->goldReward;
    result.experienceGained = quest->experienceReward;
    
    if (quest->affectsMorality) {
        result.moralityChange = quest->moralityChange;
    }
    
    for (int itemId : quest->itemRewards) {
        Item reward;
        if (fetchTemplateItem(itemId, gameRef.itemTemplates, reward)) {
            result.itemsGained.push_back(reward);
        } else {
            // Fallback to generating a random item if template missing
            result.itemsGained.push_back(generateRandomItem(ItemRarity::RARE));
        }
    }
    
    // Move to completed quests
    playerRef.completedQuests.push_back(*quest);
    
    // Remove from active quests
    auto it = std::find_if(playerRef.activeQuests.begin(), playerRef.activeQuests.end(),
        [questId](const Quest& q) { return q.id == questId; });
    if (it != playerRef.activeQuests.end()) {
        playerRef.activeQuests.erase(it);
    }
    
    std::cout << "\n";
    displaySeparator(50, '*');
    std::cout << "*** QUEST COMPLETE: " << quest->name << " ***\n";
    displaySeparator(50, '*');
    std::cout << quest->completionText << "\n";
    std::cout << "\nRewards:\n";
    std::cout << "  Gold: +" << quest->goldReward << "\n";
    std::cout << "  Experience: +" << quest->experienceReward << "\n";
    
    for (const Item& item : result.itemsGained) {
        std::cout << "  Item: " << item.name << "\n";
    }
    
    applyEventEffects(result);
    logEvent("Completed quest: " + quest->name);
    
    return result;
}

void EventSystem::displayActiveQuests() const {
    std::cout << "\n";
    displaySeparator(50, '-');
    std::cout << centerString("ACTIVE QUESTS", 50) << "\n";
    displaySeparator(50, '-');
    
    if (playerRef.activeQuests.empty()) {
        std::cout << "  No active quests.\n";
    } else {
        for (const Quest& quest : playerRef.activeQuests) {
            std::cout << "\n  [" << (quest.isMainQuest ? "MAIN" : "SIDE") << "] " 
                      << quest.name << "\n";
            std::cout << "  " << wrapText(quest.description, 45) << "\n";
            
            if (quest.targetEnemyCount > 0) {
                std::cout << "  Progress: " << quest.currentEnemyCount 
                          << "/" << quest.targetEnemyCount << "\n";
            }
        }
    }
    displaySeparator(50, '-');
}

void EventSystem::displayCompletedQuests() const {
    std::cout << "\n";
    displaySeparator(50, '-');
    std::cout << centerString("COMPLETED QUESTS", 50) << "\n";
    displaySeparator(50, '-');
    
    if (playerRef.completedQuests.empty()) {
        std::cout << "  No completed quests.\n";
    } else {
        for (const Quest& quest : playerRef.completedQuests) {
            std::cout << "  [✓] " << quest.name << "\n";
        }
    }
    displaySeparator(50, '-');
}

Quest* EventSystem::getQuest(int questId) {
    for (Quest& quest : playerRef.activeQuests) {
        if (quest.id == questId) {
            return &quest;
        }
    }
    return nullptr;
}

//==============================================================================
// RANDOM EVENTS
//==============================================================================

EventResult EventSystem::rollRandomEvent() {
    EventResult result;
    
    if (!randomChance(15)) {  // 15% chance for random event
        result.occurred = false;
        return result;
    }
    
    int eventType = randomInt(1, 5);
    
    switch (eventType) {
        case 1:
            result = generateMerchantEvent();
            break;
        case 2:
            result = generateTreasureEvent();
            break;
        case 3:
            result = generateAmbushEvent();
            break;
        case 4:
            // Healing fountain
            result.occurred = true;
            result.hpChange = randomInt(15, 35);
            result.message = "You find a magical healing fountain! +" + 
                           std::to_string(result.hpChange) + " HP";
            break;
        case 5:
            // Gold find
            result.occurred = true;
            result.goldChange = randomInt(10, 50);
            result.message = "You discover a hidden stash of gold! +" + 
                           std::to_string(result.goldChange) + " gold";
            break;
    }
    
    applyEventEffects(result);
    return result;
}

EventResult EventSystem::generateMerchantEvent() {
    EventResult result;
    result.occurred = true;
    
    std::cout << "\n";
    displaySeparator(50, '$');
    std::cout << "A mysterious merchant appears!\n";
    displaySeparator(50, '$');
    std::cout << "\"Greetings, traveler! Care to see my wares?\"\n\n";
    
    // Generate items for sale
    std::vector<Item> forSale;
    for (int i = 0; i < 3; ++i) {
        ItemRarity rarity = static_cast<ItemRarity>(randomInt(0, 2));
        forSale.push_back(generateRandomItem(rarity));
    }
    
    bool shopping = true;
    while (shopping) {
        std::cout << "\nYour gold: " << playerRef.gold << "\n\n";
        std::cout << "For sale:\n";
        
        for (size_t i = 0; i < forSale.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << forSale[i].name 
                      << " - " << forSale[i].value << " gold\n";
        }
        std::cout << "  0. Leave\n";
        
        int choice = getValidatedInt("What would you like to buy? ", 0, 
                                     static_cast<int>(forSale.size()));
        
        if (choice == 0) {
            shopping = false;
            std::cout << "\"Safe travels, friend!\"\n";
        } else {
            int idx = choice - 1;
            if (playerRef.gold >= forSale[idx].value) {
                playerRef.gold -= forSale[idx].value;
                
                InventorySystem inv(playerRef);
                inv.addItem(forSale[idx]);
                
                std::cout << "\"Excellent choice!\" You purchased " 
                          << forSale[idx].name << ".\n";
                result.itemsGained.push_back(forSale[idx]);
                
                forSale.erase(forSale.begin() + idx);
                
                if (forSale.empty()) {
                    std::cout << "\"That's all I have. Farewell!\"\n";
                    shopping = false;
                }
            } else {
                std::cout << "\"You don't have enough gold for that.\"\n";
            }
        }
    }
    
    result.message = "Merchant encounter";
    return result;
}

EventResult EventSystem::generateTreasureEvent() {
    EventResult result;
    result.occurred = true;
    
    std::cout << "\n*** You discovered a treasure chest! ***\n";
    
    // Chance for trap
    if (randomChance(30)) {
        std::cout << "It's trapped!\n";
        
        if (playerRef.charClass == CharacterClass::ROGUE && randomChance(60)) {
            std::cout << "Your rogue instincts help you disarm it!\n";
        } else {
            int damage = randomInt(10, 20);
            result.hpChange = -damage;
            std::cout << "The trap triggers! -" << damage << " HP\n";
        }
    }
    
    // Treasure
    result.goldChange = randomInt(20, 75);
    
    // Chance for item
    if (randomChance(60)) {
        ItemRarity rarity = static_cast<ItemRarity>(randomInt(1, 3));
        Item treasure = generateRandomItem(rarity);
        result.itemsGained.push_back(treasure);
        
        InventorySystem inv(playerRef);
        inv.addItem(treasure);
        
        std::cout << "Found: " << treasure.name << "!\n";
    }
    
    std::cout << "Found: " << result.goldChange << " gold!\n";
    
    result.message = "Treasure found!";
    return result;
}

EventResult EventSystem::generateAmbushEvent() {
    EventResult result;
    result.occurred = true;
    result.triggersCombat = true;
    result.combatEnemyId = 0;  // Random enemy
    
    std::cout << "\n*** AMBUSH! ***\n";
    std::cout << "Enemies leap from the shadows!\n";
    
    // Ambushed - player takes first hit
    int ambushDamage = randomInt(5, 15);
    result.hpChange = -ambushDamage;
    
    std::cout << "You're caught off guard! -" << ambushDamage << " HP\n";
    
    result.message = "Ambushed by enemies!";
    return result;
}

//==============================================================================
// PRIVATE METHODS
//==============================================================================

void EventSystem::applyEventEffects(const EventResult& result) {
    // Apply HP change
    if (result.hpChange != 0) {
        playerRef.stats.currentHP += result.hpChange;
        
        if (playerRef.stats.currentHP > playerRef.stats.maxHP) {
            playerRef.stats.currentHP = playerRef.stats.maxHP;
        }
        if (playerRef.stats.currentHP < 0) {
            playerRef.stats.currentHP = 0;
            playerRef.isAlive = false;
        }
    }
    
    // Apply gold change
    if (result.goldChange != 0) {
        playerRef.gold += result.goldChange;
        if (playerRef.gold < 0) playerRef.gold = 0;
    }
    
    // Apply morality change
    if (result.moralityChange != 0) {
        playerRef.moralityScore += result.moralityChange;
    }
    
    // Apply experience
    if (result.experienceGained > 0) {
        playerRef.experience += result.experienceGained;
    }
    
    // Remove lost items
    for (int itemId : result.itemsLost) {
        InventorySystem inv(playerRef);
        inv.removeItemById(itemId, 1);
    }
}

void EventSystem::logEvent(const std::string& message) {
    playerRef.gameLog.push_back(message);
}

bool EventSystem::meetsEventRequirements(const Event& event) const {
    // Check required item
    if (event.requiresItem) {
        bool hasItem = false;
        for (const Item& item : playerRef.inventory) {
            if (item.id == event.requiredItemId) {
                hasItem = true;
                break;
            }
        }
        if (!hasItem) return false;
    }
    
    // Check one-time flag
    if (event.oneTimeOnly && event.hasOccurred) {
        return false;
    }
    
    return true;
}

//==============================================================================
// PREDEFINED STORY CHOICES
//==============================================================================

EventResult storyChoice_WoundedTraveler(Player& player) {
    EventResult result;
    result.occurred = true;
    
    std::cout << "\n";
    displaySeparator(60, '-');
    std::cout << "STORY CHOICE: The Wounded Traveler\n";
    displaySeparator(60, '-');
    std::cout << "\nYou encounter a wounded traveler on the path. They're bleeding\n";
    std::cout << "and barely conscious, but you notice a heavy coin purse at their belt.\n\n";
    std::cout << "What do you do?\n";
    std::cout << "1. Help them and give them a health potion\n";
    std::cout << "2. Rob them while they're helpless\n";
    std::cout << "3. Ignore them and move on\n";
    
    int choice = getValidatedInt("Your choice: ", 1, 3);
    
    switch (choice) {
        case 1:
            result.moralityChange = 15;
            result.message = "You bandage their wounds and share your supplies. "
                           "In gratitude, they reveal a secret passage location!";
            player.secretsFound++;
            std::cout << "\n\"Thank you, kind soul! Let me tell you of a hidden path...\"\n";
            break;
        case 2:
            result.moralityChange = -25;
            result.goldChange = 50;
            result.message = "You take their gold as they slip into unconsciousness.";
            std::cout << "\nTheir eyes watch you with betrayal as you pocket their coin...\n";
            break;
        case 3:
            result.moralityChange = -5;
            result.message = "You walk past, leaving them to their fate.";
            std::cout << "\nTheir pleas fade behind you...\n";
            break;
    }
    
    return result;
}

EventResult storyChoice_CursedTreasure(Player& player) {
    EventResult result;
    result.occurred = true;
    
    std::cout << "\n";
    displaySeparator(60, '-');
    std::cout << "STORY CHOICE: The Cursed Treasure\n";
    displaySeparator(60, '-');
    std::cout << "\nA glittering artifact sits on a pedestal, emanating dark energy.\n";
    std::cout << "Ancient warnings are carved around it, but the power it offers\n";
    std::cout << "is undeniable.\n\n";
    std::cout << "What do you do?\n";
    std::cout << "1. Take the artifact (gain power, risk curse)\n";
    std::cout << "2. Destroy the artifact (good deed, no reward)\n";
    std::cout << "3. Leave it alone\n";
    
    int choice = getValidatedInt("Your choice: ", 1, 3);
    
    switch (choice) {
        case 1: {
            if (randomChance(40)) {
                // Cursed!
                result.moralityChange = -10;
                result.hpChange = -20;
                player.stats.maxHP -= 10;  // Permanent curse
                result.message = "The artifact's curse takes hold! Max HP permanently reduced!";
                std::cout << "\nDark energy courses through you painfully...\n";
            } else {
                // Gained power
                player.stats.attack += 5;
                result.message = "You resist the curse and gain the artifact's power! +5 Attack";
                std::cout << "\nPower flows into you!\n";
            }
            break;
        }
        case 2:
            result.moralityChange = 20;
            result.experienceGained = 50;
            result.message = "You destroy the evil artifact, freeing trapped souls.";
            std::cout << "\nGhostly voices whisper thanks as they find peace...\n";
            break;
        case 3:
            result.message = "You wisely leave the artifact alone.";
            std::cout << "\nSome treasures aren't worth the cost...\n";
            break;
    }
    
    return result;
}

EventResult storyChoice_Rebellion(Player& player) {
    EventResult result;
    result.occurred = true;
    (void)player; // parameter unused in this story function
    
    std::cout << "\n";
    displaySeparator(60, '-');
    std::cout << "STORY CHOICE: The Rebellion\n";
    displaySeparator(60, '-');
    std::cout << "\nRebels approach you with information about the corrupt kingdom.\n";
    std::cout << "They want your help to overthrow the tyrant king. The kingdom's\n";
    std::cout << "soldiers offer gold for loyalty to the crown.\n\n";
    std::cout << "What do you do?\n";
    std::cout << "1. Join the rebels (risky, but righteous)\n";
    std::cout << "2. Side with the kingdom (safe, but questionable)\n";
    std::cout << "3. Stay neutral\n";
    
    int choice = getValidatedInt("Your choice: ", 1, 3);
    
    switch (choice) {
        case 1:
            result.moralityChange = 15;
            // Quest given
            result.message = "You join the rebellion! A new quest awaits.";
            std::cout << "\n\"Welcome, friend! Together we will bring justice!\"\n";
            break;
        case 2:
            result.moralityChange = -10;
            result.goldChange = 100;
            result.message = "The kingdom rewards your loyalty with gold.";
            std::cout << "\n\"Your loyalty will be remembered. Here's your payment.\"\n";
            break;
        case 3:
            result.message = "You choose to stay out of political matters.";
            std::cout << "\n\"A wise choice... or a cowardly one. Time will tell.\"\n";
            break;
    }
    
    return result;
}

EventResult storyChoice_DarkRitual(Player& player, GameState& gameState) {
    EventResult result;
    result.occurred = true;
    (void)gameState; // parameter unused here
    
    std::cout << "\n";
    displaySeparator(60, '-');
    std::cout << "STORY CHOICE: The Dark Ritual\n";
    displaySeparator(60, '-');
    std::cout << "\nA hooded figure offers you immense power in exchange for\n";
    std::cout << "a blood sacrifice. You could become nearly invincible...\n";
    std::cout << "at a terrible cost.\n\n";
    std::cout << "What do you do?\n";
    std::cout << "1. Accept the dark pact (massive power, dark path)\n";
    std::cout << "2. Refuse and attack the cultist\n";
    std::cout << "3. Walk away\n";
    
    int choice = getValidatedInt("Your choice: ", 1, 3);
    
    switch (choice) {
        case 1:
            result.moralityChange = -50;
            player.stats.attack += 15;
            player.stats.critChance += 20;
            player.stats.maxHP -= 30;
            player.stats.currentHP = player.stats.maxHP;
            result.message = "Dark power fills you! But your humanity slips away...";
            std::cout << "\n\"Excellent... you belong to the darkness now.\"\n";
            break;
        case 2:
            result.moralityChange = 10;
            result.triggersCombat = true;
            result.combatEnemyId = 999;  // Cultist enemy
            result.message = "You attack the dark cultist!";
            std::cout << "\n\"You will regret this, fool!\"\n";
            break;
        case 3:
            result.message = "You reject the dark path.";
            std::cout << "\n\"You'll be back... they always come back...\"\n";
            break;
    }
    
    return result;
}

EventResult storyChoice_SecretKeeper(Player& player) {
    EventResult result;
    result.occurred = true;
    (void)player; // parameter unused in this story function
    
    std::cout << "\n";
    displaySeparator(60, '-');
    std::cout << "STORY CHOICE: The Secret Keeper\n";
    displaySeparator(60, '-');
    std::cout << "\nYou discover evidence of a noble's treachery against the people.\n";
    std::cout << "Exposing them would bring justice, but they offer a fortune\n";
    std::cout << "for your silence.\n\n";
    std::cout << "What do you do?\n";
    std::cout << "1. Expose the noble (justice, enemy made)\n";
    std::cout << "2. Accept the bribe (wealth, guilty conscience)\n";
    std::cout << "3. Use the information for personal gain\n";
    
    int choice = getValidatedInt("Your choice: ", 1, 3);
    
    switch (choice) {
        case 1:
            result.moralityChange = 20;
            result.experienceGained = 75;
            result.message = "Justice is served! The noble is arrested.";
            std::cout << "\nThe people celebrate your bravery!\n";
            break;
        case 2:
            result.moralityChange = -15;
            result.goldChange = 200;
            result.message = "You pocket the bribe and forget what you saw.";
            std::cout << "\n\"Smart choice. Now forget we ever met.\"\n";
            break;
        case 3:
            result.moralityChange = -5;
            result.goldChange = 100;
            result.experienceGained = 25;
            result.message = "You blackmail the noble for ongoing payments.";
            std::cout << "\nA dangerous game, but profitable...\n";
            break;
    }
    
    return result;
}

EventResult storyChoice_BossMercy(Player& player) {
    EventResult result;
    result.occurred = true;
    
    std::cout << "\n";
    displaySeparator(60, '-');
    std::cout << "STORY CHOICE: Mercy for the Fallen\n";
    displaySeparator(60, '-');
    std::cout << "\nYour defeated enemy lies before you, begging for mercy.\n";
    std::cout << "They swear to change their ways and offer valuable information.\n";
    std::cout << "But can you trust someone who has caused so much harm?\n\n";
    std::cout << "What do you do?\n";
    std::cout << "1. Show mercy and spare them\n";
    std::cout << "2. Execute them for their crimes\n";
    std::cout << "3. Imprison them (let others decide)\n";
    
    int choice = getValidatedInt("Your choice: ", 1, 3);
    
    switch (choice) {
        case 1:
            result.moralityChange = 15;
            player.secretsFound++;
            result.message = "You show mercy. They reveal a secret passage in gratitude.";
            std::cout << "\n\"I... thank you. There's a hidden room behind the throne.\"\n";
            break;
        case 2:
            result.moralityChange = -10;
            result.experienceGained = 50;
            result.message = "You execute them. Justice... or vengeance?";
            std::cout << "\nTheir last breath is a curse upon your name...\n";
            break;
        case 3:
            result.moralityChange = 5;
            result.message = "You bind them for proper judgment.";
            std::cout << "\n\"A fair decision. Perhaps there's justice after all.\"\n";
            break;
    }
    
    return result;
}

//==============================================================================
// STANDALONE FUNCTIONS
//==============================================================================

std::vector<Event> createDefaultEvents() {
    std::vector<Event> events;
    
    // Trap event
    Event trap;
    trap.id = 1;
    trap.name = "Arrow Trap";
    trap.description = "Arrows fly from hidden holes in the walls!";
    trap.type = EventType::TRAP;
    trap.damageDealt = 15;
    trap.triggerChance = 100;
    events.push_back(trap);
    
    // Healing event
    Event heal;
    heal.id = 2;
    heal.name = "Healing Shrine";
    heal.description = "A glowing shrine pulses with restorative energy.";
    heal.type = EventType::HEAL;
    heal.healingGiven = 30;
    heal.triggerChance = 100;
    heal.oneTimeOnly = true;
    events.push_back(heal);
    
    // Choice event
    Event choice;
    choice.id = 3;
    choice.name = "The Mysterious Chest";
    choice.description = "A ornate chest sits in the corner, slightly glowing.";
    choice.type = EventType::CHOICE;
    choice.triggerChance = 100;
    choice.oneTimeOnly = true;
    
    EventChoice open;
    open.text = "Open the chest";
    open.resultText = "The chest contains valuable treasures!";
    open.goldChange = 50;
    choice.choices.push_back(open);
    
    EventChoice leave;
    leave.text = "Leave it alone";
    leave.resultText = "You wisely avoid a potential trap.";
    leave.moralityChange = 0;
    choice.choices.push_back(leave);
    
    events.push_back(choice);
    
    // Riddle event
    Event riddle;
    riddle.id = 4;
    riddle.name = "The Sphinx's Question";
    riddle.description = "A stone sphinx blocks your path, demanding an answer.";
    riddle.type = EventType::RIDDLE;
    riddle.riddleQuestion = "I have cities, but no houses. Mountains, but no trees. Water, but no fish. What am I?";
    riddle.riddleAnswer = "map";
    riddle.riddleRewardId = 1;
    riddle.riddleFailDamage = 20;
    riddle.triggerChance = 100;
    riddle.oneTimeOnly = true;
    events.push_back(riddle);
    
    return events;
}

std::vector<Quest> createDefaultQuests() {
    std::vector<Quest> quests;
    
    // Main quest: Escape the dungeon
    Quest main;
    main.id = 1;
    main.name = "Escape the Dungeon";
    main.description = "Find the exit and escape this dark place.";
    main.completionText = "Freedom at last! You've escaped the dungeon!";
    main.isMainQuest = true;
    main.goldReward = 100;
    main.experienceReward = 200;
    quests.push_back(main);
    
    // Side quest: Kill enemies
    Quest killQuest;
    killQuest.id = 2;
    killQuest.name = "Monster Hunter";
    killQuest.description = "Defeat 5 enemies in the dungeon.";
    killQuest.completionText = "You've proven yourself a capable warrior!";
    killQuest.targetEnemyCount = 5;
    killQuest.goldReward = 50;
    killQuest.experienceReward = 100;
    quests.push_back(killQuest);
    
    // Side quest: Find treasure rooms
    Quest treasureQuest;
    treasureQuest.id = 3;
    treasureQuest.name = "Treasure Seeker";
    treasureQuest.description = "Discover 3 treasure rooms.";
    treasureQuest.completionText = "Your greed... I mean, exploration has paid off!";
    treasureQuest.targetEnemyCount = 3;  // Reusing for room count
    treasureQuest.goldReward = 75;
    treasureQuest.experienceReward = 75;
    quests.push_back(treasureQuest);
    
    // Side quest: Solve puzzles
    Quest puzzleQuest;
    puzzleQuest.id = 4;
    puzzleQuest.name = "Puzzle Master";
    puzzleQuest.description = "Solve 2 puzzles in the dungeon.";
    puzzleQuest.completionText = "Your wisdom is unmatched!";
    puzzleQuest.targetEnemyCount = 2;  // Reusing for puzzle count
    puzzleQuest.goldReward = 60;
    puzzleQuest.experienceReward = 120;
    quests.push_back(puzzleQuest);
    
    // Morality quest: Good path
    Quest heroQuest;
    heroQuest.id = 5;
    heroQuest.name = "The Hero's Path";
    heroQuest.description = "Make noble choices throughout your journey.";
    heroQuest.completionText = "You have walked the path of the true hero!";
    heroQuest.affectsMorality = true;
    heroQuest.moralityChange = 0;  // Check morality score instead
    heroQuest.goldReward = 0;
    heroQuest.experienceReward = 150;
    quests.push_back(heroQuest);
    
    return quests;
}

std::vector<Event> loadEvents(const std::string& filename) {
    std::vector<Event> events;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return createDefaultEvents();
    }
    
    // Simple file parsing (can be expanded)
    std::string line;
    Event current;
    bool inEvent = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line == "[EVENT]") {
            if (inEvent) {
                events.push_back(current);
            }
            current = Event();
            inEvent = true;
            continue;
        }
        
        if (!inEvent) continue;
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));
            
            if (key == "id") current.id = std::stoi(value);
            else if (key == "name") current.name = value;
            else if (key == "description") current.description = value;
            else if (key == "damage") current.damageDealt = std::stoi(value);
            else if (key == "healing") current.healingGiven = std::stoi(value);
            else if (key == "gold") current.goldGiven = std::stoi(value);
            else if (key == "chance") current.triggerChance = std::stoi(value);
            else if (key == "onetime") current.oneTimeOnly = (value == "true");
        }
    }
    
    if (inEvent) {
        events.push_back(current);
    }
    
    file.close();
    
    if (events.empty()) {
        return createDefaultEvents();
    }
    
    return events;
}

std::vector<Quest> loadQuests(const std::string& filename) {
    std::vector<Quest> quests;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return createDefaultQuests();
    }
    
    std::string line;
    Quest current;
    bool inQuest = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line == "[QUEST]") {
            if (inQuest) {
                quests.push_back(current);
            }
            current = Quest();
            inQuest = true;
            continue;
        }
        
        if (!inQuest) continue;
        
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));
            
            if (key == "id") current.id = std::stoi(value);
            else if (key == "name") current.name = value;
            else if (key == "description") current.description = value;
            else if (key == "completion") current.completionText = value;
            else if (key == "gold") current.goldReward = std::stoi(value);
            else if (key == "experience") current.experienceReward = std::stoi(value);
            else if (key == "targetcount") current.targetEnemyCount = std::stoi(value);
            else if (key == "main") current.isMainQuest = (value == "true");
        }
    }
    
    if (inQuest) {
        quests.push_back(current);
    }
    
    file.close();
    
    if (quests.empty()) {
        return createDefaultQuests();
    }
    
    return quests;
}

}  // namespace AQE
