





#ifndef COMBAT_H
#define COMBAT_H

#include "Types.h"
#include "SkillTree.h"
#include <vector>
#include <string>
#include <queue>

namespace AQE {






enum class CombatPosition {
    FRONT_LINE,          
    MID_LINE,            
    BACK_LINE            
};


enum class CombatAction {
    BASIC_ATTACK,
    HEAVY_ATTACK,        
    QUICK_ATTACK,        
    USE_SKILL,
    CAST_SPELL,          
    USE_ITEM,
    DEFEND,              
    CHANGE_POSITION,
    FLEE,
    COUNTERATTACK,       
    COMBO_FINISHER
};


struct ElementalResistance {
    DamageElement element;
    float resistancePercent;         
    
    ElementalResistance(DamageElement e = DamageElement::PHYSICAL, float r = 0.0f) 
        : element(e), resistancePercent(r) {}
};


struct EnhancedAttackResult {
    bool hit;
    bool critical;
    bool dodged;
    bool blocked;                    
    bool grazing;                    
    int damageDealt;
    int rawDamage;
    int shieldDamage;                
    DamageElement element;
    bool triggeredEffect;            
    StatusEffect triggeredStatusEffect;
    int comboPoints;                 
    bool isCounterattack;
    std::string message;
    
    EnhancedAttackResult() : hit(false), critical(false), dodged(false),
                            blocked(false), grazing(false), damageDealt(0),
                            rawDamage(0), shieldDamage(0), element(DamageElement::PHYSICAL),
                            triggeredEffect(false), comboPoints(1), isCounterattack(false) {}
};


struct CombatCombo {
    std::string name;
    int currentChain;                
    int maxChain;                    
    float damageMultiplier;          
    int comboMeter;                  
    bool finisherAvailable;          
    DamageElement comboElement;      
    
    CombatCombo() : currentChain(0), maxChain(5), damageMultiplier(1.1f),
                   comboMeter(0), finisherAvailable(false),
                   comboElement(DamageElement::PHYSICAL) {}
};


struct Combatant {
    std::string name;
    CombatStats stats;
    CombatPosition position;
    std::vector<StatusEffect> activeEffects;
    bool isPlayer;
    bool isCompanion;
    int companionId;                 
    CombatCombo combo;
    
    Combatant() : position(CombatPosition::FRONT_LINE), isPlayer(false),
                 isCompanion(false), companionId(-1) {}
};






struct AttackResult {
    bool hit;            
    bool critical;       
    bool dodged;         
    int damageDealt;     
    int rawDamage;       
    std::string message; 
    
    AttackResult() : hit(false), critical(false), dodged(false),
                     damageDealt(0), rawDamage(0) {}
};


struct AbilityResult {
    bool used;           
    bool success;        
    int damageDealt;     
    int healingDone;     
    std::string message; 
    StatusEffect appliedEffect;  
    
    AbilityResult() : used(false), success(false), damageDealt(0), healingDone(0) {}
};


struct SpellResult {
    bool cast;                       
    std::string message;             
    int damageDealt;                 
    int healingDone;                 
    std::vector<int> affectedTargets; 
    StatusEffect appliedEffect;      
    
    SpellResult() : cast(false), damageDealt(0), healingDone(0) {}
};


struct CombatResult {
    bool playerWon;          
    bool playerFled;         
    bool playerDied;         
    int experienceGained;    
    int goldGained;          
    std::vector<Item> loot;  
    std::vector<std::string> combatLog;  
    
    
    int totalDamageDealt;
    int totalDamageTaken;
    int criticalHitsLanded;
    int dodgesPerformed;
    int combosCompleted;
    int abilitiesUsed;
    int turnsElapsed;
    bool flawlessVictory;    
    
    CombatResult() : playerWon(false), playerFled(false), playerDied(false),
                     experienceGained(0), goldGained(0), totalDamageDealt(0),
                     totalDamageTaken(0), criticalHitsLanded(0), dodgesPerformed(0),
                     combosCompleted(0), abilitiesUsed(0), turnsElapsed(0),
                     flawlessVictory(false) {}
};






class CombatSystem {
public:
    
    CombatSystem(Player& player, int difficulty = 2);
    
    
    CombatResult startCombat(std::vector<Enemy>& enemies);
    
    
    CombatResult startCombat(Enemy& enemy);
    
    
    CombatStats getEffectivePlayerStats() const;
    
    
    CombatStats getEffectiveEnemyStats(const Enemy& enemy) const;

private:
    
    
    
    
    Player& playerRef;
    int gameDifficulty;
    std::vector<std::string> combatLog;
    bool combatEnded;
    bool playerVictory;
    
    
    
    
    
    
    bool combatLoop(std::vector<Enemy>& enemies, std::vector<Enemy>& defeatedEnemies);
    
    
    void displayCombatStatus(const std::vector<Enemy>& enemies) const;

    
    void rollEnemyIntents(std::vector<Enemy>& enemies);
    
    
    int getPlayerAction() const;
    
    
    int selectTarget(const std::vector<Enemy>& enemies) const;
    
    
    std::vector<std::pair<bool, int>> determineTurnOrder(
        const std::vector<Enemy>& enemies) const;
    
    
    
    
    
    
    AttackResult playerAttack(Enemy& target);
    
    
    AttackResult enemyAttack(const Enemy& attacker);
    
    
    bool rollToHit(int attackerSpeed, int targetDodge) const;
    
    
    bool rollCritical(int critChance) const;
    
    
    int calculateFinalDamage(int baseDamage, int defense, bool isCrit) const;

    
    int applyShieldThenDamage(int incoming, std::vector<StatusEffect>& effects, int& hpPool) const;
    
    
    
    
    
    
    AbilityResult usePlayerAbility(int abilityIndex, Enemy* target);
    
    
    AbilityResult useEnemyAbility(Enemy& enemy);
    
    
    bool shouldEnemyUseAbility(const Enemy& enemy) const;
    
    
    
    
    
    
    void applyStatusToPlayer(const StatusEffect& effect);
    
    
    void processPlayerStatuses();
    
    
    void processEnemyStatuses(Enemy& enemy);

    
    void tickStatuses(std::vector<StatusEffect>& effects, CombatStats& stats, bool isPlayer);

    
    void cleanupStatuses(std::vector<StatusEffect>& effects);
    
    
    
    
    
    
    bool attemptFlee(const std::vector<Enemy>& enemies);
    
    
    bool useItemInCombat(int itemIndex);
    
    
    SpellResult castSpellInCombat(int spellIndex, Enemy* target, std::vector<Enemy>* enemies);
    
    
    
    
    
    
    std::vector<Item> generateLoot(const std::vector<Enemy>& enemies);
    
    
    std::vector<Item> rollLootTable(const Enemy& enemy);
    
    
    int calculateExperience(const std::vector<Enemy>& enemies) const;
    
    
    int calculateGold(const std::vector<Enemy>& enemies) const;
    
    
    
    
    
    
    void log(const std::string& message);
    
    
    void displayMessage(const std::string& message, int delayMs = 500) const;
};






bool quickCombat(Player& player, Enemy& enemy);


bool hasCombatItems(const Player& player);


std::vector<int> getCombatItemIndices(const Player& player);


int applyDifficultyModifier(int damage, int difficulty, bool isPlayerDamage);


std::string generateCombatMessage(const std::string& attackerName,
                                  const std::string& defenderName,
                                  int damage, bool isCrit, bool isMiss);

}  

#endif 
