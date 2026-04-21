//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Combat system implementation

#include "../include/Combat.h"
#include "../include/Utils.h"
#include "../include/Constants.h"
#include "../include/Inventory.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cmath>

namespace AQE {

namespace {
    std::string intentToString(EnemyIntent intent) {
        switch (intent) {
            case EnemyIntent::ATTACK_LIGHT: return "Light Attack";
            case EnemyIntent::ATTACK_HEAVY: return "Heavy Attack";
            case EnemyIntent::DEFEND: return "Defend";
            case EnemyIntent::DEBUFF: return "Debuff";
            case EnemyIntent::BUFF: return "Buff";
            case EnemyIntent::HEAL: return "Heal";
            case EnemyIntent::CONTROL: return "Control";
            case EnemyIntent::SUMMON: return "Summon";
            case EnemyIntent::CHARGE: return "Charge";
            default: return "Unknown";
        }
    }

    int computeVulnerablePercent(const std::vector<StatusEffect>& effects) {
        int bonus = 0;
        for (const auto& e : effects) {
            if (e.appliesVulnerable) {
                bonus = std::max(bonus, e.vulnerablePercent);
            }
        }
        return bonus;
    }

    bool hasStun(const std::vector<StatusEffect>& effects) {
        for (const auto& e : effects) {
            if (e.appliesStun && e.turnsRemaining > 0) {
                return true;
            }
        }
        return false;
    }
}

// ---
// CONSTRUCTOR
// ---

CombatSystem::CombatSystem(Player& player, int difficulty)
    : playerRef(player), gameDifficulty(difficulty),
      combatEnded(false), playerVictory(false) {
    // Clamp difficulty
    if (gameDifficulty < 1) gameDifficulty = 1;
    if (gameDifficulty > 3) gameDifficulty = 3;
}

// ---
// PUBLIC METHODS
// ---

CombatResult CombatSystem::startCombat(std::vector<Enemy>& enemies) {
    CombatResult result;
    combatLog.clear();
    combatEnded = false;
    playerVictory = false;
    
    // Filter out dead enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return e.stats.currentHP <= 0; }),
        enemies.end()
    );
    
    if (enemies.empty()) {
        result.playerWon = true;
        return result;
    }
    
    // Display combat start
    clearScreen();
    std::cout << "\n";
    displaySeparator(60, '=');
    printColor(centerString("!!! COMBAT INITIATED !!!", 60), Color::BRIGHT_RED);
    displaySeparator(60, '=');
    
    // List enemies
    std::cout << "\nYou face:\n";
    for (size_t i = 0; i < enemies.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << colorize(enemies[i].name, Color::RED);
        if (enemies[i].isBoss) std::cout << colorize(" [BOSS]", Color::BRIGHT_MAGENTA);
        std::cout << "\n";
        if (!enemies[i].dialogue.empty()) {
            std::cout << "     \"" << colorize(enemies[i].dialogue, Color::YELLOW) << "\"\n";
        }
    }
    
    log("Combat started against " + std::to_string(enemies.size()) + " enemies.");
    pause("\nPress Enter to begin combat...");
    
    // Main combat loop
    result.playerWon = combatLoop(enemies);
    result.playerDied = !playerRef.isAlive;
    result.playerFled = combatEnded && !result.playerWon && playerRef.isAlive;
    
    // Calculate rewards if won
    if (result.playerWon) {
        result.experienceGained = calculateExperience(enemies);
        result.goldGained = calculateGold(enemies);
        result.loot = generateLoot(enemies);
        
        // Apply rewards
        playerRef.experience += result.experienceGained;
        playerRef.gold += result.goldGained;
        
        // Count kills
        for (const Enemy& enemy : enemies) {
            playerRef.totalKills++;
            if (enemy.isBoss) {
                playerRef.bossesDefeated++;
            }
        }
        
        // Add loot to inventory
        for (const Item& item : result.loot) {
            playerRef.inventory.push_back(item);
        }
        
        // Display victory
        std::cout << "\n";
        displaySeparator(60, '=');
        std::cout << VICTORY_ART;
        displaySeparator(60, '=');
        std::cout << "\nVictory! You have defeated your enemies!\n";
        std::cout << "Experience gained: " << result.experienceGained << "\n";
        std::cout << "Gold gained: " << result.goldGained << "\n";
        
        if (!result.loot.empty()) {
            std::cout << "Items found:\n";
            for (const Item& item : result.loot) {
                std::cout << "  - " << item.name << " [" 
                          << rarityToString(item.rarity) << "]"
                          << " (iLvl " << item.itemLevel;
                if (item.hasUniquePerk && !item.uniquePerkName.empty()) {
                    std::cout << ", Perk: " << item.uniquePerkName;
                }
                std::cout << ")\n";
            }
        }
    } else if (!playerRef.isAlive) {
        // Player died
        std::cout << "\n";
        std::cout << GAME_OVER_ART;
        std::cout << "\nYou have been defeated...\n";
        log("Player died in combat.");
    } else {
        // Player fled
        std::cout << "\nYou escaped from combat!\n";
        log("Player fled from combat.");
    }
    
    result.combatLog = combatLog;
    return result;
}

CombatResult CombatSystem::startCombat(Enemy& enemy) {
    std::vector<Enemy> enemies = { enemy };
    CombatResult result = startCombat(enemies);
    enemy = enemies.empty() ? enemy : enemies[0];
    return result;
}

CombatStats CombatSystem::getEffectivePlayerStats() const {
    CombatStats effective = playerRef.stats;
    
    // Apply equipment bonuses
    for (const Item& item : playerRef.inventory) {
        if (item.isEquipped) {
            effective.attack += item.attackBonus;
            effective.defense += item.defenseBonus;
            effective.critChance += item.critBonus;
            effective.dodgeChance += item.dodgeBonus;
            effective.speed += item.speedBonus;
        }
    }
    
    // Apply active status effects
    for (const StatusEffect& effect : playerRef.activeEffects) {
        effective.attack += effect.attackMod;
        effective.defense += effect.defenseMod;
        effective.critChance += effect.critMod;
        effective.dodgeChance += effect.dodgeMod;
        effective.speed += effect.speedMod;
    }
    
    // Clamp values
    if (effective.critChance > MAX_CRIT_CHANCE) effective.critChance = MAX_CRIT_CHANCE;
    if (effective.dodgeChance > MAX_DODGE_CHANCE) effective.dodgeChance = MAX_DODGE_CHANCE;
    if (effective.attack < 1) effective.attack = 1;
    if (effective.defense < 0) effective.defense = 0;
    
    return effective;
}

CombatStats CombatSystem::getEffectiveEnemyStats(const Enemy& enemy) const {
    CombatStats effective = enemy.stats;
    
    // Apply difficulty modifiers
    if (gameDifficulty == 1) {  // Easy
        effective.attack = static_cast<int>(effective.attack * 0.7f);
        effective.defense = static_cast<int>(effective.defense * 0.7f);
        effective.maxHP = static_cast<int>(effective.maxHP * 0.85f);
    } else if (gameDifficulty == 3) {  // Hard
        effective.attack = static_cast<int>(effective.attack * 1.15f);
        effective.defense = static_cast<int>(effective.defense * 1.1f);
        effective.maxHP = static_cast<int>(effective.maxHP * 1.1f);
    }
    
    return effective;
}

// ---
// PRIVATE METHODS - Combat Flow
// ---

bool CombatSystem::combatLoop(std::vector<Enemy>& enemies) {
    while (!combatEnded) {
        // Process status effects
        processPlayerStatuses();
        for (Enemy& enemy : enemies) {
            processEnemyStatuses(enemy);
        }
        
        // Check if combat ended due to status effects
        if (playerRef.stats.currentHP <= 0) {
            playerRef.isAlive = false;
            combatEnded = true;
            return false;
        }
        
        // Roll intents after statuses tick so telegraphs are accurate
        rollEnemyIntents(enemies);

        // Display current state
        displayCombatStatus(enemies);
        
        // Get turn order
        auto turnOrder = determineTurnOrder(enemies);
        
        for (auto& [isPlayer, index] : turnOrder) {
            if (combatEnded) break;
            
            if (isPlayer) {
                // Player turn
                // Regen Mana (more for casters)
                int manaRegen = (playerRef.learnedSpells.size() > 0) ? 8 : 5;
                if (playerRef.stats.currentMana < playerRef.stats.maxMana) {
                    playerRef.stats.currentMana += manaRegen;
                    if (playerRef.stats.currentMana > playerRef.stats.maxMana) 
                        playerRef.stats.currentMana = playerRef.stats.maxMana;
                }
                
                // Reduce cooldowns for skills
                for(auto& skill : playerRef.skills) {
                    if(skill.currentCooldown > 0) skill.currentCooldown--;
                }
                
                // Reduce cooldowns for spells
                for(auto& spell : playerRef.learnedSpells) {
                    if(spell.currentCooldown > 0) spell.currentCooldown--;
                }

                std::cout << "\n--- YOUR TURN ---\n";
                int action = getPlayerAction();
                
                // Adjust case numbers based on whether magic option is available
                int caseAttack = 1;
                int caseSkills = 2;
                int caseCastSpell = -1;  // Set if available
                int caseUseItem = -1;
                int caseFlee = -1;
                
                // Count available options
                if (playerRef.learnedSpells.size() > 0) {
                    caseCastSpell = 3;
                    caseUseItem = 4;
                    caseFlee = 5;
                } else {
                    caseUseItem = 3;
                    caseFlee = 4;
                }
                
                switch (action) {
                    case 1: {  // Attack
                        int targetIdx = (enemies.size() == 1) ? 0 : selectTarget(enemies);
                        AttackResult result = playerAttack(enemies[targetIdx]);
                        displayMessage(result.message);
                        
                        if (enemies[targetIdx].stats.currentHP <= 0) {
                            displayMessage(enemies[targetIdx].name + " " + 
                                         randomElement(DEATH_TEXT));
                        }
                        break;
                    }
                    case 2: {  // Skills
                        if (playerRef.skills.empty()) {
                            std::cout << "You have no skills learned!\n";
                            // Ideally we'd loop back, but for now we just lose turn
                        } else {
                            std::cout << "\nAvailable Skills:\n";
                            for (size_t i = 0; i < playerRef.skills.size(); ++i) {
                                const Skill& skill = playerRef.skills[i];
                                std::string color = (playerRef.stats.currentMana >= skill.manaCost && skill.currentCooldown <= 0) 
                                                    ? Color::BRIGHT_WHITE : Color::RED;
                                std::cout << "  " << (i + 1) << ". " << colorize(skill.name, color)
                                          << " (MP: " << skill.manaCost << ")";
                                if (skill.currentCooldown > 0) std::cout << " [CD: " << skill.currentCooldown << "]";
                                std::cout << "\n";
                            }
                            std::cout << "  0. Cancel\n";
                            
                            int skillChoice = getValidatedInt("Choose skill: ", 0, static_cast<int>(playerRef.skills.size()));
                            
                            if (skillChoice > 0) {
                                int targetIdx = 0;
                                if (!playerRef.skills[skillChoice-1].isHeal) {
                                    targetIdx = (enemies.size() == 1) ? 0 : selectTarget(enemies);
                                }
                                AbilityResult result = usePlayerAbility(skillChoice - 1, &enemies[targetIdx]);
                                displayMessage(result.message);
                            }
                        }
                        break;
                    }
                    case 3: {  // Cast Spell (if available)
                        if (caseCastSpell == 3) {
                            if (playerRef.learnedSpells.empty()) {
                                std::cout << "You have no spells learned!\n";
                            } else {
                                std::cout << "\nAvailable Spells:\n";
                                for (size_t i = 0; i < playerRef.learnedSpells.size(); ++i) {
                                    const struct Spell& spell = playerRef.learnedSpells[i];
                                    std::string color = (playerRef.stats.currentMana >= spell.manaCost && spell.currentCooldown <= 0) 
                                                        ? Color::BRIGHT_CYAN : Color::RED;
                                    std::cout << "  " << (i + 1) << ". " << colorize(spell.name, color)
                                              << " (MP: " << spell.manaCost << ")";
                                    if (spell.currentCooldown > 0) std::cout << " [CD: " << spell.currentCooldown << "]";
                                    std::cout << "\n";
                                }
                                std::cout << "  0. Cancel\n";
                                
                                int spellChoice = getValidatedInt("Choose spell: ", 0, static_cast<int>(playerRef.learnedSpells.size()));
                                
                                if (spellChoice > 0) {
                                    int targetIdx = 0;
                                    if (!playerRef.learnedSpells[spellChoice-1].isHeal && !playerRef.learnedSpells[spellChoice-1].isAOE) {
                                        targetIdx = (enemies.size() == 1) ? 0 : selectTarget(enemies);
                                    }
                                    SpellResult result = castSpellInCombat(spellChoice - 1, &enemies[targetIdx]);
                                    displayMessage(result.message);
                                }
                            }
                            break;
                        }
                        // Fall through if no spells
                    }
                    case 4: {  // Use item or remaining action
                        if (caseUseItem == action) {
                            auto combatItems = getCombatItemIndices(playerRef);
                            if (combatItems.empty()) {
                                std::cout << "No usable items!\n";
                            } else {
                                std::cout << "\nUsable items:\n";
                                for (size_t i = 0; i < combatItems.size(); ++i) {
                                    const Item& item = playerRef.inventory[combatItems[i]];
                                    std::cout << "  " << (i + 1) << ". " << item.name;
                                    if (item.healAmount > 0) {
                                        std::cout << " (Heals " << item.healAmount << " HP)";
                                    }
                                    std::cout << "\n";
                                }
                                std::cout << "  0. Cancel\n";
                                
                                int choice = getValidatedInt("Choose item: ", 0, 
                                    static_cast<int>(combatItems.size()));
                                if (choice > 0) {
                                    useItemInCombat(combatItems[choice - 1]);
                                }
                            }
                            break;
                        }
                        // Fall through if not item action
                    }
                    case 5: {  // Flee
                        if (caseFlee == action) {
                            if (attemptFlee(enemies)) {
                                combatEnded = true;
                                return false;
                            }
                            break;
                        }
                    }
                }
            } else {
                // Enemy turn
                if (index < 0 || index >= static_cast<int>(enemies.size())) continue;
                Enemy& enemy = enemies[index];
                
                if (enemy.stats.currentHP <= 0) continue;  // Skip dead enemies

                if (hasStun(enemy.activeEffects)) {
                    displayMessage(enemy.name + " is stunned and loses the turn!");
                    continue;
                }
                
                std::cout << "\n--- " << toUpper(enemy.name) << "'S TURN ---\n";
                delay(300);
                
                // Check if enemy uses ability or intent dictates action
                bool acted = false;
                if (enemy.nextIntent == EnemyIntent::HEAL || shouldEnemyUseAbility(enemy)) {
                    AbilityResult result = useEnemyAbility(enemy);
                    displayMessage(result.message);
                    acted = result.used;
                }
                if (!acted) {
                    if (enemy.nextIntent == EnemyIntent::DEFEND) {
                        StatusEffect guard;
                        guard.name = "Guard";
                        guard.turnsRemaining = 2;
                        guard.shieldAmount = enemy.stats.defense / 2 + 5;
                        guard.isDebuff = false;
                        enemy.activeEffects.push_back(guard);
                        displayMessage(enemy.name + " braces for impact, raising a guard!");
                    } else {
                        // Normal attack (heavy charge simply benefits from higher damage roll)
                        AttackResult result = enemyAttack(enemy);
                        displayMessage(result.message);
                        
                        // Check if player died
                        if (playerRef.stats.currentHP <= 0) {
                            playerRef.isAlive = false;
                            combatEnded = true;
                            return false;
                        }
                    }
                }
            }
        }
        
        // Remove dead enemies
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [](const Enemy& e) { return e.stats.currentHP <= 0; }),
            enemies.end()
        );
        
        // Check victory
        if (enemies.empty()) {
            combatEnded = true;
            return true;
        }
        
        playerRef.combatTurns++;
        playerRef.totalTurns = playerRef.explorationTurns + playerRef.combatTurns;
    }
    
    return playerVictory;
}

void CombatSystem::displayCombatStatus(const std::vector<Enemy>& enemies) const {
    clearScreen();
    std::cout << "\n";
    displaySeparator(60, '-');
    printColor(centerString("COMBAT STATUS", 60), Color::BRIGHT_WHITE);
    displaySeparator(60, '-');
    
    // Player status
    std::cout << "\n" << colorize(playerRef.name, Color::BRIGHT_GREEN) 
              << " (" << classToString(playerRef.charClass) << ")\n";
    
    // HP Bar
    std::cout << "HP: ";
    displayProgressBar(playerRef.stats.currentHP, playerRef.stats.maxHP, 25, "");
    std::cout << " " << playerRef.stats.currentHP << "/" << playerRef.stats.maxHP << "\n";
    
    // Mana Bar
    std::cout << "MP: ";
    displayProgressBar(playerRef.stats.currentMana, playerRef.stats.maxMana, 25, "");
    std::cout << " " << playerRef.stats.currentMana << "/" << playerRef.stats.maxMana << "\n";
    
    CombatStats pStats = getEffectivePlayerStats();
    std::cout << "ATK: " << pStats.attack << " | DEF: " << pStats.defense
              << " | CRIT: " << pStats.critChance << "% | SPD: " << pStats.speed << "\n";
    
    // Show active effects
    if (!playerRef.activeEffects.empty()) {
        std::cout << "Effects: ";
        for (const StatusEffect& e : playerRef.activeEffects) {
            std::string color = e.isDebuff ? Color::RED : Color::CYAN;
            std::cout << colorize(e.name, color) << "(" << e.turnsRemaining << ") ";
        }
        std::cout << "\n";
    }
    
    // Enemy status
    std::cout << "\n--- " << colorize("ENEMIES", Color::RED) << " ---\n";
    for (size_t i = 0; i < enemies.size(); ++i) {
        const Enemy& enemy = enemies[i];
        std::cout << "\n" << (i + 1) << ". " << colorize(enemy.name, Color::RED);
        if (enemy.isBoss) std::cout << colorize(" [BOSS]", Color::BRIGHT_MAGENTA);
        std::cout << "\n";
        displayProgressBar(enemy.stats.currentHP, enemy.stats.maxHP, 25, "HP");

        std::cout << "Intent: " << intentToString(enemy.nextIntent) << "\n";
        if (!enemy.activeEffects.empty()) {
            std::cout << "Effects: ";
            for (const StatusEffect& e : enemy.activeEffects) {
                std::string color = e.isDebuff ? Color::RED : Color::CYAN;
                std::cout << colorize(e.name, color) << "(" << e.turnsRemaining << ") ";
            }
            std::cout << "\n";
        }
    }
    
    displaySeparator(60, '-');
}

int CombatSystem::getPlayerAction() const {
    std::vector<std::string> options;
    options.push_back("Attack");
    options.push_back("Skills");
    
    // Add magic option if player can cast
    if (playerRef.learnedSpells.size() > 0) {
        options.push_back("Cast Spell");
    }
    
    options.push_back("Use Item");
    options.push_back("Attempt to Flee");
    
    return displayMenu("Choose your action", options);
}

int CombatSystem::selectTarget(const std::vector<Enemy>& enemies) const {
    std::cout << "\nSelect target:\n";
    for (size_t i = 0; i < enemies.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << enemies[i].name 
                  << " (HP: " << enemies[i].stats.currentHP << "/"
                  << enemies[i].stats.maxHP << ")\n";
    }
    
    return getValidatedInt("Target: ", 1, static_cast<int>(enemies.size())) - 1;
}

std::vector<std::pair<bool, int>> CombatSystem::determineTurnOrder(
    const std::vector<Enemy>& enemies) const {
    
    std::vector<std::pair<int, std::pair<bool, int>>> speedList;
    
    // Add player
    CombatStats pStats = getEffectivePlayerStats();
    speedList.push_back({pStats.speed + randomInt(0, 5), {true, 0}});
    
    // Add enemies
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].stats.currentHP > 0) {
            CombatStats eStats = getEffectiveEnemyStats(enemies[i]);
            speedList.push_back({eStats.speed + randomInt(0, 5), {false, static_cast<int>(i)}});
        }
    }
    
    // Sort by speed (descending)
    std::sort(speedList.begin(), speedList.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Extract turn order
    std::vector<std::pair<bool, int>> result;
    for (const auto& entry : speedList) {
        result.push_back(entry.second);
    }
    
    return result;
}

void CombatSystem::rollEnemyIntents(std::vector<Enemy>& enemies) {
    for (Enemy& enemy : enemies) {
        if (enemy.stats.currentHP <= 0) {
            enemy.nextIntent = EnemyIntent::UNKNOWN;
            continue;
        }

        // Maw enemies use a distinct telegraph profile.
        if (enemy.trueDamage > 0) {
            int roll = randomInt(1, 100);
            if (roll <= 30) {
                enemy.nextIntent = EnemyIntent::CHARGE;
            } else if (roll <= 55) {
                enemy.nextIntent = EnemyIntent::CONTROL;
            } else if (roll <= 80) {
                enemy.nextIntent = EnemyIntent::ATTACK_HEAVY;
            } else {
                enemy.nextIntent = EnemyIntent::ATTACK_LIGHT;
            }
            enemy.intentStrength = enemy.stats.attack + enemy.trueDamage;
            continue;
        }

        // Healing intent if low (very rare — only 5% chance)
        if (enemy.stats.currentHP < enemy.stats.maxHP / 3) {
            bool hasHeal = std::any_of(enemy.abilities.begin(), enemy.abilities.end(),
                [](const EnemyAbility& a) { return a.heals; });
            if (hasHeal) {
                if (randomChance(15)) {
                    enemy.nextIntent = EnemyIntent::HEAL;
                    enemy.intentStrength = 1;
                    continue;
                }
            }
        }

        // Ability-based intents
        EnemyAbility const* readyAbility = nullptr;
        for (const auto& ability : enemy.abilities) {
            if (ability.currentCooldown <= 0) {
                readyAbility = &ability;
                break;
            }
        }

        if (readyAbility) {
            if (readyAbility->heals) {
                // Healing abilities are rare: 15% chance to use heal intent
                if (randomChance(15)) {
                    enemy.nextIntent = EnemyIntent::HEAL;
                    enemy.intentStrength = readyAbility->damage;
                    continue;
                } else {
                    // If not healing, choose a fallback intent based on ability damage
                    if (readyAbility->damage > enemy.stats.attack * 1.2f) {
                        enemy.nextIntent = EnemyIntent::ATTACK_HEAVY;
                    } else {
                        enemy.nextIntent = EnemyIntent::DEBUFF;
                    }
                }
            } else if (readyAbility->damage > enemy.stats.attack * 1.2f) {
                enemy.nextIntent = EnemyIntent::ATTACK_HEAVY;
            } else {
                enemy.nextIntent = EnemyIntent::DEBUFF;
            }
            enemy.intentStrength = readyAbility->damage;
            continue;
        }

        // Default: attack or defend
        if (enemy.stats.currentHP < enemy.stats.maxHP / 2 && randomChance(30)) {
            enemy.nextIntent = EnemyIntent::DEFEND;
            enemy.intentStrength = 0;
        } else {
            enemy.nextIntent = randomChance(30) ? EnemyIntent::ATTACK_HEAVY : EnemyIntent::ATTACK_LIGHT;
            enemy.intentStrength = enemy.stats.attack;
        }
    }
}

// ---
// PRIVATE METHODS - Attack Calculations
// ---

AttackResult CombatSystem::playerAttack(Enemy& target) {
    AttackResult result;
    CombatStats pStats = getEffectivePlayerStats();
    CombatStats eStats = getEffectiveEnemyStats(target);
    
    // Roll to hit
    if (!rollToHit(pStats.speed, eStats.dodgeChance)) {
        result.hit = false;
        result.dodged = true;
        result.message = target.name + " dodges your attack!";
        log(result.message);
        return result;
    }
    
    result.hit = true;
    
    // Roll for critical
    result.critical = rollCritical(pStats.critChance);
    
    // Use tighter player variance so gear quality matters more than high-roll spikes.
    int baseDamage = pStats.attack + randomInt(-1, 3) + 1;
    result.rawDamage = baseDamage;
    result.damageDealt = calculateFinalDamage(baseDamage, eStats.defense, result.critical);

    // Apply vulnerability then shields
    int vuln = computeVulnerablePercent(target.activeEffects);
    if (vuln > 0) {
        result.damageDealt += (result.damageDealt * vuln) / 100;
    }

    result.damageDealt = applyShieldThenDamage(result.damageDealt, target.activeEffects, target.stats.currentHP);
    
    // Generate message
    result.message = generateCombatMessage(playerRef.name, target.name, 
                                           result.damageDealt, result.critical, false);
    log(result.message);
    
    return result;
}

AttackResult CombatSystem::enemyAttack(const Enemy& attacker) {
    AttackResult result;
    CombatStats pStats = getEffectivePlayerStats();
    CombatStats eStats = getEffectiveEnemyStats(attacker);
    
    // Roll to hit
    if (!rollToHit(eStats.speed, pStats.dodgeChance)) {
        result.hit = false;
        result.dodged = true;
        result.message = "You dodge " + attacker.name + "'s attack!";
        log(result.message);
        return result;
    }
    
    result.hit = true;
    
    // Roll for critical
    result.critical = rollCritical(eStats.critChance);
    
    // Keep enemy damage slightly less swingy while preserving threat consistency.
    int baseDamage = eStats.attack + randomInt(-1, 2);
    if (attacker.nextIntent == EnemyIntent::CHARGE) {
        baseDamage += std::max(1, eStats.attack / 4);
    }
    result.rawDamage = baseDamage;
    result.damageDealt = calculateFinalDamage(baseDamage, pStats.defense, result.critical);
    
    // Difficulty already factored into enemy stats, no additional modifier needed
    
    int vuln = computeVulnerablePercent(playerRef.activeEffects);
    if (vuln > 0) {
        result.damageDealt += (result.damageDealt * vuln) / 100;
    }

    result.damageDealt = applyShieldThenDamage(result.damageDealt, playerRef.activeEffects, playerRef.stats.currentHP);

    // The Maw introduces true damage that bypasses mitigation and shields.
    if (attacker.trueDamage > 0) {
        int unavoidable = attacker.trueDamage + randomInt(0, std::max(1, attacker.trueDamage / 2));
        if (attacker.nextIntent == EnemyIntent::CHARGE) {
            unavoidable += std::max(1, attacker.trueDamage / 2);
        }

        playerRef.stats.currentHP -= unavoidable;
        if (playerRef.stats.currentHP < 0) playerRef.stats.currentHP = 0;
        result.damageDealt += unavoidable;
    }
    
    // Generate message
    result.message = generateCombatMessage(attacker.name, playerRef.name,
                                           result.damageDealt, result.critical, false);
    if (attacker.trueDamage > 0) {
        result.message += "\n" + attacker.name + " rends reality for unavoidable damage!";
    }
    log(result.message);
    
    return result;
}

bool CombatSystem::rollToHit(int attackerSpeed, int targetDodge) const {
    // Base hit chance is 90% + speed bonus - dodge
    int hitChance = 90 + attackerSpeed - targetDodge;
    if (hitChance < 30) hitChance = 30;   // Minimum 30% hit chance
    if (hitChance > 98) hitChance = 98;   // Maximum 98% hit chance
    return randomChance(hitChance);
}

bool CombatSystem::rollCritical(int critChance) const {
    return randomChance(critChance);
}

int CombatSystem::calculateFinalDamage(int baseDamage, int defense, bool isCrit) const {
    float damage = static_cast<float>(baseDamage);

    if (isCrit) {
        damage *= BASE_CRIT_MULTIPLIER;
    }

    // Diminishing returns on defense for stability.
    // Higher k and a lower cap reduce late-game over-mitigation from stacked armor.
    const float k = 72.0f;
    float mitigation = static_cast<float>(defense) / (static_cast<float>(defense) + k);
    if (mitigation < 0.0f) mitigation = 0.0f;
    if (mitigation > 0.65f) mitigation = 0.65f; // keep at least 35% damage always going through

    int finalDamage = static_cast<int>(std::floor(damage * (1.0f - mitigation)));
    if (finalDamage < 1) finalDamage = 1;
    return finalDamage;
}

// ---
// PRIVATE METHODS - Abilities
// ---

AbilityResult CombatSystem::usePlayerAbility(int abilityIndex, Enemy* target) {
    AbilityResult result;
    
    if (abilityIndex < 0 || abilityIndex >= static_cast<int>(playerRef.skills.size())) {
        result.used = false;
        result.message = "Invalid skill!";
        return result;
    }
    
    Skill& skill = playerRef.skills[abilityIndex];
    
    // Check Cooldown
    if (skill.currentCooldown > 0) {
        result.used = false;
        result.message = skill.name + " is on cooldown! (" + std::to_string(skill.currentCooldown) + " turns)";
        return result;
    }
    
    // Check Mana
    if (playerRef.stats.currentMana < skill.manaCost) {
        result.used = false;
        result.message = "Not enough Mana! Need " + std::to_string(skill.manaCost);
        return result;
    }
    
    // Consume Mana
    playerRef.stats.currentMana -= skill.manaCost;
    skill.currentCooldown = skill.cooldown;
    
    result.used = true;
    result.success = true;
    
    if (skill.isHeal) {
        // Healing Skill
        int healAmount = static_cast<int>(playerRef.stats.magicPower * skill.damageMultiplier);
        int oldHP = playerRef.stats.currentHP;
        playerRef.stats.currentHP += healAmount;
        if (playerRef.stats.currentHP > playerRef.stats.maxHP) {
            playerRef.stats.currentHP = playerRef.stats.maxHP;
        }
        result.healingDone = playerRef.stats.currentHP - oldHP;
        result.message = "You cast " + colorize(skill.name, Color::CYAN) + 
                         " and heal for " + colorize(std::to_string(result.healingDone), Color::GREEN) + " HP!";
    } else {
        // Offensive Skill
        if (target == nullptr) {
            result.used = false;
            result.message = "No target selected!";
            return result; // Should refund mana/cooldown but keeping simple
        }
        
        CombatStats pStats = getEffectivePlayerStats();
        CombatStats eStats = getEffectiveEnemyStats(*target);
        
        int baseDamage = skill.isMagical ? pStats.magicPower : pStats.attack;
        int defense = skill.isMagical ? eStats.magicResist : eStats.defense;
        
        int damage = static_cast<int>(baseDamage * skill.damageMultiplier) - defense;
        if (damage < 1) damage = 1;
        
        // Apply Crit (Spells can crit too now)
        if (randomChance(pStats.critChance)) {
            damage *= BASE_CRIT_MULTIPLIER;
            result.message = "CRITICAL HIT! ";
        }
        
        target->stats.currentHP -= damage;
        if (target->stats.currentHP < 0) target->stats.currentHP = 0;
        
        result.damageDealt = damage;
        result.message += "You use " + colorize(skill.name, Color::CYAN) + " on " + 
                          colorize(target->name, Color::RED) + " for " + 
                          colorize(std::to_string(damage), Color::BRIGHT_RED) + " damage!";
                          
        // Apply status effect if any
        if (skill.effect.turnsRemaining > 0) {
            target->activeEffects.push_back(skill.effect);
            result.message += "\n" + target->name + " is affected by " + skill.effect.name + "!";
        }
    }
    
    log(result.message);
    return result;
}

SpellResult CombatSystem::castSpellInCombat(int spellIndex, Enemy* target) {
    SpellResult result;
    
    if (spellIndex < 0 || spellIndex >= static_cast<int>(playerRef.learnedSpells.size())) {
        result.cast = false;
        result.message = "Invalid spell!";
        return result;
    }
    
    Spell& spell = playerRef.learnedSpells[spellIndex];
    
    // Check Cooldown
    if (spell.currentCooldown > 0) {
        result.cast = false;
        result.message = spell.name + " is on cooldown! (" + std::to_string(spell.currentCooldown) + " turns)";
        return result;
    }
    
    // Check Mana
    if (playerRef.stats.currentMana < spell.manaCost) {
        result.cast = false;
        result.message = "Not enough Mana! Need " + std::to_string(spell.manaCost);
        return result;
    }
    
    // Consume Mana
    playerRef.stats.currentMana -= spell.manaCost;
    spell.currentCooldown = spell.cooldown;
    
    result.cast = true;
    
    // Healing spell
    if (spell.isHeal) {
        int healAmount = static_cast<int>(playerRef.stats.magicPower * spell.healMultiplier);
        int oldHP = playerRef.stats.currentHP;
        playerRef.stats.currentHP += healAmount;
        if (playerRef.stats.currentHP > playerRef.stats.maxHP) {
            playerRef.stats.currentHP = playerRef.stats.maxHP;
        }
        result.healingDone = playerRef.stats.currentHP - oldHP;
        result.message = "You cast " + colorize(spell.name, Color::BRIGHT_CYAN) + 
                         " and heal for " + colorize(std::to_string(result.healingDone), Color::GREEN) + " HP!";
        log(result.message);
        return result;
    }
    
    // Offensive spell - need target
    if (target == nullptr && !spell.isAOE) {
        result.cast = false;
        result.message = "No target selected!";
        return result;
    }
    
    // Single target offensive spell
    if (!spell.isAOE) {
        CombatStats pStats = getEffectivePlayerStats();
        CombatStats eStats = getEffectiveEnemyStats(*target);
        
        int baseDamage = static_cast<int>(pStats.magicPower * spell.damageMultiplier);
        int defense = eStats.magicResist;
        
        int damage = baseDamage - defense;
        if (damage < 1) damage = 1;
        
        // Apply Crit
        if (randomChance(pStats.critChance)) {
            damage *= BASE_CRIT_MULTIPLIER;
            result.message = "CRITICAL SPELL! ";
        }
        
        target->stats.currentHP -= damage;
        if (target->stats.currentHP < 0) target->stats.currentHP = 0;
        
        result.damageDealt = damage;
        result.affectedTargets.push_back(0);
        result.message += "You cast " + colorize(spell.name, Color::BRIGHT_CYAN) + " on " + 
                          colorize(target->name, Color::RED) + " for " + 
                          colorize(std::to_string(damage), Color::BRIGHT_RED) + " damage!";
        
        // Apply Dot
        if (spell.dotDamage > 0) {
            StatusEffect dotEffect;
            dotEffect.name = spell.name + " Dot";
            dotEffect.dotDamage = spell.dotDamage;
            dotEffect.dotIsMagic = true;
            dotEffect.turnsRemaining = spell.dotTurns;
            dotEffect.isDebuff = true;
            target->activeEffects.push_back(dotEffect);
            result.message += "\n" + target->name + " is burning!";
        }
        
        // Apply effect
        if (spell.appliedEffect.turnsRemaining > 0) {
            target->activeEffects.push_back(spell.appliedEffect);
            result.message += "\n" + target->name + " is " + spell.appliedEffect.name + "!";
        }
    } else {
        // AOE spell - hit all enemies
        CombatStats pStats = getEffectivePlayerStats();
        int baseDamage = static_cast<int>(pStats.magicPower * spell.damageMultiplier);
        
        result.message = "You cast " + colorize(spell.name, Color::BRIGHT_CYAN) + " on all enemies!\n";
        
        for (size_t i = 0; i < playerRef.currentCombatEnemies.size(); ++i) {
            Enemy& enemy = playerRef.currentCombatEnemies[i];
            if (enemy.stats.currentHP <= 0) continue;
            
            CombatStats eStats = getEffectiveEnemyStats(enemy);
            int defense = eStats.magicResist;
            
            int damage = baseDamage - defense;
            if (damage < 1) damage = 1;
            
            if (randomChance(pStats.critChance)) {
                damage *= BASE_CRIT_MULTIPLIER;
            }
            
            enemy.stats.currentHP -= damage;
            if (enemy.stats.currentHP < 0) enemy.stats.currentHP = 0;
            
            result.damageDealt += damage;
            result.affectedTargets.push_back(i);
            result.message += "  " + enemy.name + " takes " + std::to_string(damage) + " damage!\n";
            
            // Apply effect to all
            if (spell.appliedEffect.turnsRemaining > 0) {
                enemy.activeEffects.push_back(spell.appliedEffect);
            }
        }
    }
    
    log(result.message);
    return result;
}

AbilityResult CombatSystem::useEnemyAbility(Enemy& enemy) {
    AbilityResult result;
    
    if (enemy.abilities.empty()) {
        // Fallback to normal attack
        AttackResult attack = enemyAttack(enemy);
        result.message = attack.message;
        return result;
    }
    
    // Find usable ability (not on cooldown)
    for (EnemyAbility& ability : enemy.abilities) {
        if (ability.currentCooldown <= 0) {
            result.used = true;
            result.success = true;
            
            if (ability.heals) {
                // Healing ability
                int healed = ability.healAmount;
                enemy.stats.currentHP += healed;
                if (enemy.stats.currentHP > enemy.stats.maxHP) {
                    enemy.stats.currentHP = enemy.stats.maxHP;
                }
                result.healingDone = healed;
                result.message = enemy.name + " uses " + ability.name + 
                                " and heals for " + std::to_string(healed) + " HP!";
            } else {
                // Damage ability
                CombatStats pStats = getEffectivePlayerStats();
                int damage = calculateFinalDamage(ability.damage, pStats.defense, false);
                int vuln = computeVulnerablePercent(playerRef.activeEffects);
                if (vuln > 0) {
                    damage += (damage * vuln) / 100;
                }

                damage = applyShieldThenDamage(damage, playerRef.activeEffects, playerRef.stats.currentHP);
                if (playerRef.stats.currentHP < 0) playerRef.stats.currentHP = 0;

                result.damageDealt = damage;
                result.message = enemy.name + " uses " + ability.name + 
                                "! You take " + std::to_string(damage) + " damage!";
            }
            
            ability.currentCooldown = ability.cooldown;
            log(result.message);
            return result;
        }
    }
    
    // All abilities on cooldown, normal attack
    AttackResult attack = enemyAttack(enemy);
    result.message = attack.message;
    return result;
}

bool CombatSystem::shouldEnemyUseAbility(const Enemy& enemy) const {
    if (enemy.abilities.empty()) return false;
    
    for (const EnemyAbility& ability : enemy.abilities) {
        if (ability.currentCooldown <= 0 && randomFloat() < ability.triggerChance) {
            return true;
        }
    }
    return false;
}

// ---
// PRIVATE METHODS - Status Effects
// ---

void CombatSystem::applyStatusToPlayer(const StatusEffect& effect) {
    playerRef.activeEffects.push_back(effect);
    log("Status applied: " + effect.name);
}

void CombatSystem::processPlayerStatuses() {
    tickStatuses(playerRef.activeEffects, playerRef.stats, true);
    cleanupStatuses(playerRef.activeEffects);
}

void CombatSystem::processEnemyStatuses(Enemy& enemy) {
    // Reduce ability cooldowns
    for (EnemyAbility& ability : enemy.abilities) {
        if (ability.currentCooldown > 0) {
            ability.currentCooldown--;
        }
    }

    tickStatuses(enemy.activeEffects, enemy.stats, false);
    cleanupStatuses(enemy.activeEffects);
}

void CombatSystem::tickStatuses(std::vector<StatusEffect>& effects, CombatStats& stats, bool isPlayer) {
    for (StatusEffect& effect : effects) {
        if (effect.turnsRemaining > 0) {
            // Damage over time
            if (effect.dotDamage > 0) {
                int dmg = effect.dotDamage;
                stats.currentHP -= dmg;
                if (stats.currentHP < 0) stats.currentHP = 0;
                log((isPlayer ? "Player" : "Enemy") + std::string(" suffers ") + std::to_string(dmg) + " from " + effect.name);
            }

            effect.turnsRemaining--;
        }
    }
}

void CombatSystem::cleanupStatuses(std::vector<StatusEffect>& effects) {
    effects.erase(
        std::remove_if(effects.begin(), effects.end(), [](const StatusEffect& e) {
            return e.turnsRemaining <= 0 && e.shieldAmount <= 0;
        }),
        effects.end());
}

int CombatSystem::applyShieldThenDamage(int incoming, std::vector<StatusEffect>& effects, int& hpPool) const {
    int remaining = incoming;
    int absorbed = 0;

    for (StatusEffect& e : effects) {
        if (e.shieldAmount <= 0 || remaining <= 0) continue;
        int used = std::min(e.shieldAmount, remaining);
        e.shieldAmount -= used;
        remaining -= used;
        absorbed += used;
    }

    if (remaining > 0) {
        int before = hpPool;
        hpPool -= remaining;
        if (hpPool < 0) hpPool = 0;
        absorbed += (before - hpPool);
    }

    return absorbed;
}

// ---
// PRIVATE METHODS - Flee & Items
// ---

bool CombatSystem::attemptFlee(const std::vector<Enemy>& enemies) {
    // Calculate flee chance based on speed difference
    CombatStats pStats = getEffectivePlayerStats();
    
    if (enemies.empty()) {
        return 50;  // Safe default if no enemies (shouldn't happen in normal gameplay)
    }
    
    int totalEnemySpeed = 0;
    for (const Enemy& enemy : enemies) {
        totalEnemySpeed += getEffectiveEnemyStats(enemy).speed;
    }
    int avgEnemySpeed = totalEnemySpeed / static_cast<int>(enemies.size());
    
    int speedDiff = pStats.speed - avgEnemySpeed;
    int fleeChance = FLEE_BASE_CHANCE + (speedDiff * SPEED_FLEE_BONUS);
    
    if (fleeChance < 10) fleeChance = 10;
    if (fleeChance > 80) fleeChance = 80;
    
    // Bosses are harder to flee from
    for (const Enemy& enemy : enemies) {
        if (enemy.isBoss) {
            fleeChance /= 2;
            break;
        }
    }
    
    if (randomChance(fleeChance)) {
        log("Player fled successfully.");
        return true;
    } else {
        displayMessage("You failed to escape!");
        log("Player failed to flee.");
        return false;
    }
}

bool CombatSystem::useItemInCombat(int itemIndex) {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(playerRef.inventory.size())) {
        return false;
    }
    
    Item& item = playerRef.inventory[itemIndex];
    
    if (!item.isUsable || item.type != ItemType::CONSUMABLE) {
        std::cout << "You can't use that in combat!\n";
        return false;
    }
    
    // Apply item effects
    if (item.healAmount > 0) {
        int healed = item.healAmount;
        playerRef.stats.currentHP += healed;
        if (playerRef.stats.currentHP > playerRef.stats.maxHP) {
            playerRef.stats.currentHP = playerRef.stats.maxHP;
        }
        std::cout << "You use " << item.name << " and restore " << healed << " HP!\n";
        log("Used " + item.name + ", healed " + std::to_string(healed) + " HP.");
    }
    
    // Apply buffs if any
    if (item.duration > 0) {
        StatusEffect buff;
        buff.name = item.name + " Effect";
        buff.attackMod = item.attackBonus;
        buff.defenseMod = item.defenseBonus;
        buff.critMod = item.critBonus;
        buff.dodgeMod = item.dodgeBonus;
        buff.speedMod = item.speedBonus;
        buff.turnsRemaining = item.duration;
        buff.isDebuff = false;
        applyStatusToPlayer(buff);
        
        std::cout << "You gain a temporary buff!\n";
    }
    
    // Consume item
    item.quantity--;
    if (item.quantity <= 0) {
        playerRef.inventory.erase(playerRef.inventory.begin() + itemIndex);
    }
    
    return true;
}

// ---
// PRIVATE METHODS - Loot
// ---

std::vector<Item> CombatSystem::generateLoot(const std::vector<Enemy>& enemies) {
    std::vector<Item> loot;
    
    for (const Enemy& enemy : enemies) {
        std::vector<Item> enemyLoot = rollLootTable(enemy);
        loot.insert(loot.end(), enemyLoot.begin(), enemyLoot.end());
    }
    
    return loot;
}

std::vector<Item> CombatSystem::rollLootTable(const Enemy& enemy) {
    std::vector<Item> drops;
    const int enemyPowerLevel = estimateEnemyPowerLevel(enemy);
    const int contextLevel = std::max(playerRef.level, enemyPowerLevel);
    
    // Use enemy's loot table if available
    for (size_t i = 0; i < enemy.lootTable.size() && i < enemy.lootChances.size(); ++i) {
        if (randomChance(enemy.lootChances[i])) {
            ItemRarity rolledRarity = ItemRarity::COMMON;
            
            // Determine rarity based on enemy
            if (enemy.isBoss) {
                if (randomChance(LEGENDARY_DROP_RATE * 5)) {
                    rolledRarity = ItemRarity::LEGENDARY;
                } else if (randomChance(EPIC_DROP_RATE * 5)) {
                    rolledRarity = ItemRarity::EPIC;
                } else {
                    rolledRarity = ItemRarity::RARE;
                }
            } else {
                int roll = randomInt(1, 100);
                if (roll <= LEGENDARY_DROP_RATE) rolledRarity = ItemRarity::LEGENDARY;
                else if (roll <= LEGENDARY_DROP_RATE + EPIC_DROP_RATE) rolledRarity = ItemRarity::EPIC;
                else if (roll <= LEGENDARY_DROP_RATE + EPIC_DROP_RATE + RARE_DROP_RATE) 
                    rolledRarity = ItemRarity::RARE;
                else if (roll <= LEGENDARY_DROP_RATE + EPIC_DROP_RATE + RARE_DROP_RATE + UNCOMMON_DROP_RATE)
                    rolledRarity = ItemRarity::UNCOMMON;
                else rolledRarity = ItemRarity::COMMON;
            }

            const int dropContextLevel = contextLevel + (enemy.isBoss ? 3 : 0);
            Item drop = generateRandomItem(rolledRarity, std::max(1, dropContextLevel));
            if (enemy.lootTable[i] > 0) {
                drop.id = enemy.lootTable[i];
            }
            
            drops.push_back(drop);
        }
    }
    
    // If no loot table, generate random consumable
    if (drops.empty() && randomChance(30)) {
        Item potion;
        potion.id = 1000 + randomInt(1, 100);
        potion.name = "Minor Health Potion";
        potion.description = "Restores a small amount of health.";
        potion.type = ItemType::CONSUMABLE;
        potion.rarity = ItemRarity::COMMON;
        potion.healAmount = 20;
        potion.quantity = 1;
        potion.value = 10;
        const int potionItemLevel = rollItemLevel(contextLevel, potion.rarity, enemy.isBoss);
        applyItemLevelScaling(potion, potionItemLevel, true);
        drops.push_back(potion);
    }
    
    return drops;
}

int CombatSystem::calculateExperience(const std::vector<Enemy>& enemies) const {
    int total = 0;
    for (const Enemy& enemy : enemies) {
        total += enemy.experienceReward;
    }
    
    // Difficulty bonus
    if (gameDifficulty == 3) {
        total = static_cast<int>(total * 1.5f);
    } else if (gameDifficulty == 1) {
        total = static_cast<int>(total * 0.75f);
    }
    
    return total;
}

int CombatSystem::calculateGold(const std::vector<Enemy>& enemies) const {
    int total = 0;
    for (const Enemy& enemy : enemies) {
        total += enemy.goldReward + randomInt(-2, 5);
    }
    
    // Difficulty modifier
    if (gameDifficulty == 1) {
        total = static_cast<int>(total * EASY_GOLD_MULT);
    } else if (gameDifficulty == 3) {
        total = static_cast<int>(total * HARD_GOLD_MULT);
    }
    
    return (total < 0) ? 0 : total;
}

// ---
// PRIVATE METHODS - Logging
// ---

void CombatSystem::log(const std::string& message) {
    combatLog.push_back(message);
}

void CombatSystem::displayMessage(const std::string& message, int delayMs) const {
    std::cout << "\n" << message << "\n";
    delay(delayMs);
}

// ---
// STANDALONE COMBAT FUNCTIONS
// ---

bool quickCombat(Player& player, Enemy& enemy) {
    CombatSystem combat(player, 2);
    CombatResult result = combat.startCombat(enemy);
    return result.playerWon;
}

bool hasCombatItems(const Player& player) {
    for (const Item& item : player.inventory) {
        if (item.isUsable && item.type == ItemType::CONSUMABLE) {
            return true;
        }
    }
    return false;
}

std::vector<int> getCombatItemIndices(const Player& player) {
    std::vector<int> indices;
    for (size_t i = 0; i < player.inventory.size(); ++i) {
        const Item& item = player.inventory[i];
        if (item.isUsable && item.type == ItemType::CONSUMABLE) {
            indices.push_back(static_cast<int>(i));
        }
    }
    return indices;
}

int applyDifficultyModifier(int damage, int difficulty, bool isPlayerDamage) {
    if (difficulty == 1) {  // Easy
        if (isPlayerDamage) {
            return static_cast<int>(damage * 1.2f);
        } else {
            return static_cast<int>(damage * EASY_DAMAGE_MULT);
        }
    } else if (difficulty == 3) {  // Hard
        if (isPlayerDamage) {
            return static_cast<int>(damage * 0.9f);
        } else {
            return static_cast<int>(damage * HARD_DAMAGE_MULT);
        }
    }
    return damage;
}

std::string generateCombatMessage(const std::string& attackerName,
                                  const std::string& defenderName,
                                  int damage, bool isCrit, bool isMiss) {
    std::stringstream ss;
    
    if (isMiss) {
        ss << attackerName << " " << randomElement(COMBAT_MISS_TEXT) 
           << " " << defenderName << "!";
    } else if (isCrit) {
        ss << attackerName << " " << randomElement(COMBAT_CRIT_TEXT)
           << " " << defenderName << " for " << damage << " CRITICAL damage!";
    } else {
        ss << attackerName << " " << randomElement(COMBAT_HIT_TEXT)
           << " " << defenderName << " for " << damage << " damage.";
    }
    
    return ss.str();
}

}  // namespace AQE
