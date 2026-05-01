





#ifndef MAGIC_H
#define MAGIC_H

#include "Types.h"
#include <vector>
#include <string>

namespace AQE {


class MagicSystem {
public:
    
    MagicSystem(Player& player);
    
    
    bool learnSpell(const Spell& spell);
    
    
    std::vector<Spell>& getKnownSpells();
    
    
    static bool canCastMagic(CharacterClass charClass);
    
    
    static bool canLearnSpell(CharacterClass charClass, const Spell& spell);
    
    
    static std::vector<MagicSchool> getAvailableSchools(CharacterClass charClass);
    
private:
    Player& playerRef;
    std::vector<Spell> knownSpells;
};


namespace SpellLibrary {
    
    
    Spell createFirebolt();
    Spell createFrostbolt();
    Spell createChainLightning();
    Spell createInferno();
    Spell createBlizzard();
    
    
    Spell createMagicShield();
    Spell createReflectionAura();
    
    
    Spell createStrengthBoost();
    Spell createEnfeeblement();
    Spell createHaste();
    Spell createSlowness();
    
    
    Spell createDrainLife();
    Spell createCurseOfWeakness();
    Spell createUndeadSummon();
    Spell createDeathWish();
    
    
    Spell createHeal();
    Spell createGreaterHeal();
    Spell createDispelMagic();
    
    
    Spell createArcaneMissiles();
    Spell createMeteorShower();
    
    
    Spell createAnalyzeWeakness();
    
    
    Spell createFascinate();
}

} 

#endif 
