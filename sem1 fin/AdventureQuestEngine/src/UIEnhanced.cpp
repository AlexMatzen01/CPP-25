//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Enhanced UI system implementation (partially implemented - many features scrapped)

#include "../include/UIEnhanced.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include <thread>

namespace AQE {

// Static member definitions
std::map<std::string, Animation> UIManager::animations;
std::vector<LogEntry> UIManager::globalLog;

// ---
// UIManager Implementation
// ---

UIManager::UIManager() {
    initializeAnimations();
}

std::string UIManager::colorize(const std::string& text, UIColor color, UIEffect effect) {
    std::stringstream ss;
    ss << "\033[";
    if (static_cast<int>(effect) != 0) {
        ss << static_cast<int>(effect) << ";";
    }
    ss << static_cast<int>(color) << "m" << text << "\033[0m";
    return ss.str();
}

std::string UIManager::colorizeRGB(const std::string& text, int r, int g, int b) {
    std::stringstream ss;
    ss << "\033[38;2;" << r << ";" << g << ";" << b << "m" << text << "\033[0m";
    return ss.str();
}

std::string UIManager::addEffect(const std::string& text, UIEffect effect) {
    std::stringstream ss;
    ss << "\033[" << static_cast<int>(effect) << "m" << text << "\033[0m";
    return ss.str();
}

std::string UIManager::resetColor() {
    return "\033[0m";
}

std::string UIManager::renderHealthBar(const HealthBar& bar) {
    std::stringstream ss;
    ss << bar.prefix;
    
    if (bar.maximum <= 0) {
        return bar.prefix + "[ERROR: Invalid maximum]";  // Safeguard against division by zero
    }
    
    int filled = (bar.current * bar.width) / bar.maximum;
    if (filled > bar.width) filled = bar.width;
    if (filled < 0) filled = 0;
    
    float percentage = static_cast<float>(bar.current) / bar.maximum;
    UIColor color;
    if (percentage > 0.6f) {
        color = bar.fullColor;
    } else if (percentage > 0.3f) {
        color = bar.midColor;
    } else {
        color = bar.lowColor;
    }
    
    // Use Unicode block characters for gradient effect
    ss << "[";
    ss << "\033[" << static_cast<int>(color) << "m";
    for (int i = 0; i < bar.width; ++i) {
        if (i < filled) {
            ss << "█";  // Full block
        } else if (i == filled && (bar.current * bar.width) % bar.maximum > bar.maximum / 4) {
            ss << "▓";  // Partial block for smooth transition
        } else {
            ss << "░";  // Light shade for empty
        }
    }
    ss << "\033[0m]";
    
    if (bar.showNumbers) {
        ss << " " << colorize(std::to_string(bar.current), color) << "/" << bar.maximum;
    }
    
    if (bar.showPercentage) {
        ss << " (" << colorize(std::to_string(static_cast<int>(percentage * 100)) + "%", color) << ")";
    }
    
    return ss.str();
}

std::string UIManager::renderProgressBar(const ProgressBar& bar) {
    std::stringstream ss;
    
    if (!bar.label.empty()) {
        ss << bar.label << " ";
    }
    
    int filled = (bar.current * bar.width) / bar.maximum;
    if (filled > bar.width) filled = bar.width;
    if (filled < 0) filled = 0;
    
    ss << "[";
    ss << "\033[" << static_cast<int>(bar.barColor) << "m";
    for (int i = 0; i < bar.width; ++i) {
        if (i < filled) {
            ss << bar.fillChar;
        } else {
            ss << bar.emptyChar;
        }
    }
    ss << "\033[0m]";
    
    ss << " " << bar.current << "/" << bar.maximum;
    
    return ss.str();
}

std::string UIManager::renderManaBar(int current, int max, int width) {
    HealthBar bar;
    bar.current = current;
    bar.maximum = max;
    bar.width = width;
    bar.prefix = "💧 Mana: ";
    bar.fullColor = UIColor::BRIGHT_BLUE;
    bar.midColor = UIColor::BLUE;
    bar.lowColor = UIColor::BRIGHT_BLACK;
    bar.showNumbers = true;
    
    return renderHealthBar(bar);
}

std::string UIManager::renderXPBar(int current, int needed, int width) {
    ProgressBar bar;
    bar.current = current;
    bar.maximum = needed;
    bar.width = width;
    bar.barColor = UIColor::BRIGHT_YELLOW;
    bar.fillChar = '▓';
    bar.emptyChar = '░';
    bar.label = "✨";
    
    return renderProgressBar(bar);
}

void UIManager::displayPlayerStatus(const StatusDisplay& status) {
    int boxWidth = 70;
    
    // Top border
    std::cout << "\n" << colorize("╔", UIColor::BRIGHT_CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("═", UIColor::BRIGHT_CYAN);
    std::cout << colorize("╗", UIColor::BRIGHT_CYAN) << "\n";
    
    // Name and basic info
    std::cout << colorize("║", UIColor::BRIGHT_CYAN) << " ";
    std::cout << colorize(status.name, UIColor::BRIGHT_YELLOW, UIEffect::BOLD);
    std::cout << colorize(" ⚔ Level ", UIColor::WHITE);
    std::cout << colorize(std::to_string(status.level), UIColor::BRIGHT_GREEN, UIEffect::BOLD);
    std::cout << std::string(boxWidth - status.name.length() - 13, ' ');
    std::cout << colorize("║", UIColor::BRIGHT_CYAN) << "\n";
    
    // Divider
    std::cout << colorize("╟", UIColor::BRIGHT_CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("─", UIColor::BRIGHT_CYAN);
    std::cout << colorize("╢", UIColor::BRIGHT_CYAN) << "\n";
    
    // HP Bar
    HealthBar hpBar;
    hpBar.current = status.hp;
    hpBar.maximum = status.maxHP;
    hpBar.showNumbers = true;
    hpBar.prefix = "❤ HP: ";
    std::cout << colorize("║", UIColor::BRIGHT_CYAN) << " " << renderHealthBar(hpBar);
    std::cout << std::string(boxWidth - 38, ' ');
    std::cout << colorize("║", UIColor::BRIGHT_CYAN) << "\n";
    
    // Mana Bar
    std::cout << colorize("║", UIColor::BRIGHT_CYAN) << " " << renderManaBar(status.mana, status.maxMana);
    std::cout << std::string(boxWidth - 38, ' ');
    std::cout << colorize("║", UIColor::BRIGHT_CYAN) << "\n";
    
    // Gold
    std::cout << colorize("║", UIColor::BRIGHT_CYAN) << " ";
    std::cout << colorize("💰 Gold: ", UIColor::BRIGHT_YELLOW);
    std::cout << colorize(std::to_string(status.gold), UIColor::YELLOW, UIEffect::BOLD);
    std::cout << std::string(boxWidth - 20, ' ');
    std::cout << colorize("║", UIColor::BRIGHT_CYAN) << "\n";
    
    // Buffs/Debuffs if present
    if (!status.activeBuffs.empty() || !status.activeDebuffs.empty()) {
        std::cout << colorize("╟", UIColor::BRIGHT_CYAN);
        for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("─", UIColor::BRIGHT_CYAN);
        std::cout << colorize("╢", UIColor::BRIGHT_CYAN) << "\n";
        
        if (!status.activeBuffs.empty()) {
            std::cout << colorize("║", UIColor::BRIGHT_CYAN) << " ";
            std::cout << colorize("✓ Buffs: ", UIColor::BRIGHT_GREEN);
            for (const auto& buff : status.activeBuffs) {
                std::cout << colorize("[" + buff + "]", UIColor::GREEN) << " ";
            }
            std::cout << std::string(std::max(0, boxWidth - 20 - static_cast<int>(status.activeBuffs.size()) * 8), ' ');
            std::cout << colorize("║", UIColor::BRIGHT_CYAN) << "\n";
        }
        
        if (!status.activeDebuffs.empty()) {
            std::cout << colorize("║", UIColor::BRIGHT_CYAN) << " ";
            std::cout << colorize("✗ Debuffs: ", UIColor::BRIGHT_RED);
            for (const auto& debuff : status.activeDebuffs) {
                std::cout << colorize("[" + debuff + "]", UIColor::RED) << " ";
            }
            std::cout << std::string(std::max(0, boxWidth - 22 - static_cast<int>(status.activeDebuffs.size()) * 8), ' ');
            std::cout << colorize("║", UIColor::BRIGHT_CYAN) << "\n";
        }
    }
    
    // Location
    if (!status.location.empty()) {
        std::cout << colorize("╟", UIColor::BRIGHT_CYAN);
        for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("─", UIColor::BRIGHT_CYAN);
        std::cout << colorize("╢", UIColor::BRIGHT_CYAN) << "\n";
        std::cout << colorize("║", UIColor::BRIGHT_CYAN) << " ";
        std::cout << colorize("📍 Location: ", UIColor::CYAN);
        std::cout << colorize(status.location, UIColor::WHITE);
        std::cout << std::string(std::max(0, boxWidth - static_cast<int>(status.location.length()) - 16), ' ');
        std::cout << colorize("║", UIColor::BRIGHT_CYAN) << "\n";
    }
    
    // Bottom border
    std::cout << colorize("╚", UIColor::BRIGHT_CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("═", UIColor::BRIGHT_CYAN);
    std::cout << colorize("╝", UIColor::BRIGHT_CYAN) << "\n";
}

void UIManager::displayEnemyStatus(const std::string& name, int hp, int maxHP, 
                                    const std::vector<std::string>& effects) {
    std::cout << colorize(name, UIColor::BRIGHT_RED) << " ";
    
    HealthBar hpBar;
    hpBar.current = hp;
    hpBar.maximum = maxHP;
    hpBar.width = 15;
    hpBar.prefix = "";
    hpBar.showNumbers = true;
    std::cout << renderHealthBar(hpBar);
    
    if (!effects.empty()) {
        std::cout << " ";
        for (const auto& effect : effects) {
            std::cout << colorize("[" + effect + "]", UIColor::YELLOW) << " ";
        }
    }
    std::cout << "\n";
}

void UIManager::displayCompactStats(const Player& player) {
    std::cout << player.name << " Lv" << player.level;
    std::cout << " HP:" << player.stats.currentHP << "/" << player.stats.maxHP;
    std::cout << " MP:" << player.stats.currentMana << "/" << player.stats.maxMana;
    std::cout << " G:" << player.gold << "\n";
}

void UIManager::displayFullStats(const Player& player) {
    int boxWidth = 80;
    
    // Title
    std::cout << "\n";
    std::cout << createTitle("⚔ CHARACTER STATS ⚔", boxWidth, UIColor::BRIGHT_CYAN);
    std::cout << "\n";
    
    // Character Info Section
    std::cout << colorize("╔", UIColor::CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("═", UIColor::CYAN);
    std::cout << colorize("╗", UIColor::CYAN) << "\n";
    
    std::cout << colorize("║", UIColor::CYAN) << " ";
    std::cout << colorize("Name: ", UIColor::WHITE);
    std::cout << colorize(player.name, UIColor::BRIGHT_YELLOW, UIEffect::BOLD);
    std::cout << std::string(boxWidth - player.name.length() - 10, ' ');
    std::cout << colorize("║", UIColor::CYAN) << "\n";
    
    std::cout << colorize("║", UIColor::CYAN) << " ";
    std::cout << colorize("Level: ", UIColor::WHITE);
    std::cout << colorize(std::to_string(player.level), UIColor::BRIGHT_GREEN, UIEffect::BOLD);
    std::cout << colorize("  XP: ", UIColor::WHITE);
    std::cout << colorize(std::to_string(player.experience) + "/" + std::to_string(player.experienceToLevel), UIColor::YELLOW);
    std::cout << std::string(boxWidth - 35, ' ');
    std::cout << colorize("║", UIColor::CYAN) << "\n";
    
    // XP Bar
    std::cout << colorize("║", UIColor::CYAN) << " " << renderXPBar(player.experience, player.experienceToLevel, 70);
    std::cout << std::string(3, ' ') << colorize("║", UIColor::CYAN) << "\n";
    
    // Combat Stats Section
    std::cout << colorize("╠", UIColor::CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("═", UIColor::CYAN);
    std::cout << colorize("╣", UIColor::CYAN) << "\n";
    std::cout << colorize("║", UIColor::CYAN) << " ";
    std::cout << colorize("⚔ COMBAT STATS", UIColor::BRIGHT_RED, UIEffect::BOLD);
    std::cout << std::string(boxWidth - 20, ' ');
    std::cout << colorize("║", UIColor::CYAN) << "\n";
    std::cout << colorize("╠", UIColor::CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("─", UIColor::CYAN);
    std::cout << colorize("╣", UIColor::CYAN) << "\n";
    
    // HP/Mana bars
    HealthBar hpBar;
    hpBar.current = player.stats.currentHP;
    hpBar.maximum = player.stats.maxHP;
    hpBar.showNumbers = true;
    hpBar.prefix = "❤ HP:    ";
    std::cout << colorize("║", UIColor::CYAN) << " " << renderHealthBar(hpBar);
    std::cout << std::string(boxWidth - 42, ' ');
    std::cout << colorize("║", UIColor::CYAN) << "\n";
    
    std::cout << colorize("║", UIColor::CYAN) << " ";
    std::cout << renderManaBar(player.stats.currentMana, player.stats.maxMana, 20);
    std::cout << std::string(boxWidth - 38, ' ');
    std::cout << colorize("║", UIColor::CYAN) << "\n";
    
    // Stats in two columns
    std::cout << colorize("╟", UIColor::CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("─", UIColor::CYAN);
    std::cout << colorize("╢", UIColor::CYAN) << "\n";
    
    auto statLine = [&](const std::string& label1, int val1, const std::string& label2, int val2, const std::string& suffix = "") {
        std::cout << colorize("║", UIColor::CYAN) << " ";
        std::cout << colorize(label1 + ": ", UIColor::YELLOW);
        std::cout << colorize(std::to_string(val1) + suffix, UIColor::WHITE, UIEffect::BOLD);
        std::cout << std::string(20 - label1.length() - std::to_string(val1).length() - suffix.length(), ' ');
        std::cout << colorize("│", UIColor::BRIGHT_BLACK) << " ";
        std::cout << colorize(label2 + ": ", UIColor::YELLOW);
        std::cout << colorize(std::to_string(val2) + suffix, UIColor::WHITE, UIEffect::BOLD);
        std::cout << std::string(boxWidth - 48 - label2.length() - std::to_string(val2).length() - suffix.length(), ' ');
        std::cout << colorize("║", UIColor::CYAN) << "\n";
    };
    
    statLine("⚔ Attack", player.stats.attack, "🛡 Defense", player.stats.defense);
    statLine("💥 Crit Chance", player.stats.critChance, "🌀 Dodge", player.stats.dodgeChance, "%");
    statLine("⚡ Speed", player.stats.speed, "✨ Magic Power", player.stats.magicPower);
    
    std::cout << colorize("║", UIColor::CYAN) << " ";
    std::cout << colorize("🔮 Magic Resist: ", UIColor::YELLOW);
    std::cout << colorize(std::to_string(player.stats.magicResist), UIColor::WHITE, UIEffect::BOLD);
    std::cout << std::string(boxWidth - 25, ' ');
    std::cout << colorize("║", UIColor::CYAN) << "\n";
    
    // Progression Section
    std::cout << colorize("╠", UIColor::CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("═", UIColor::CYAN);
    std::cout << colorize("╣", UIColor::CYAN) << "\n";
    std::cout << colorize("║", UIColor::CYAN) << " ";
    std::cout << colorize("📊 PROGRESSION", UIColor::BRIGHT_GREEN, UIEffect::BOLD);
    std::cout << std::string(boxWidth - 20, ' ');
    std::cout << colorize("║", UIColor::CYAN) << "\n";
    std::cout << colorize("╟", UIColor::CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("─", UIColor::CYAN);
    std::cout << colorize("╢", UIColor::CYAN) << "\n";
    
    auto progLine = [&](const std::string& icon, const std::string& label, int value) {
        std::cout << colorize("║", UIColor::CYAN) << " ";
        std::cout << colorize(icon + " " + label + ": ", UIColor::GREEN);
        std::cout << colorize(std::to_string(value), UIColor::BRIGHT_WHITE, UIEffect::BOLD);
        std::cout << std::string(boxWidth - icon.length() - label.length() - std::to_string(value).length() - 7, ' ');
        std::cout << colorize("║", UIColor::CYAN) << "\n";
    };
    
    progLine("💰", "Gold", player.gold);
    progLine("🚪", "Rooms Visited", static_cast<int>(player.visitedRooms.size()));
    progLine("⚔", "Enemies Killed", player.totalKills);
    progLine("🔍", "Secrets Found", player.secretsFound);
    progLine("🧩", "Puzzles Solved", player.puzzlesSolved);
    
    // Bottom border
    std::cout << colorize("╚", UIColor::CYAN);
    for (int i = 0; i < boxWidth - 2; ++i) std::cout << colorize("═", UIColor::CYAN);
    std::cout << colorize("╝", UIColor::CYAN) << "\n";
}

std::string UIManager::createBox(const std::string& content, int width, const std::string& style) {
    std::stringstream ss;
    
    std::string topLeft, topRight, bottomLeft, bottomRight, horizontal, vertical;
    
    if (style == "double") {
        topLeft = "╔"; topRight = "╗";
        bottomLeft = "╚"; bottomRight = "╝";
        horizontal = "═"; vertical = "║";
    } else if (style == "thick") {
        topLeft = "┏"; topRight = "┓";
        bottomLeft = "┗"; bottomRight = "┛";
        horizontal = "━"; vertical = "┃";
    } else if (style == "round") {
        topLeft = "╭"; topRight = "╮";
        bottomLeft = "╰"; bottomRight = "╯";
        horizontal = "─"; vertical = "│";
    } else {  // single
        topLeft = "┌"; topRight = "┐";
        bottomLeft = "└"; bottomRight = "┘";
        horizontal = "─"; vertical = "│";
    }
    
    ss << topLeft;
    for (int i = 0; i < width - 2; ++i) ss << horizontal;
    ss << topRight << "\n";
    
    ss << vertical << " " << std::setw(width - 4) << std::left << content << " " << vertical << "\n";
    
    ss << bottomLeft;
    for (int i = 0; i < width - 2; ++i) ss << horizontal;
    ss << bottomRight << "\n";
    
    return ss.str();
}

std::string UIManager::createTitle(const std::string& text, int width, UIColor color) {
    std::stringstream ss;
    int padding = (width - static_cast<int>(text.length()) - 2) / 2;
    
    // Top border with double line
    ss << colorize("╔", color);
    for (int i = 0; i < width - 2; ++i) ss << colorize("═", color);
    ss << colorize("╗", color) << "\n";
    
    // Title text
    ss << colorize("║", color);
    for (int i = 0; i < padding; ++i) ss << " ";
    ss << colorize(text, color, UIEffect::BOLD);
    for (int i = 0; i < width - padding - static_cast<int>(text.length()) - 2; ++i) ss << " ";
    ss << colorize("║", color) << "\n";
    
    // Bottom border
    ss << colorize("╚", color);
    for (int i = 0; i < width - 2; ++i) ss << colorize("═", color);
    ss << colorize("╝", color) << "\n";
    
    return ss.str();
}

void UIManager::drawBorder(int width, const std::string& style) {
    char ch = (style == "double") ? '=' : '-';
    for (int i = 0; i < width; ++i) {
        std::cout << ch;
    }
    std::cout << "\n";
}

void UIManager::drawDivider(int width, char character) {
    for (int i = 0; i < width; ++i) {
        std::cout << character;
    }
    std::cout << "\n";
}

int UIManager::displayMenu(Menu& menu) {
    renderMenu(menu);
    
    std::cout << "\nChoice: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice > 0 && choice <= static_cast<int>(menu.items.size())) {
        menu.selectedIndex = choice - 1;
    }
    
    return choice;
}

void UIManager::renderMenu(const Menu& menu) {
    int menuWidth = 60;
    
    // Title with box
    std::cout << "\n";
    std::cout << createTitle(menu.title, menuWidth, menu.titleColor);
    std::cout << "\n";
    
    int index = 1;
    for (const auto& item : menu.items) {
        if (item.isSeparator) {
            std::cout << colorize("  ├", menu.borderColor);
            for (int i = 0; i < menuWidth - 4; ++i) std::cout << colorize("─", menu.borderColor);
            std::cout << colorize("┤", menu.borderColor) << "\n";
            continue;
        }
        
        std::cout << "  ";
        
        // Icon
        if (!item.icon.empty()) {
            std::cout << colorize(item.icon, UIColor::BRIGHT_YELLOW) << " ";
        } else {
            std::cout << "  ";
        }
        
        // Number with special formatting
        std::cout << colorize("[", UIColor::BRIGHT_BLACK);
        std::cout << colorize(std::to_string(index), menu.selectedColor);
        std::cout << colorize("]", UIColor::BRIGHT_BLACK) << " ";
        
        // Item text
        if (item.isEnabled) {
            if (index - 1 == menu.selectedIndex) {
                std::cout << colorize("▶ " + item.text, menu.selectedColor, UIEffect::BOLD);
            } else {
                std::cout << colorize(item.text, item.textColor);
            }
        } else {
            std::cout << colorize(item.text, UIColor::BRIGHT_BLACK);
            std::cout << colorize(" (disabled)", UIColor::BRIGHT_BLACK);
        }
        
        std::cout << "\n";
        
        // Description if available and selected
        if (!item.description.empty() && index - 1 == menu.selectedIndex) {
            std::cout << "    " << colorize("└─ ", UIColor::BRIGHT_BLACK);
            std::cout << colorize(item.description, UIColor::CYAN, UIEffect::ITALIC) << "\n";
        }
        
        index++;
    }
    
    std::cout << "\n";
    drawDivider(menuWidth, '─');
}

int UIManager::selectFromList(const std::vector<std::string>& items, const std::string& title) {
    std::cout << "\n=== " << title << " ===\n\n";
    
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << items[i] << "\n";
    }
    
    std::cout << "\nChoice: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice < 1 || choice > static_cast<int>(items.size())) {
        return -1;
    }
    
    return choice - 1;
}

void UIManager::playAttackAnimation(DamageElement element) {
    std::string effect;
    
    switch (element) {
        case DamageElement::FIRE:
            effect = colorize("*** FIREBALL ***", UIColor::BRIGHT_RED);
            break;
        case DamageElement::FROST:
            effect = colorize("*** ICE SHARD ***", UIColor::BRIGHT_CYAN);
            break;
        case DamageElement::LIGHTNING:
            effect = colorize("*** LIGHTNING ***", UIColor::BRIGHT_YELLOW);
            break;
        case DamageElement::ARCANE:
            effect = colorize("*** ARCANE ***", UIColor::BRIGHT_MAGENTA);
            break;
        case DamageElement::HOLY:
            effect = colorize("*** HOLY LIGHT ***", UIColor::BRIGHT_WHITE);
            break;
        case DamageElement::SHADOW:
            effect = colorize("*** SHADOW ***", UIColor::MAGENTA);
            break;
        default:
            effect = colorize("*** ATTACK ***", UIColor::WHITE);
            break;
    }
    
    std::cout << "\n" << effect << "\n";
}

void UIManager::showDamageNumber(int damage, bool isCrit) {
    if (isCrit) {
        std::cout << colorize("CRITICAL! ", UIColor::BRIGHT_YELLOW);
        std::cout << colorize("-" + std::to_string(damage), UIColor::BRIGHT_RED);
    } else {
        std::cout << colorize("-" + std::to_string(damage), UIColor::RED);
    }
    std::cout << "\n";
}

void UIManager::showHealingNumber(int healing) {
    std::cout << colorize("+" + std::to_string(healing) + " HP", UIColor::BRIGHT_GREEN) << "\n";
}

void UIManager::displayCombatLog(const std::vector<LogEntry>& combatLog, int maxLines) {
    std::cout << "\n--- Combat Log ---\n";
    
    int start = std::max(0, static_cast<int>(combatLog.size()) - maxLines);
    for (size_t i = start; i < combatLog.size(); ++i) {
        const LogEntry& entry = combatLog[i];
        std::cout << colorize(entry.message, entry.color) << "\n";
    }
}

void UIManager::displayASCIIArt(const std::string& artName) {
    auto it = animations.find(artName);
    if (it != animations.end() && !it->second.frames.empty()) {
        for (const auto& line : it->second.frames[0].lines) {
            std::cout << line << "\n";
        }
    }
}

void UIManager::playAnimation(const Animation& anim) {
    for (const auto& frame : anim.frames) {
        clearScreen();
        for (const auto& line : frame.lines) {
            std::cout << line << "\n";
        }
        sleep(frame.delayMs);
    }
}

std::string UIManager::getElementIcon(DamageElement element) {
    switch (element) {
        case DamageElement::FIRE: return "[F]";
        case DamageElement::FROST: return "[I]";
        case DamageElement::LIGHTNING: return "[L]";
        case DamageElement::ARCANE: return "[A]";
        case DamageElement::HOLY: return "[H]";
        case DamageElement::SHADOW: return "[S]";
        case DamageElement::PHYSICAL: return "[P]";
        case DamageElement::NATURE: return "[N]";
        default: return "[?]";
    }
}

std::string UIManager::getClassIcon(CharacterClass charClass) {
    switch (charClass) {
        case CharacterClass::WARRIOR: return "[W]";
        case CharacterClass::MAGE: return "[M]";
        case CharacterClass::ROGUE: return "[R]";
        case CharacterClass::PALADIN: return "[P]";
        default: return "[?]";
    }
}

std::string UIManager::getRarityColor(ItemRarity rarity) {
    switch (rarity) {
        case ItemRarity::COMMON: return "\033[37m";
        case ItemRarity::UNCOMMON: return "\033[32m";
        case ItemRarity::RARE: return "\033[34m";
        case ItemRarity::EPIC: return "\033[35m";
        case ItemRarity::LEGENDARY: return "\033[33m";
        default: return "\033[0m";
    }
}

void UIManager::showEffect(EffectType effect, int x, int y) {
    (void)x;
    (void)y;
    
    switch (effect) {
        case EffectType::SLASH:
            std::cout << "  /\n / \n/\n";
            break;
        case EffectType::FIREBALL:
            std::cout << " ((*))\n";
            break;
        case EffectType::LIGHTNING:
            std::cout << "  /\n /\n/\n";
            break;
        case EffectType::HEAL:
            std::cout << " +++ \n";
            break;
        case EffectType::EXPLOSION:
            std::cout << " *BOOM* \n";
            break;
        default:
            break;
    }
}

void UIManager::screenShake(int intensity) {
    for (int i = 0; i < intensity; ++i) {
        std::cout << "\r  ";
        std::cout.flush();
        sleep(50);
        std::cout << "\r";
        std::cout.flush();
        sleep(50);
    }
}

void UIManager::fadeTransition(int durationMs) {
    int steps = durationMs / 100;
    for (int i = 0; i < steps; ++i) {
        std::cout << ".";
        std::cout.flush();
        sleep(100);
    }
    std::cout << "\n";
}

void UIManager::flashScreen(UIColor color, int times) {
    for (int i = 0; i < times; ++i) {
        std::cout << "\033[" << static_cast<int>(color) << "m";
        std::cout << "!!! FLASH !!!\033[0m\n";
        sleep(100);
    }
}

void UIManager::displayTwoColumn(const std::string& left, const std::string& right, int width) {
    int halfWidth = width / 2;
    std::cout << std::setw(halfWidth) << std::left << left;
    std::cout << std::setw(halfWidth) << std::right << right << "\n";
}

void UIManager::displayThreeColumn(const std::string& left, const std::string& mid, 
                                    const std::string& right, int width) {
    int thirdWidth = width / 3;
    std::cout << std::setw(thirdWidth) << std::left << left;
    std::cout << std::setw(thirdWidth) << std::left << mid;
    std::cout << std::setw(thirdWidth) << std::right << right << "\n";
}

void UIManager::displayPanel(const std::string& title, const std::string& content, 
                              int width, UIColor borderColor) {
    std::cout << "\033[" << static_cast<int>(borderColor) << "m";
    drawBorder(width);
    std::cout << centerText(title, width) << "\n";
    drawBorder(width);
    std::cout << "\033[0m";
    
    std::cout << content << "\n";
    
    std::cout << "\033[" << static_cast<int>(borderColor) << "m";
    drawBorder(width);
    std::cout << "\033[0m";
}

void UIManager::log(const std::string& message, LogLevel level) {
    LogEntry entry;
    entry.message = message;
    entry.level = level;
    
    switch (level) {
        case LogLevel::SUCCESS: entry.color = UIColor::GREEN; break;
        case LogLevel::WARNING: entry.color = UIColor::YELLOW; break;
        case LogLevel::ERROR: entry.color = UIColor::RED; break;
        case LogLevel::COMBAT_HIT: entry.color = UIColor::BRIGHT_RED; break;
        case LogLevel::COMBAT_MISS: entry.color = UIColor::BRIGHT_BLACK; break;
        case LogLevel::COMBAT_CRIT: entry.color = UIColor::BRIGHT_YELLOW; break;
        case LogLevel::HEAL: entry.color = UIColor::BRIGHT_GREEN; break;
        case LogLevel::BUFF: entry.color = UIColor::CYAN; break;
        case LogLevel::DEBUFF: entry.color = UIColor::MAGENTA; break;
        case LogLevel::LOOT: entry.color = UIColor::BRIGHT_CYAN; break;
        case LogLevel::QUEST: entry.color = UIColor::BRIGHT_MAGENTA; break;
        default: entry.color = UIColor::WHITE; break;
    }
    
    globalLog.push_back(entry);
    
    if (globalLog.size() > 100) {
        globalLog.erase(globalLog.begin());
    }
}

void UIManager::logCombat(const std::string& message, bool isCrit, bool isMiss) {
    if (isMiss) {
        log(message, LogLevel::COMBAT_MISS);
    } else if (isCrit) {
        log(message, LogLevel::COMBAT_CRIT);
    } else {
        log(message, LogLevel::COMBAT_HIT);
    }
}

void UIManager::logLoot(const std::string& itemName, ItemRarity rarity) {
    std::string prefix = "Found: ";
    std::string color = getRarityColor(rarity);
    log(prefix + color + itemName + "\033[0m", LogLevel::LOOT);
}

void UIManager::logQuest(const std::string& message, bool isComplete) {
    if (isComplete) {
        log("QUEST COMPLETE: " + message, LogLevel::QUEST);
    } else {
        log("Quest: " + message, LogLevel::QUEST);
    }
}

void UIManager::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

void UIManager::moveCursor(int x, int y) {
    std::cout << "\033[" << y << ";" << x << "H";
}

void UIManager::hideCursor() {
    std::cout << "\033[?25l";
}

void UIManager::showCursor() {
    std::cout << "\033[?25h";
}

void UIManager::waitForKey() {
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void UIManager::sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

std::string UIManager::centerText(const std::string& text, int width) {
    int padding = (width - static_cast<int>(text.length())) / 2;
    if (padding < 0) padding = 0;
    
    std::stringstream ss;
    for (int i = 0; i < padding; ++i) ss << " ";
    ss << text;
    return ss.str();
}

std::string UIManager::padRight(const std::string& text, int width) {
    std::stringstream ss;
    ss << std::setw(width) << std::left << text;
    return ss.str();
}

std::string UIManager::padLeft(const std::string& text, int width) {
    std::stringstream ss;
    ss << std::setw(width) << std::right << text;
    return ss.str();
}

void UIManager::particleBurst(int x, int y, UIColor color, char particle) {
    (void)x;
    (void)y;
    
    std::cout << "\033[" << static_cast<int>(color) << "m";
    std::cout << "  " << particle << particle << particle << "\n";
    std::cout << " " << particle << " " << particle << " " << particle << "\n";
    std::cout << "  " << particle << particle << particle << "\n";
    std::cout << "\033[0m";
}

void UIManager::floatingText(const std::string& text, int x, int y, UIColor color) {
    moveCursor(x, y);
    std::cout << "\033[" << static_cast<int>(color) << "m" << text << "\033[0m";
}

void UIManager::typewriterEffect(const std::string& text, int delayMs) {
    for (char c : text) {
        std::cout << c;
        std::cout.flush();
        sleep(delayMs);
    }
    std::cout << "\n";
}

void UIManager::displayItemComparison(const Item& item1, const Item& item2) {
    std::cout << "\n=== Item Comparison ===\n\n";
    
    std::cout << std::setw(20) << std::left << "Stat";
    std::cout << std::setw(15) << item1.name;
    std::cout << std::setw(15) << item2.name << "\n";
    
    drawDivider(50, '-');
    
    std::cout << std::setw(20) << "Value:";
    std::cout << std::setw(15) << item1.value;
    std::cout << std::setw(15) << item2.value << "\n";
}

void UIManager::displaySkillTree(const std::vector<std::string>& nodes) {
    std::cout << "\n=== Skill Tree ===\n\n";
    
    for (size_t i = 0; i < nodes.size(); ++i) {
        std::cout << "  ";
        if (i > 0) std::cout << "|-- ";
        std::cout << nodes[i] << "\n";
    }
}

void UIManager::displayMap(const std::vector<std::vector<char>>& map, int playerX, int playerY) {
    std::cout << "\n=== Map ===\n\n";
    
    for (size_t y = 0; y < map.size(); ++y) {
        std::cout << "  ";
        for (size_t x = 0; x < map[y].size(); ++x) {
            if (static_cast<int>(x) == playerX && static_cast<int>(y) == playerY) {
                std::cout << colorize("@", UIColor::BRIGHT_GREEN);
            } else {
                std::cout << map[y][x];
            }
        }
        std::cout << "\n";
    }
}

void UIManager::displayInventoryGrid(const std::vector<Item>& items, int columns) {
    std::cout << "\n=== Inventory ===\n\n";
    
    int col = 0;
    for (const auto& item : items) {
        std::cout << std::setw(20) << std::left << item.name;
        col++;
        if (col >= columns) {
            std::cout << "\n";
            col = 0;
        }
    }
    
    if (col != 0) std::cout << "\n";
}

void UIManager::initializeAnimations() {
    Animation slash;
    slash.name = "slash";
    ASCIIFrame frame1;
    frame1.lines = {"  /", " / ", "/"};
    frame1.delayMs = 100;
    slash.frames.push_back(frame1);
    animations["slash"] = slash;
    
    Animation fireball;
    fireball.name = "fireball";
    ASCIIFrame fireFrame;
    fireFrame.lines = {"  *  ", " *** ", "*****", " *** ", "  *  "};
    fireFrame.delayMs = 200;
    fireball.frames.push_back(fireFrame);
    animations["fireball"] = fireball;
}

std::string UIManager::getBoxChar(const std::string& style, const std::string& position) {
    if (style == "double") {
        if (position == "tl") return "+";
        if (position == "tr") return "+";
        if (position == "bl") return "+";
        if (position == "br") return "+";
        if (position == "h") return "=";
        if (position == "v") return "|";
    }
    return "+";
}

// ---
// ASCIIArtLibrary Implementation
// ---

std::string ASCIIArtLibrary::getWarriorEmblem() {
    return "    /\\\n   /  \\\n  |    |\n  | () |\n   \\  /\n    \\/\n";
}

std::string ASCIIArtLibrary::getMageEmblem() {
    return "    *\n   /|\\\n  / | \\\n /  |  \\\n    |\n    ^\n";
}

std::string ASCIIArtLibrary::getRogueEmblem() {
    return "   __\n  |  |\n  |  |\n  |  |\n   \\/\n";
}

std::string ASCIIArtLibrary::getPaladinEmblem() {
    return "  +---+\n  |   |\n  | + |\n  |   |\n  +---+\n";
}

std::string ASCIIArtLibrary::getRangerEmblem() {
    return "     (\n      \\\n       )\n  ##-->>\n       )\n      /\n     (\n";
}

std::string ASCIIArtLibrary::getNecromancerEmblem() {
    return "    ___\n   /   \\\n  | X X |\n   \\_^_/\n    | |\n";
}

std::string ASCIIArtLibrary::getBarbarianEmblem() {
    return "   _  _\n  | \\/ |\n  |    |\n  |____|\n    ||\n    ||\n";
}

std::string ASCIIArtLibrary::getMonkEmblem() {
    return "    _ \n   ( )\n  _|_|_\n |     |\n |_____|\n";
}

std::string ASCIIArtLibrary::getSwordSlash() {
    return "  /\n / \n/  ";
}

std::string ASCIIArtLibrary::getFireball() {
    return " ((*)) ";
}

std::string ASCIIArtLibrary::getLightningBolt() {
    return " /\n/\n \\\n  \\";
}

std::string ASCIIArtLibrary::getShieldBlock() {
    return "[###]";
}

std::string ASCIIArtLibrary::getPoisonCloud() {
    return "~ ~ ~\n ~   ~\n~ ~ ~";
}

std::string ASCIIArtLibrary::getIceShard() {
    return "  /\\\n /  \\\n/    \\";
}

std::string ASCIIArtLibrary::getGoblinArt() {
    return "   /\\_/\\\n  ( o.o )\n   > ^ <\n";
}

std::string ASCIIArtLibrary::getOrcArt() {
    return "  /\\_/\\\n (=o=o=)\n  \\   /\n   \\_/\n";
}

std::string ASCIIArtLibrary::getDragonArt() {
    return "      /\\  /\\\n     /  \\/  \\\n    /   ()   \\\n   /   ^^^^   \\\n";
}

std::string ASCIIArtLibrary::getUndeadArt() {
    return "   ___\n  (x_x)\n  /| |\\\n  || ||\n";
}

std::string ASCIIArtLibrary::getBossArt() {
    return "    /\\  ___  /\\\n   /  \\/   \\/  \\\n  |    \\   /    |\n   \\   |   |   /\n    \\__|   |__/\n";
}

std::string ASCIIArtLibrary::getTreasureChest() {
    return "   _____\n  /     \\\n |  $$$ |\n |_______|\n";
}

std::string ASCIIArtLibrary::getDoorway() {
    return "  _____\n |     |\n |     |\n |_____|\n";
}

std::string ASCIIArtLibrary::getCampfire() {
    return "   \\|/\n   (())\n  (( ))\n  \\\\||//\n";
}

std::string ASCIIArtLibrary::getShrine() {
    return "    /\\\n   /  \\\n  | <> |\n  |____|\n";
}

std::string ASCIIArtLibrary::getLibrary() {
    return "   ______\n  |      |\n  | [][][] |\n  |______|\n";
}

std::string ASCIIArtLibrary::getForge() {
    return "   ______\n  |  __  |\n  | |  | |\n  |_|__|_|\n   /|  |\\\n";
}

std::string ASCIIArtLibrary::getLaboratory() {
    return "    _ \n   | |\n  /   \\\n | [V] |\n  \\___/\n";
}

std::string ASCIIArtLibrary::getGraveyard() {
    return "    _ \n  _| |_ \n |_   _| \n   |_| \n  /   \\\n";
}

std::string ASCIIArtLibrary::getArmory() {
    return "   _____\n  |  S  |\n  | /|\\ |\n  |_____|\n";
}

std::string ASCIIArtLibrary::getPrison() {
    return "  | | | |\n  | | | |\n  |_|_|_|\n";
}

std::string ASCIIArtLibrary::getTrap() {
    return "  /\\  /\\\n /  \\/  \\\n/________\\\n";
}

std::string ASCIIArtLibrary::getHealthIcon() {
    return "[+]";
}

std::string ASCIIArtLibrary::getManaIcon() {
    return "[*]";
}

std::string ASCIIArtLibrary::getGoldIcon() {
    return "[$]";
}

std::string ASCIIArtLibrary::getXPIcon() {
    return "[#]";
}

std::string ASCIIArtLibrary::getSkullIcon() {
    return "[X]";
}

std::string ASCIIArtLibrary::getStarIcon() {
    return "[*]";
}

std::string ASCIIArtLibrary::getTitleBanner() {
    return "====================================\n    ADVENTURE QUEST ENGINE\n====================================\n";
}

std::string ASCIIArtLibrary::getVictoryBanner() {
    return "====================================\n       V I C T O R Y !\n====================================\n";
}

std::string ASCIIArtLibrary::getDefeatBanner() {
    return "====================================\n       G A M E  O V E R\n====================================\n";
}

std::string ASCIIArtLibrary::getLevelUpBanner() {
    return "====================================\n       L E V E L  U P !\n====================================\n";
}

}  // namespace AQE
