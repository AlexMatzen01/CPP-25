//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Magic system for spellcasting classes with restricted access

#ifndef MAGIC_H
#define MAGIC_H

#include "Types.h"
#include <vector>
#include <string>

namespace AQE {

/**
// Magic system manager
 */
class MagicSystem {
public:
    /**
// Constructor
     * @param player Reference to player
     */
    MagicSystem(Player& player);
    
    /**
// Learn a spell
     * @param spell Spell to learn
     * @return True if successfully learned
     */
    bool learnSpell(const Spell& spell);
    
    /**
// Get spells available to player
     * @return Vector of known spells
     */
    std::vector<Spell>& getKnownSpells();
    
    /**
// Check if class can cast magic
     * @param charClass Class to check
     * @return True if class is magical
     */
    static bool canCastMagic(CharacterClass charClass);
    
    /**
// Check if class can learn a specific spell
     * @param charClass Class to check
     * @param spell Spell to check
     * @return True if class can learn spell
     */
    static bool canLearnSpell(CharacterClass charClass, const Spell& spell);
    
    /**
// Get magical schools available to a class
     * @param charClass Character class
     * @return Vector of available schools
     */
    static std::vector<MagicSchool> getAvailableSchools(CharacterClass charClass);
    
    //==========================================================================
    // UTILITY METHODS
    //==========================================================================
    
    /**
// Get spell by ID
     * @param spellId ID to search for
     * @return Pointer to spell or nullptr if not found
     */
    Spell* getSpellById(int spellId);
    
    /**
// Check if player has learned a specific spell
     * @param spellId ID of spell to check
     * @return True if spell is known
     */
    bool hasSpell(int spellId) const;
    
    /**
// Get total number of spells known
     * @return Count of known spells
     */
    int getSpellCount() const;
    
    /**
// Calculate mana cost for a spell
     * @param spell The spell to calculate cost for
     * @return Mana points required
     */
    static int getManaCost(const Spell& spell);
    
    /**
// Check if player has enough mana to cast spell
     * @param spell Spell to check
     * @param currentMana Current mana amount
     * @return True if enough mana
     */
    static bool canCastSpell(const Spell& spell, int currentMana);
    
    /**
// Get spells of a specific school
     * @param school Magic school to filter by
     * @return Vector of spells from that school
     */
    std::vector<Spell> getSpellsBySchool(MagicSchool school) const;
    
    /**
// Get spells with minimum damage
     * @param minDamage Minimum damage value
     * @return Vector of damaging spells above threshold
     */
    std::vector<Spell> getDamagingSpells(int minDamage) const;
    
    /**
// Get healing spells known
     * @return Vector of healing spells
     */
    std::vector<Spell> getHealingSpells() const;
    
private:
    Player& playerRef;
    std::vector<Spell> knownSpells;
};

/**
// Create all spell templates
 */
namespace SpellLibrary {
    
    // Evocation Spells (Fire, Ice, Lightning damage)
    Spell createFirebolt();
    Spell createFrostbolt();
    Spell createChainLightning();
    Spell createInferno();
    Spell createBlizzard();
    
    // Abjuration Spells (Defense)
    Spell createMagicShield();
    Spell createReflectionAura();
    
    // Transmutation Spells (Buffs/Debuffs)
    Spell createStrengthBoost();
    Spell createEnfeeblement();
    Spell createHaste();
    Spell createSlowness();
    
    // Necromancy Spells (Dark magic, life drain)
    Spell createDrainLife();
    Spell createCurseOfWeakness();
    Spell createUndeadSummon();
    Spell createDeathWish();
    
    // Restoration Spells (Healing, cleansing)
    Spell createHeal();
    Spell createGreaterHeal();
    Spell createDispelMagic();
    
    // Conjuration Spells (Temporary effects)
    Spell createArcaneMissiles();
    Spell createMeteorShower();
    
    // Divination Spells (Utility, buffs)
    Spell createAnalyzeWeakness();
    
    // Enchantment Spells (Control)
    Spell createFascinate();
}

} // namespace AQE

#endif // MAGIC_H
