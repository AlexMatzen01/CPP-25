//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Enhanced UI system (partially implemented - many features scrapped)

#ifndef UI_ENHANCED_H
#define UI_ENHANCED_H

#include "Types.h"
#include "Combat.h"
#include <string>
#include <vector>
#include <map>

namespace AQE {

// ---
// COLOR SYSTEM (ANSI CODES)
// ---

enum class UIColor {
    BLACK = 30,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    WHITE = 37,
    BRIGHT_BLACK = 90,
    BRIGHT_RED = 91,
    BRIGHT_GREEN = 92,
    BRIGHT_YELLOW = 93,
    BRIGHT_BLUE = 94,
    BRIGHT_MAGENTA = 95,
    BRIGHT_CYAN = 96,
    BRIGHT_WHITE = 97,
    RESET = 0
};

enum class UIEffect {
    BOLD = 1,
    DIM = 2,
    ITALIC = 3,
    UNDERLINE = 4,
    BLINK = 5,
    REVERSE = 7,
    HIDDEN = 8
};

// ---
// UI ELEMENTS
// ---

struct HealthBar {
    int current;
    int maximum;
    int width;                           // Bar width in characters
    UIColor fullColor;                   // Color when high HP
    UIColor midColor;                    // Color when medium HP
    UIColor lowColor;                    // Color when low HP
    std::string prefix;                  // "HP: " or similar
    bool showNumbers;                    // Display "50/100"
    bool showPercentage;                 // Display "(50%)"
    
    HealthBar() : current(100), maximum(100), width(20),
                 fullColor(UIColor::BRIGHT_GREEN), midColor(UIColor::YELLOW),
                 lowColor(UIColor::BRIGHT_RED), prefix("HP: "),
                 showNumbers(true), showPercentage(false) {}
};

struct ProgressBar {
    int current;
    int maximum;
    int width;
    UIColor barColor;
    UIColor backgroundColor;
    char fillChar;
    char emptyChar;
    std::string label;
    
    ProgressBar() : current(0), maximum(100), width(30),
                   barColor(UIColor::CYAN), backgroundColor(UIColor::BLACK),
                   fillChar('='), emptyChar(' ') {}
};

struct StatusDisplay {
    std::string name;
    int hp;
    int maxHP;
    int mana;
    int maxMana;
    int level;
    int gold;
    std::vector<std::string> activeBuffs;
    std::vector<std::string> activeDebuffs;
    std::string location;
    
    StatusDisplay() : hp(0), maxHP(0), mana(0), maxMana(0), 
                     level(1), gold(0) {}
};

// ---
// ASCII ART FRAMES AND ANIMATIONS
// ---

struct ASCIIFrame {
    std::vector<std::string> lines;
    int delayMs;                         // Display duration
    
    ASCIIFrame() : delayMs(100) {}
};

struct Animation {
    std::string name;
    std::vector<ASCIIFrame> frames;
    bool loop;
    int currentFrame;
    
    Animation() : loop(false), currentFrame(0) {}
};

// ---
// VISUAL EFFECTS
// ---

enum class EffectType {
    SLASH,               // Sword slash animation
    FIREBALL,            // Fire spell
    LIGHTNING,           // Lightning bolt
    HEAL,                // Healing sparkles
    EXPLOSION,           // AoE attack
    SHIELD,              // Defense buff
    POISON,              // Poison cloud
    ICE,                 // Ice shard
    BLOOD,               // Damage taken
    DEATH                // Enemy defeated
};

struct VisualEffect {
    EffectType type;
    std::string asciiArt;
    UIColor primaryColor;
    UIColor secondaryColor;
    int durationMs;
    
    VisualEffect() : type(EffectType::SLASH), primaryColor(UIColor::WHITE),
                    secondaryColor(UIColor::WHITE), durationMs(500) {}
};

// ---
// MENU SYSTEM
// ---

struct MenuItem {
    std::string text;
    std::string icon;                    // ASCII icon/emoji
    std::string description;             // Shown when highlighted
    bool isEnabled;
    bool isSeparator;
    int value;                           // Associated value/ID
    UIColor textColor;
    
    MenuItem() : isEnabled(true), isSeparator(false), value(0),
                textColor(UIColor::WHITE) {}
};

struct Menu {
    std::string title;
    std::vector<MenuItem> items;
    int selectedIndex;
    UIColor titleColor;
    UIColor selectedColor;
    UIColor normalColor;
    UIColor borderColor;
    bool showBorder;
    std::string borderStyle;             // "single", "double", "thick"
    
    Menu() : selectedIndex(0), titleColor(UIColor::CYAN),
            selectedColor(UIColor::BRIGHT_YELLOW), normalColor(UIColor::WHITE),
            borderColor(UIColor::BLUE), showBorder(true), borderStyle("double") {}
};

// ---
// COMBAT LOG FORMATTING
// ---

enum class LogLevel {
    INFO,
    SUCCESS,
    WARNING,
    ERROR,
    COMBAT_HIT,
    COMBAT_MISS,
    COMBAT_CRIT,
    HEAL,
    BUFF,
    DEBUFF,
    LOOT,
    QUEST
};

struct LogEntry {
    std::string message;
    LogLevel level;
    UIColor color;
    long long timestamp;                 // For filtering
    bool important;                      // Show in summary
    
    LogEntry() : level(LogLevel::INFO), color(UIColor::WHITE),
                timestamp(0), important(false) {}
};

// ---
// UI MANAGER CLASS
// ---

class UIManager {
public:
    UIManager();
    
    // Color and formatting
    static std::string colorize(const std::string& text, UIColor color, UIEffect effect = static_cast<UIEffect>(0));
    static std::string colorizeRGB(const std::string& text, int r, int g, int b);
    static std::string addEffect(const std::string& text, UIEffect effect);
    static std::string resetColor();
    
    // Health/Progress bars
    static std::string renderHealthBar(const HealthBar& bar);
    static std::string renderProgressBar(const ProgressBar& bar);
    static std::string renderManaBar(int current, int max, int width = 20);
    static std::string renderXPBar(int current, int needed, int width = 30);
    
    // Status displays
    static void displayPlayerStatus(const StatusDisplay& status);
    static void displayEnemyStatus(const std::string& name, int hp, int maxHP, const std::vector<std::string>& effects);
    static void displayCompactStats(const Player& player);
    static void displayFullStats(const Player& player);
    
    // Boxes and borders
    static std::string createBox(const std::string& content, int width, const std::string& style = "double");
    static std::string createTitle(const std::string& text, int width, UIColor color = UIColor::CYAN);
    static void drawBorder(int width, const std::string& style = "single");
    static void drawDivider(int width, char character = '-');
    
    // Menus
    static int displayMenu(Menu& menu);                 // Interactive menu
    static void renderMenu(const Menu& menu);           // Just draw it
    static int selectFromList(const std::vector<std::string>& items, const std::string& title);
    
    // Combat visuals
    static void playAttackAnimation(DamageElement element);
    static void showDamageNumber(int damage, bool isCrit);
    static void showHealingNumber(int healing);
    static void displayCombatLog(const std::vector<LogEntry>& log, int maxLines = 5);
    
    // ASCII Art
    static void displayASCIIArt(const std::string& artName);
    static void playAnimation(const Animation& anim);
    static std::string getElementIcon(DamageElement element);
    static std::string getClassIcon(CharacterClass charClass);
    static std::string getRarityColor(ItemRarity rarity);
    
    // Visual effects
    static void showEffect(EffectType effect, int x = 0, int y = 0);
    static void screenShake(int intensity = 3);
    static void fadeTransition(int durationMs = 1000);
    static void flashScreen(UIColor color, int times = 3);
    
    // Panels and layouts
    static void displayTwoColumn(const std::string& left, const std::string& right, int width = 80);
    static void displayThreeColumn(const std::string& left, const std::string& mid, 
                                   const std::string& right, int width = 120);
    static void displayPanel(const std::string& title, const std::string& content, 
                            int width = 60, UIColor borderColor = UIColor::BLUE);
    
    // Logging
    static void log(const std::string& message, LogLevel level = LogLevel::INFO);
    static void logCombat(const std::string& message, bool isCrit = false, bool isMiss = false);
    static void logLoot(const std::string& itemName, ItemRarity rarity);
    static void logQuest(const std::string& message, bool isComplete = false);
    
    // Utilities
    static void clearScreen();
    static void moveCursor(int x, int y);
    static void hideCursor();
    static void showCursor();
    static void waitForKey();
    static void sleep(int milliseconds);
    static std::string centerText(const std::string& text, int width);
    static std::string padRight(const std::string& text, int width);
    static std::string padLeft(const std::string& text, int width);
    
    // Particle effects (text)
    static void particleBurst(int x, int y, UIColor color, char particle = '*');
    static void floatingText(const std::string& text, int x, int y, UIColor color);
    static void typewriterEffect(const std::string& text, int delayMs = 30);
    
    // Advanced features
    static void displayItemComparison(const Item& item1, const Item& item2);
    static void displaySkillTree(const std::vector<std::string>& nodes);
    static void displayMap(const std::vector<std::vector<char>>& map, int playerX, int playerY);
    static void displayInventoryGrid(const std::vector<Item>& items, int columns = 4);
    
private:
    static std::map<std::string, Animation> animations;
    static std::vector<LogEntry> globalLog;
    
    static void initializeAnimations();
    static std::string getBoxChar(const std::string& style, const std::string& position);
};

// ---
// ENHANCED ASCII ART LIBRARY
// ---

class ASCIIArtLibrary {
public:
    // Class emblems
    static std::string getWarriorEmblem();
    static std::string getMageEmblem();
    static std::string getRogueEmblem();
    static std::string getPaladinEmblem();
    static std::string getRangerEmblem();
    static std::string getNecromancerEmblem();
    static std::string getBarbarianEmblem();
    static std::string getMonkEmblem();
    
    // Combat effects
    static std::string getSwordSlash();
    static std::string getFireball();
    static std::string getLightningBolt();
    static std::string getShieldBlock();
    static std::string getPoisonCloud();
    static std::string getIceShard();
    
    // Enemy art
    static std::string getGoblinArt();
    static std::string getOrcArt();
    static std::string getDragonArt();
    static std::string getUndeadArt();
    static std::string getBossArt();
    
    // Environment
    static std::string getTreasureChest();
    static std::string getDoorway();
    static std::string getCampfire();
    static std::string getShrine();
    static std::string getLibrary();
    static std::string getForge();
    static std::string getLaboratory();
    static std::string getGraveyard();
    static std::string getArmory();
    static std::string getPrison();
    static std::string getTrap();
    
    // Status icons
    static std::string getHealthIcon();
    static std::string getManaIcon();
    static std::string getGoldIcon();
    static std::string getXPIcon();
    static std::string getSkullIcon();
    static std::string getStarIcon();
    
    // UI elements
    static std::string getTitleBanner();
    static std::string getVictoryBanner();
    static std::string getDefeatBanner();
    static std::string getLevelUpBanner();
};

}  // namespace AQE

#endif // UI_ENHANCED_H
