//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Combat system implementation - Enhanced UI Version

#include "../include/Combat.h"
#include "../include/Utils.h"
#include "../include/Constants.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <iomanip>

namespace AQE {

// ============================================================================
// ENHANCED COMBAT UI CONSTANTS AND HELPERS
// ============================================================================

namespace {
    // Box drawing characters for enhanced UI
    const std::string BOX_TOP_LEFT = "╔";
    const std::string BOX_TOP_RIGHT = "╗";
    const std::string BOX_BOTTOM_LEFT = "╚";
    const std::string BOX_BOTTOM_RIGHT = "╝";
    const std::string BOX_HORIZONTAL = "═";
    const std::string BOX_VERTICAL = "║";
    const std::string BOX_T_RIGHT = "╠";
    const std::string BOX_T_LEFT = "╣";
    const std::string BOX_T_DOWN = "╦";
    const std::string BOX_T_UP = "╩";
    const std::string BOX_CROSS = "╬";
    
    // Single line box characters
    const std::string THIN_TOP_LEFT = "┌";
    const std::string THIN_TOP_RIGHT = "┐";
    const std::string THIN_BOTTOM_LEFT = "└";
    const std::string THIN_BOTTOM_RIGHT = "┘";
    const std::string THIN_HORIZONTAL = "─";
    const std::string THIN_VERTICAL = "│";
    
    // Progress bar characters
    const std::string BAR_FULL = "█";
    const std::string BAR_PARTIAL = "▓";
    const std::string BAR_LIGHT = "░";
    const std::string BAR_EMPTY = "·";
    
    // Combat icons - ASCII compatible for wider terminal support
    const std::string ICON_SWORD = "[ATK]";
    const std::string ICON_SHIELD = "[DEF]";
    const std::string ICON_HEART = "[HP]";
    const std::string ICON_MANA = "[MP]";
    const std::string ICON_SKULL = "[X]";
    const std::string ICON_STAR = "*";
    const std::string ICON_ARROW = ">";
    const std::string ICON_LIGHTNING = "[!]";
    const std::string ICON_FIRE = "[F]";
    const std::string ICON_MAGIC = "[M]";
    
    // Combat width constant
    const int COMBAT_WIDTH = 70;

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
    
    std::string getIntentIcon(EnemyIntent intent) {
        switch (intent) {
            case EnemyIntent::ATTACK_LIGHT: return "⚔";
            case EnemyIntent::ATTACK_HEAVY: return "💥";
            case EnemyIntent::DEFEND: return "🛡";
            case EnemyIntent::DEBUFF: return "💀";
            case EnemyIntent::BUFF: return "✨";
            case EnemyIntent::HEAL: return "💚";
            case EnemyIntent::CONTROL: return "🌀";
            case EnemyIntent::SUMMON: return "👥";
            case EnemyIntent::CHARGE: return "⚡";
            default: return "?";
        }
    }
    
    std::string getIntentColor(EnemyIntent intent) {
        switch (intent) {
            case EnemyIntent::ATTACK_LIGHT: return Color::YELLOW;
            case EnemyIntent::ATTACK_HEAVY: return Color::BRIGHT_RED;
            case EnemyIntent::DEFEND: return Color::CYAN;
            case EnemyIntent::DEBUFF: return Color::MAGENTA;
            case EnemyIntent::BUFF: return Color::GREEN;
            case EnemyIntent::HEAL: return Color::BRIGHT_GREEN;
            case EnemyIntent::CONTROL: return Color::BLUE;
            case EnemyIntent::SUMMON: return Color::BRIGHT_MAGENTA;
            case EnemyIntent::CHARGE: return Color::BRIGHT_YELLOW;
            default: return Color::WHITE;
        }
    }
    
    // Draw enhanced health bar with gradient colors
    std::string drawEnhancedHealthBar(int current, int max, int width, bool isPlayer = true) {
        std::stringstream ss;
        float ratio = static_cast<float>(current) / static_cast<float>(max);
        if (ratio < 0) ratio = 0;
        if (ratio > 1) ratio = 1;
        
        int filled = static_cast<int>(ratio * width);
        
        // Determine color based on health percentage
        std::string barColor;
        if (ratio > 0.6f) {
            barColor = isPlayer ? Color::BRIGHT_GREEN : Color::BRIGHT_RED;
        } else if (ratio > 0.3f) {
            barColor = Color::YELLOW;
        } else {
            barColor = Color::RED;
        }
        
        ss << barColor;
        for (int i = 0; i < width; ++i) {
            if (i < filled) {
                ss << BAR_FULL;
            } else {
                ss << BAR_EMPTY;
            }
        }
        ss << Color::RESET;
        
        return ss.str();
    }
    
    // Draw mana bar
    std::string drawManaBar(int current, int max, int width) {
        std::stringstream ss;
        float ratio = static_cast<float>(current) / static_cast<float>(max);
        if (ratio < 0) ratio = 0;
        if (ratio > 1) ratio = 1;
        
        int filled = static_cast<int>(ratio * width);
        
        ss << Color::BRIGHT_BLUE;
        for (int i = 0; i < width; ++i) {
            if (i < filled) {
                ss << BAR_FULL;
            } else {
                ss << BAR_EMPTY;
            }
        }
        ss << Color::RESET;
        
        return ss.str();
    }
    
    // Draw a horizontal line with box characters
    void drawCombatLine(char type = '-') {
        if (type == '=') {
            std::cout << BOX_T_RIGHT;
            for (int i = 0; i < COMBAT_WIDTH - 2; ++i) std::cout << BOX_HORIZONTAL;
            std::cout << BOX_T_LEFT << "\n";
        } else {
            std::cout << BOX_VERTICAL << " ";
            for (int i = 0; i < COMBAT_WIDTH - 4; ++i) std::cout << THIN_HORIZONTAL;
            std::cout << " " << BOX_VERTICAL << "\n";
        }
    }
    
    // Draw top border
    void drawCombatTop() {
        std::cout << Color::CYAN << BOX_TOP_LEFT;
        for (int i = 0; i < COMBAT_WIDTH - 2; ++i) std::cout << BOX_HORIZONTAL;
        std::cout << BOX_TOP_RIGHT << Color::RESET << "\n";
    }
    
    // Draw bottom border
    void drawCombatBottom() {
        std::cout << Color::CYAN << BOX_BOTTOM_LEFT;
        for (int i = 0; i < COMBAT_WIDTH - 2; ++i) std::cout << BOX_HORIZONTAL;
        std::cout << BOX_BOTTOM_RIGHT << Color::RESET << "\n";
    }
    
    // Draw a centered title in the combat box
    void drawCombatTitle(const std::string& title, const std::string& color = Color::BRIGHT_WHITE) {
        int padding = (COMBAT_WIDTH - 2 - static_cast<int>(title.length())) / 2;
        std::cout << Color::CYAN << BOX_VERTICAL << Color::RESET;
        for (int i = 0; i < padding; ++i) std::cout << " ";
        std::cout << color << title << Color::RESET;
        int remaining = COMBAT_WIDTH - 2 - padding - static_cast<int>(title.length());
        for (int i = 0; i < remaining; ++i) std::cout << " ";
        std::cout << Color::CYAN << BOX_VERTICAL << Color::RESET << "\n";
    }
    
    // Draw a text line in the combat box
    void drawCombatText(const std::string& text, const std::string& color = Color::WHITE) {
        std::cout << Color::CYAN << BOX_VERTICAL << Color::RESET << " ";
        std::cout << color << std::left << std::setw(COMBAT_WIDTH - 4) << text << Color::RESET;
        std::cout << " " << Color::CYAN << BOX_VERTICAL << Color::RESET << "\n";
    }
    
    // Draw empty line in combat box
    void drawCombatEmpty() {
        std::cout << Color::CYAN << BOX_VERTICAL << Color::RESET;
        for (int i = 0; i < COMBAT_WIDTH - 2; ++i) std::cout << " ";
        std::cout << Color::CYAN << BOX_VERTICAL << Color::RESET << "\n";
    }
    
    // ASCII art for attack animations
    void showAttackAnimation(const std::string& attackerName, bool isPlayer, bool isCrit) {
        if (isCrit) {
            std::cout << "\n";
            std::cout << Color::BRIGHT_YELLOW << "    * * * CRITICAL HIT * * *" << Color::RESET << "\n";
            std::cout << Color::BRIGHT_RED;
            std::cout << "         \\  |  /\n";
            std::cout << "          \\ | /\n";
            std::cout << "       ----*----\n";
            std::cout << "          / | \\\n";
            std::cout << "         /  |  \\\n";
            std::cout << Color::RESET;
        } else if (isPlayer) {
            std::cout << Color::BRIGHT_GREEN << "    " << attackerName << " attacks!" << Color::RESET << "\n";
            std::cout << Color::WHITE;
            std::cout << "       ───━━━━══►\n";
            std::cout << Color::RESET;
        } else {
            std::cout << Color::BRIGHT_RED << "    " << attackerName << " attacks!" << Color::RESET << "\n";
            std::cout << Color::RED;
            std::cout << "       ◄══━━━━───\n";
            std::cout << Color::RESET;
        }
    }
    
    // Show damage number effect
    void showDamageEffect(int damage, bool isCrit, bool isPlayerDamaged) {
        std::string color = isPlayerDamaged ? Color::BRIGHT_RED : Color::BRIGHT_GREEN;
        if (isCrit) {
            std::cout << color << "         -" << damage << "! *" << Color::RESET << "\n";
        } else {
            std::cout << color << "         -" << damage << Color::RESET << "\n";
        }
    }
    
    // Show dodge effect
    void showDodgeEffect(const std::string& dodgerName) {
        std::cout << Color::BRIGHT_CYAN << "    " << dodgerName << " dodges gracefully!" << Color::RESET << "\n";
        std::cout << Color::CYAN;
        std::cout << "         ~ MISS ~\n";
        std::cout << Color::RESET;
    }
    
    // Show healing effect
    void showHealEffect(int amount) {
        std::cout << Color::BRIGHT_GREEN;
        std::cout << "       + +" << amount << " HP +\n";
        std::cout << Color::RESET;
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
    
    // Display enhanced combat start screen
    clearScreen();
    std::cout << "\n";
    
    // Epic combat initiation banner
    drawCombatTop();
    drawCombatEmpty();
    drawCombatTitle("⚔  COMBAT INITIATED  ⚔", Color::BRIGHT_RED);
    drawCombatEmpty();
    drawCombatLine('=');
    drawCombatEmpty();
    
    // List enemies with enhanced formatting
    std::string enemyHeader = "You face " + std::to_string(enemies.size()) + " opponent" + (enemies.size() > 1 ? "s:" : ":");
    drawCombatText(enemyHeader, Color::WHITE);
    drawCombatEmpty();
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        std::stringstream enemyLine;
        enemyLine << "  " << (i + 1) << ". ";
        if (enemies[i].isBoss) {
            enemyLine << "* " << enemies[i].name << " * [BOSS]";
            drawCombatText(enemyLine.str(), Color::BRIGHT_MAGENTA);
        } else {
            enemyLine << enemies[i].name;
            drawCombatText(enemyLine.str(), Color::BRIGHT_RED);
        }
        
        if (!enemies[i].dialogue.empty()) {
            std::string dialogue = "     \"" + enemies[i].dialogue + "\"";
            drawCombatText(dialogue, Color::YELLOW);
        }
    }
    
    drawCombatEmpty();
    drawCombatBottom();
    
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
        
        // Display enhanced victory screen
        clearScreen();
        std::cout << "\n";
        drawCombatTop();
        drawCombatEmpty();
        drawCombatTitle("* * *  VICTORY  * * *", Color::BRIGHT_YELLOW);
        drawCombatEmpty();
        drawCombatLine('=');
        drawCombatEmpty();
        drawCombatText("You have defeated your enemies!", Color::BRIGHT_GREEN);
        drawCombatEmpty();
        
        // Display rewards with icons
        std::stringstream xpLine;
        xpLine << "  + Experience: +" << result.experienceGained << " XP";
        drawCombatText(xpLine.str(), Color::BRIGHT_CYAN);
        
        std::stringstream goldLine;
        goldLine << "  $ Gold: +" << result.goldGained << " gold";
        drawCombatText(goldLine.str(), Color::BRIGHT_YELLOW);
        
        if (!result.loot.empty()) {
            drawCombatEmpty();
            drawCombatText("  Items Found:", Color::WHITE);
            for (const Item& item : result.loot) {
                std::stringstream lootLine;
                lootLine << "    • " << item.name;
                
                // Color based on rarity
                std::string rarityColor;
                switch (item.rarity) {
                    case ItemRarity::LEGENDARY: rarityColor = Color::BRIGHT_YELLOW; break;
                    case ItemRarity::EPIC: rarityColor = Color::BRIGHT_MAGENTA; break;
                    case ItemRarity::RARE: rarityColor = Color::BRIGHT_BLUE; break;
                    case ItemRarity::UNCOMMON: rarityColor = Color::BRIGHT_GREEN; break;
                    default: rarityColor = Color::WHITE; break;
                }
                lootLine << " [" << rarityToString(item.rarity) << "]";
                drawCombatText(lootLine.str(), rarityColor);
            }
        }
        
        drawCombatEmpty();
        drawCombatBottom();
    } else if (!playerRef.isAlive) {
        // Enhanced death screen
        clearScreen();
        std::cout << "\n";
        drawCombatTop();
        drawCombatEmpty();
        drawCombatTitle("DEFEAT", Color::BRIGHT_RED);
        drawCombatEmpty();
        drawCombatLine('=');
        drawCombatEmpty();
        drawCombatText("You have been defeated...", Color::RED);
        drawCombatText("Your adventure ends here.", Color::WHITE);
        drawCombatEmpty();
        drawCombatBottom();
        log("Player died in combat.");
    } else {
        // Enhanced escape screen
        clearScreen();
        std::cout << "\n";
        drawCombatTop();
        drawCombatEmpty();
        drawCombatTitle("~ ESCAPED ~", Color::BRIGHT_CYAN);
        drawCombatEmpty();
        drawCombatLine('=');
        drawCombatEmpty();
        drawCombatText("You managed to escape from combat!", Color::CYAN);
        drawCombatText("Live to fight another day...", Color::WHITE);
        drawCombatEmpty();
        drawCombatBottom();
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
        effective.attack = static_cast<int>(effective.attack * 0.8f);
        effective.defense = static_cast<int>(effective.defense * 0.8f);
    } else if (gameDifficulty == 3) {  // Hard
        effective.attack = static_cast<int>(effective.attack * 1.3f);
        effective.defense = static_cast<int>(effective.defense * 1.2f);
        effective.maxHP = static_cast<int>(effective.maxHP * 1.2f);
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
                // Regen Mana
                int manaRegen = 5;
                if (playerRef.stats.currentMana < playerRef.stats.maxMana) {
                    playerRef.stats.currentMana += manaRegen;
                    if (playerRef.stats.currentMana > playerRef.stats.maxMana) 
                        playerRef.stats.currentMana = playerRef.stats.maxMana;
                }
                
                // Reduce cooldowns
                for(auto& skill : playerRef.skills) {
                    if(skill.currentCooldown > 0) skill.currentCooldown--;
                }

                int action = getPlayerAction();
                
                switch (action) {
                    case 1: {  // Attack
                        int targetIdx = (enemies.size() == 1) ? 0 : selectTarget(enemies);
                        AttackResult result = playerAttack(enemies[targetIdx]);
                        
                        // Enhanced attack display
                        if (result.dodged) {
                            showDodgeEffect(enemies[targetIdx].name);
                        } else {
                            showAttackAnimation(playerRef.name, true, result.critical);
                            showDamageEffect(result.damageDealt, result.critical, false);
                        }
                        displayMessage(result.message);
                        
                        if (enemies[targetIdx].stats.currentHP <= 0) {
                            std::cout << Color::BRIGHT_YELLOW << "\n    [X] " << enemies[targetIdx].name << " " 
                                      << randomElement(DEATH_TEXT) << Color::RESET << "\n";
                        }
                        break;
                    }
                    case 2: {  // Skills
                        if (playerRef.skills.empty()) {
                            std::cout << Color::RED << "  You have no skills learned!" << Color::RESET << "\n";
                            // Ideally we'd loop back, but for now we just lose turn
                        } else {
                            std::cout << "\n";
                            std::cout << Color::BRIGHT_BLUE << "  > Available Skills:" << Color::RESET << "\n";
                            for (size_t i = 0; i < playerRef.skills.size(); ++i) {
                                const Skill& skill = playerRef.skills[i];
                                bool canUse = (playerRef.stats.currentMana >= skill.manaCost && skill.currentCooldown <= 0);
                                std::string color = canUse ? Color::BRIGHT_WHITE : Color::RED;
                                std::cout << "    " << color << (i + 1) << ". " << skill.name
                                          << " (MP: " << skill.manaCost << ")";
                                if (skill.currentCooldown > 0) std::cout << " [CD: " << skill.currentCooldown << "]";
                                std::cout << Color::RESET << "\n";
                            }
                            std::cout << Color::YELLOW << "    0. Cancel" << Color::RESET << "\n";
                            
                            int skillChoice = getValidatedInt("  Choose skill: ", 0, static_cast<int>(playerRef.skills.size()));
                            
                            if (skillChoice > 0) {
                                int targetIdx = 0;
                                if (!playerRef.skills[skillChoice-1].isHeal) {
                                    targetIdx = (enemies.size() == 1) ? 0 : selectTarget(enemies);
                                }
                                AbilityResult result = usePlayerAbility(skillChoice - 1, &enemies[targetIdx]);
                                
                                // Show skill animation effect
                                if (result.healingDone > 0) {
                                    showHealEffect(result.healingDone);
                                } else if (result.damageDealt > 0) {
                                    std::cout << Color::BRIGHT_BLUE << "    * SKILL ACTIVATED *" << Color::RESET << "\n";
                                    showDamageEffect(result.damageDealt, false, false);
                                }
                                displayMessage(result.message);
                            }
                        }
                        break;
                    }
                    case 3: {  // Use item
                        auto combatItems = getCombatItemIndices(playerRef);
                        if (combatItems.empty()) {
                            std::cout << Color::RED << "  No usable items!" << Color::RESET << "\n";
                        } else {
                            std::cout << "\n";
                            std::cout << Color::BRIGHT_GREEN << "  > Usable Items:" << Color::RESET << "\n";
                            for (size_t i = 0; i < combatItems.size(); ++i) {
                                const Item& item = playerRef.inventory[combatItems[i]];
                                std::cout << Color::WHITE << "    " << (i + 1) << ". " << item.name;
                                if (item.healAmount > 0) {
                                    std::cout << Color::GREEN << " (Heals " << item.healAmount << " HP)" << Color::RESET;
                                }
                                std::cout << Color::RESET << "\n";
                            }
                            std::cout << Color::YELLOW << "    0. Cancel" << Color::RESET << "\n";
                            
                            int choice = getValidatedInt("  Choose item: ", 0, 
                                static_cast<int>(combatItems.size()));
                            if (choice > 0) {
                                useItemInCombat(combatItems[choice - 1]);
                                showHealEffect(playerRef.inventory[combatItems[choice - 1]].healAmount);
                            }
                        }
                        break;
                    }
                    case 4: {  // Flee
                        std::cout << Color::YELLOW << "\n    <- Attempting to flee..." << Color::RESET << "\n";
                        delay(500);
                        if (attemptFlee(enemies)) {
                            combatEnded = true;
                            return false;
                        }
                        break;
                    }
                }
            } else {
                // Enemy turn
                if (index < 0 || index >= static_cast<int>(enemies.size())) continue;
                Enemy& enemy = enemies[index];
                
                if (enemy.stats.currentHP <= 0) continue;  // Skip dead enemies

                if (hasStun(enemy.activeEffects)) {
                    std::cout << Color::CYAN << "\n    🌀 " << enemy.name << " is stunned and loses the turn!" << Color::RESET << "\n";
                    delay(500);
                    continue;
                }
                
                // Enhanced enemy turn header - constant width box
                const int ENEMY_BOX_WIDTH = 40;
                std::string turnText = toUpper(enemy.name) + "'S TURN";
                int textLen = static_cast<int>(turnText.length());
                int leftPad = (ENEMY_BOX_WIDTH - 4 - textLen) / 2;
                int rightPad = ENEMY_BOX_WIDTH - 4 - textLen - leftPad;
                
                std::cout << "\n";
                std::cout << Color::RED << "  ";
                for (int i = 0; i < ENEMY_BOX_WIDTH; ++i) std::cout << "=";
                std::cout << Color::RESET << "\n";
                
                std::cout << Color::RED << "  | " << Color::BRIGHT_RED;
                for (int i = 0; i < leftPad; ++i) std::cout << " ";
                std::cout << turnText;
                for (int i = 0; i < rightPad; ++i) std::cout << " ";
                std::cout << Color::RED << " |" << Color::RESET << "\n";
                
                std::cout << Color::RED << "  ";
                for (int i = 0; i < ENEMY_BOX_WIDTH; ++i) std::cout << "=";
                std::cout << Color::RESET << "\n";
                delay(300);
                
                // Check if enemy uses ability or intent dictates action
                bool acted = false;
                if (enemy.nextIntent == EnemyIntent::HEAL || shouldEnemyUseAbility(enemy)) {
                    AbilityResult result = useEnemyAbility(enemy);
                    if (result.healingDone > 0) {
                        std::cout << Color::GREEN << "    + " << enemy.name << " heals!" << Color::RESET << "\n";
                        showHealEffect(result.healingDone);
                    }
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
                        std::cout << Color::CYAN << "    🛡 " << enemy.name << " braces for impact, raising a guard!" << Color::RESET << "\n";
                        delay(500);
                    } else {
                        // Normal attack (heavy charge simply benefits from higher damage roll)
                        AttackResult result = enemyAttack(enemy);
                        
                        // Enhanced enemy attack display
                        if (result.dodged) {
                            showDodgeEffect(playerRef.name);
                        } else {
                            showAttackAnimation(enemy.name, false, result.critical);
                            showDamageEffect(result.damageDealt, result.critical, true);
                        }
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
    
    // Enhanced combat status display
    drawCombatTop();
    drawCombatTitle("COMBAT STATUS", Color::BRIGHT_WHITE);
    drawCombatLine('=');
    
    // Player section header
    drawCombatEmpty();
    std::stringstream playerHeader;
    playerHeader << playerRef.name << " (" << classToString(playerRef.charClass) << ")";
    drawCombatText(playerHeader.str(), Color::BRIGHT_GREEN);
    drawCombatEmpty();
    
    // Player HP Bar with visual - using drawCombatText for consistent formatting
    std::stringstream hpLine;
    hpLine << "  HP: " << drawEnhancedHealthBar(playerRef.stats.currentHP, playerRef.stats.maxHP, 25, true)
           << " " << playerRef.stats.currentHP << "/" << playerRef.stats.maxHP;
    drawCombatText(hpLine.str(), Color::WHITE);
    
    // Player MP Bar with visual
    std::stringstream mpLine;
    mpLine << "  MP: " << drawManaBar(playerRef.stats.currentMana, playerRef.stats.maxMana, 25)
           << " " << playerRef.stats.currentMana << "/" << playerRef.stats.maxMana;
    drawCombatText(mpLine.str(), Color::WHITE);
    
    // Player stats line - using ASCII-compatible symbols
    CombatStats pStats = getEffectivePlayerStats();
    std::stringstream statsLine;
    statsLine << "  ATK:" << pStats.attack << " DEF:" << pStats.defense
              << " CRIT:" << pStats.critChance << "% SPD:" << pStats.speed;
    drawCombatText(statsLine.str(), Color::WHITE);
    
    // Show active effects with better formatting
    if (!playerRef.activeEffects.empty()) {
        std::stringstream effectsLine;
        effectsLine << "  Effects: ";
        for (const StatusEffect& e : playerRef.activeEffects) {
            effectsLine << "[" << e.name << ":" << e.turnsRemaining << "] ";
        }
        drawCombatText(effectsLine.str(), Color::YELLOW);
    }
    
    drawCombatLine('=');
    
    // Enemy section header
    drawCombatText("ENEMIES", Color::BRIGHT_RED);
    drawCombatEmpty();
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        const Enemy& enemy = enemies[i];
        
        // Enemy name
        std::stringstream enemyName;
        enemyName << "  " << (i + 1) << ". ";
        if (enemy.isBoss) {
            enemyName << "* " << enemy.name << " * [BOSS]";
            drawCombatText(enemyName.str(), Color::BRIGHT_MAGENTA);
        } else {
            enemyName << enemy.name;
            drawCombatText(enemyName.str(), Color::BRIGHT_RED);
        }
        
        // Enemy HP bar - using consistent drawCombatText
        std::stringstream enemyHP;
        enemyHP << "     HP: " << drawEnhancedHealthBar(enemy.stats.currentHP, enemy.stats.maxHP, 20, false)
                << " " << enemy.stats.currentHP << "/" << enemy.stats.maxHP;
        drawCombatText(enemyHP.str(), Color::WHITE);
        
        // Enemy intent with icon and color
        std::stringstream intentLine;
        intentLine << "     Intent: " << getIntentIcon(enemy.nextIntent) << " " << intentToString(enemy.nextIntent);
        drawCombatText(intentLine.str(), getIntentColor(enemy.nextIntent));
        
        // Enemy effects
        if (!enemy.activeEffects.empty()) {
            std::stringstream effectsLine;
            effectsLine << "     Effects: ";
            for (const StatusEffect& e : enemy.activeEffects) {
                effectsLine << "[" << e.name << ":" << e.turnsRemaining << "] ";
            }
            drawCombatText(effectsLine.str(), Color::YELLOW);
        }
        
        if (i < enemies.size() - 1) {
            drawCombatEmpty();
        }
    }
    
    drawCombatEmpty();
    drawCombatBottom();
}

int CombatSystem::getPlayerAction() const {
    // Enhanced action menu with icons
    std::cout << "\n";
    std::cout << Color::CYAN << THIN_TOP_LEFT;
    for (int i = 0; i < 40; ++i) std::cout << THIN_HORIZONTAL;
    std::cout << THIN_TOP_RIGHT << Color::RESET << "\n";
    
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET;
    std::cout << Color::BRIGHT_WHITE << "      ➤ YOUR TURN - Choose Action      " << Color::RESET;
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET << "\n";
    
    std::cout << Color::CYAN << THIN_VERTICAL;
    for (int i = 0; i < 40; ++i) std::cout << THIN_HORIZONTAL;
    std::cout << THIN_VERTICAL << Color::RESET << "\n";
    
    // Action options with icons
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET;
    std::cout << Color::BRIGHT_RED << "  1. [ATK] Attack              " << Color::RESET << "        ";
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET << "\n";
    
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET;
    std::cout << Color::BRIGHT_BLUE << "  2. [SKL] Skills              " << Color::RESET << "        ";
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET << "\n";
    
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET;
    std::cout << Color::BRIGHT_GREEN << "  3. [ITM] Use Item            " << Color::RESET << "        ";
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET << "\n";
    
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET;
    std::cout << Color::BRIGHT_YELLOW << "  4. [RUN] Attempt to Flee     " << Color::RESET << "        ";
    std::cout << Color::CYAN << THIN_VERTICAL << Color::RESET << "\n";
    
    std::cout << Color::CYAN << THIN_BOTTOM_LEFT;
    for (int i = 0; i < 40; ++i) std::cout << THIN_HORIZONTAL;
    std::cout << THIN_BOTTOM_RIGHT << Color::RESET << "\n";
    
    return getValidatedInt("\n  Enter choice (1-4): ", 1, 4);
}

int CombatSystem::selectTarget(const std::vector<Enemy>& enemies) const {
    std::cout << "\n";
    std::cout << Color::YELLOW << "  > Select Target:" << Color::RESET << "\n";
    for (size_t i = 0; i < enemies.size(); ++i) {
        std::cout << Color::RED << "    " << (i + 1) << ". " << enemies[i].name << Color::RESET
                  << Color::WHITE << " (HP: " << enemies[i].stats.currentHP << "/"
                  << enemies[i].stats.maxHP << ")" << Color::RESET << "\n";
    }
    
    return getValidatedInt("  Target: ", 1, static_cast<int>(enemies.size())) - 1;
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

        // Healing intent if low
        if (enemy.stats.currentHP < enemy.stats.maxHP / 3) {
            bool hasHeal = std::any_of(enemy.abilities.begin(), enemy.abilities.end(),
                [](const EnemyAbility& a) { return a.heals; });
            if (hasHeal) {
                enemy.nextIntent = EnemyIntent::HEAL;
                enemy.intentStrength = 1;
                continue;
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
                enemy.nextIntent = EnemyIntent::HEAL;
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
    
    // Calculate damage
    int baseDamage = pStats.attack + randomInt(-2, 2);
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
    
    // Calculate damage
    int baseDamage = eStats.attack + randomInt(-2, 2);
    result.rawDamage = baseDamage;
    result.damageDealt = calculateFinalDamage(baseDamage, pStats.defense, result.critical);
    
    // Apply difficulty modifier
    result.damageDealt = applyDifficultyModifier(result.damageDealt, gameDifficulty, false);
    
    int vuln = computeVulnerablePercent(playerRef.activeEffects);
    if (vuln > 0) {
        result.damageDealt += (result.damageDealt * vuln) / 100;
    }

    result.damageDealt = applyShieldThenDamage(result.damageDealt, playerRef.activeEffects, playerRef.stats.currentHP);
    
    // Generate message
    result.message = generateCombatMessage(attacker.name, playerRef.name,
                                           result.damageDealt, result.critical, false);
    log(result.message);
    
    return result;
}

bool CombatSystem::rollToHit(int attackerSpeed, int targetDodge) const {
    // Base hit chance is 85% + speed bonus - dodge
    int hitChance = 85 + attackerSpeed - targetDodge;
    if (hitChance < 20) hitChance = 20;   // Minimum 20% hit chance
    if (hitChance > 95) hitChance = 95;   // Maximum 95% hit chance
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

    // Diminishing returns on defense for stability
    const float k = 40.0f; // tuning constant
    float mitigation = static_cast<float>(defense) / (static_cast<float>(defense) + k);
    if (mitigation < 0.0f) mitigation = 0.0f;
    if (mitigation > 0.8f) mitigation = 0.8f; // keep some damage always going through

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
        std::cout << Color::BRIGHT_GREEN << "    ✓ You escaped successfully!" << Color::RESET << "\n";
        log("Player fled successfully.");
        delay(500);
        return true;
    } else {
        std::cout << Color::BRIGHT_RED << "    ✗ You failed to escape!" << Color::RESET << "\n";
        delay(500);
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
        std::cout << Color::RED << "  You can't use that in combat!" << Color::RESET << "\n";
        return false;
    }
    
    // Apply item effects
    if (item.healAmount > 0) {
        int healed = item.healAmount;
        playerRef.stats.currentHP += healed;
        if (playerRef.stats.currentHP > playerRef.stats.maxHP) {
            playerRef.stats.currentHP = playerRef.stats.maxHP;
        }
        std::cout << Color::BRIGHT_GREEN << "  > You use " << item.name << " and restore " 
                  << healed << " HP!" << Color::RESET << "\n";
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
        
        std::cout << Color::CYAN << "  ✨ You gain a temporary buff!" << Color::RESET << "\n";
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
    
    // Use enemy's loot table if available
    for (size_t i = 0; i < enemy.lootTable.size() && i < enemy.lootChances.size(); ++i) {
        if (randomChance(enemy.lootChances[i])) {
            // Create a basic loot item (would normally look up from item templates)
            Item drop;
            drop.id = enemy.lootTable[i];
            drop.name = "Loot Item #" + std::to_string(drop.id);
            drop.quantity = 1;
            
            // Determine rarity based on enemy
            if (enemy.isBoss) {
                if (randomChance(LEGENDARY_DROP_RATE * 5)) {
                    drop.rarity = ItemRarity::LEGENDARY;
                } else if (randomChance(EPIC_DROP_RATE * 5)) {
                    drop.rarity = ItemRarity::EPIC;
                } else {
                    drop.rarity = ItemRarity::RARE;
                }
            } else {
                int roll = randomInt(1, 100);
                if (roll <= LEGENDARY_DROP_RATE) drop.rarity = ItemRarity::LEGENDARY;
                else if (roll <= LEGENDARY_DROP_RATE + EPIC_DROP_RATE) drop.rarity = ItemRarity::EPIC;
                else if (roll <= LEGENDARY_DROP_RATE + EPIC_DROP_RATE + RARE_DROP_RATE) 
                    drop.rarity = ItemRarity::RARE;
                else if (roll <= LEGENDARY_DROP_RATE + EPIC_DROP_RATE + RARE_DROP_RATE + UNCOMMON_DROP_RATE)
                    drop.rarity = ItemRarity::UNCOMMON;
                else drop.rarity = ItemRarity::COMMON;
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
