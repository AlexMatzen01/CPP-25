





#ifndef SKILLTREE_H
#define SKILLTREE_H

#include "Types.h"
#include <vector>
#include <string>
#include <map>

namespace AQE {





enum class TalentType {
    PASSIVE,         
    ACTIVE,          
    ULTIMATE,        
    SYNERGY          
};




struct TalentNode {
    int id;
    std::string name;
    std::string description;
    TalentType type;
    int tier;                        
    std::vector<int> prerequisites;  
    int currentRank;                 
    int maxRank;                     
    int pointCost;                   
    
    
    int hpBonus;
    int manaBonus;
    int attackBonus;
    int defenseBonus;
    int critChanceBonus;
    int critDamageBonus;
    int dodgeChanceBonus;
    int speedBonus;
    int magicPowerBonus;
    int magicResistBonus;
    float damageMultiplier;          
    float cooldownReduction;         
    float resourceRegen;             
    
    
    Skill ability;                   
    DamageElement element;           
    
    
    bool grantsDoubleJump;           
    bool grantsShieldRecharge;       
    bool grantsVampirism;            
    float vampirismPercent;          
    bool grantsThorns;               
    int thornsPercent;               
    bool grantsCounterattack;        
    
    TalentNode() : id(0), type(TalentType::PASSIVE), tier(1), currentRank(0),
                   maxRank(5), pointCost(1), hpBonus(0), manaBonus(0),
                   attackBonus(0), defenseBonus(0), critChanceBonus(0),
                   critDamageBonus(0), dodgeChanceBonus(0), speedBonus(0),
                   magicPowerBonus(0), magicResistBonus(0), damageMultiplier(1.0f),
                   cooldownReduction(0.0f), resourceRegen(0.0f),
                   element(DamageElement::PHYSICAL), grantsDoubleJump(false),
                   grantsShieldRecharge(false), grantsVampirism(false),
                   vampirismPercent(0.0f), grantsThorns(false), thornsPercent(0),
                   grantsCounterattack(false) {}
};





enum class WarriorSpec {
    BERSERKER,       
    TANK,            
    BLADESTORM       
};

enum class MageSpec {
    PYROMANCER,      
    CRYOMANCER,      
    ARCANIST         
};

enum class RogueSpec {
    ASSASSIN,        
    SHADOWDANCER,    
    POISONER         
};

enum class PaladinSpec {
    CRUSADER,        
    PROTECTOR,       
    JUSTICAR         
};





struct ComboChain {
    std::string name;
    std::vector<int> skillSequence;  
    int currentStep;                 
    int bonusDamage;                 
    StatusEffect bonusEffect;        
    float damageMultiplier;          
    
    ComboChain() : currentStep(0), bonusDamage(0), damageMultiplier(1.0f) {}
};





class SkillTreeManager {
public:
    SkillTreeManager(Player& player);
    
    
    void initializeTalentTree();
    void displayTalentTree() const;
    bool interactiveTalentMenu();
    void displaySkillLevelUpMenu();
    bool unlockTalent(int talentId);
    bool canUnlockTalent(int talentId) const;
    int getAvailableTalentPoints() const;
    
    
    void chooseSpecialization(int specIndex);
    std::string getCurrentSpecialization() const;

    
    void addTalentPoints(int amount = 1);
    
    
    void applyAllTalentBonuses();
    void recalculateStats();
    
    
    std::vector<Skill> getActiveAbilities() const;
    bool useAbility(int abilityId, Enemy& target);
    void updateCooldowns();
    
    
    void registerSkillUse(int skillId);
    bool checkComboCompletion();
    void resetCombo();
    std::string getCurrentComboProgress() const;
    
    
    bool hasPassive(const std::string& passiveName) const;
    float getPassiveValue(const std::string& passiveName) const;
    
    
    std::string serializeTalents() const;
    void deserializeTalents(const std::string& data);
    
private:
    Player& playerRef;
    std::vector<TalentNode> talentTree;
    std::map<CharacterClass, std::vector<TalentNode>> classTrees;
    std::vector<ComboChain> combos;
    ComboChain currentCombo;
    int selectedSpecialization;
    int totalTalentPoints;
    int spentTalentPoints;
    
    
    void buildWarriorTree();
    void buildMageTree();
    void buildRogueTree();
    void buildPaladinTree();
    std::vector<TalentNode> getUnlockedTalents() const;
    TalentNode* findTalent(int talentId);
};

}  

#endif 
