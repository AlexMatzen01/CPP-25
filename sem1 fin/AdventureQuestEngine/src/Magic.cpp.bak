//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

#include "Magic.h"
#include <algorithm>

namespace AQE {

// ---
// MAGIC SYSTEM IMPLEMENTATION
// ---

MagicSystem::MagicSystem(Player& player) : playerRef(player) {}

bool MagicSystem::learnSpell(const Spell& spell) {
    // Check if player's class can learn this spell
    if (!canLearnSpell(playerRef.charClass, spell)) {
        return false;
    }
    
    // Check if already learned
    for (const auto& known : knownSpells) {
        if (known.id == spell.id) {
            return false; // Already learned
        }
    }
    
    knownSpells.push_back(spell);
    playerRef.spellsLearned++;
    return true;
}

std::vector<Spell>& MagicSystem::getKnownSpells() {
    return knownSpells;
}

bool MagicSystem::canCastMagic(CharacterClass charClass) {
    return charClass == CharacterClass::MAGE || 
           charClass == CharacterClass::PALADIN ||
           charClass == CharacterClass::NECROMANCER ||
           charClass == CharacterClass::RANGER;
}

bool MagicSystem::canLearnSpell(CharacterClass charClass, const Spell& spell) {
    // Check if class can learn this spell
    for (const auto& restrictedClass : spell.restrictedClasses) {
        if (restrictedClass == charClass) {
            return true;
        }
    }
    return false;
}

std::vector<MagicSchool> MagicSystem::getAvailableSchools(CharacterClass charClass) {
    std::vector<MagicSchool> schools;
    
    switch (charClass) {
        case CharacterClass::MAGE:
            schools = { MagicSchool::EVOCATION, MagicSchool::ABJURATION, 
                       MagicSchool::TRANSMUTATION, MagicSchool::CONJURATION };
            break;
        case CharacterClass::PALADIN:
            schools = { MagicSchool::ABJURATION, MagicSchool::RESTORATION, 
                       MagicSchool::ENCHANTMENT };
            break;
        case CharacterClass::NECROMANCER:
            schools = { MagicSchool::NECROMANCY, MagicSchool::EVOCATION, 
                       MagicSchool::TRANSMUTATION };
            break;
        case CharacterClass::RANGER:
            schools = { MagicSchool::CONJURATION, MagicSchool::DIVINATION, 
                       MagicSchool::RESTORATION };
            break;
        default:
            break;
    }
    
    return schools;
}

// ---
// SPELL LIBRARY IMPLEMENTATIONS
// ---

namespace SpellLibrary {

Spell createFirebolt() {
    Spell spell;
    spell.id = 101;
    spell.name = "Firebolt";
    spell.description = "Launch a bolt of fire at a single target";
    spell.school = MagicSchool::EVOCATION;
    spell.manaCost = 25;  // Increased from 20
    spell.cooldown = 1;
    spell.level = 1;
    spell.damageMultiplier = 1.0f;  // Reduced from 1.2f
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::MAGE };
    return spell;
}

Spell createFrostbolt() {
    Spell spell;
    spell.id = 102;
    spell.name = "Frostbolt";
    spell.description = "Launch a bolt of ice, slowing the target";
    spell.school = MagicSchool::EVOCATION;
    spell.manaCost = 28;  // Increased from 20
    spell.cooldown = 2;  // Increased from 1
    spell.level = 2;
    spell.damageMultiplier = 0.9f;  // Reduced from 1.1f
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = true;
    spell.restrictedClasses = { CharacterClass::MAGE };
    
    spell.appliedEffect.name = "Frozen";
    spell.appliedEffect.description = "Slowed and chilled";
    spell.appliedEffect.speedMod = -3;
    spell.appliedEffect.turnsRemaining = 3;
    spell.appliedEffect.isDebuff = true;
    
    return spell;
}

Spell createChainLightning() {
    Spell spell;
    spell.id = 103;
    spell.name = "Chain Lightning";
    spell.description = "Lightning jumps between all enemies";
    spell.school = MagicSchool::EVOCATION;
    spell.manaCost = 45;  // Increased from 35
    spell.cooldown = 3;  // Increased from 2
    spell.level = 4;
    spell.damageMultiplier = 1.1f;  // Reduced from 1.5f
    spell.isAOE = true;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::MAGE };
    return spell;
}

Spell createInferno() {
    Spell spell;
    spell.id = 104;
    spell.name = "Inferno";
    spell.description = "Massive fire explosion, deals damage and applies burn";
    spell.school = MagicSchool::EVOCATION;
    spell.manaCost = 60;  // Increased from 50
    spell.cooldown = 4;  // Increased from 3
    spell.level = 8;
    spell.damageMultiplier = 1.5f;  // Reduced from 2.0f
    spell.isAOE = true;
    spell.isHeal = false;
    spell.isDebuff = true;
    spell.restrictedClasses = { CharacterClass::MAGE };
    
    spell.appliedEffect.name = "Burning";
    spell.appliedEffect.description = "Taking fire damage over time";
    spell.appliedEffect.dotDamage = 5;
    spell.appliedEffect.dotIsMagic = true;
    spell.appliedEffect.turnsRemaining = 4;
    spell.appliedEffect.isDebuff = true;
    spell.appliedEffect.ticksStartOfTurn = true;
    
    return spell;
}

Spell createBlizzard() {
    Spell spell;
    spell.id = 105;
    spell.name = "Blizzard";
    spell.description = "Winter storm hits all enemies, chilling them";
    spell.school = MagicSchool::EVOCATION;
    spell.manaCost = 60;  // Increased from 50
    spell.cooldown = 4;  // Increased from 3
    spell.level = 8;
    spell.damageMultiplier = 1.4f;  // Reduced from 1.8f
    spell.isAOE = true;
    spell.isHeal = false;
    spell.isDebuff = true;
    spell.restrictedClasses = { CharacterClass::MAGE };
    
    spell.appliedEffect.name = "Frozen Solid";
    spell.appliedEffect.description = "Severely slowed and weakened";
    spell.appliedEffect.speedMod = -5;
    spell.appliedEffect.attackMod = -3;
    spell.appliedEffect.turnsRemaining = 3;
    spell.appliedEffect.isDebuff = true;
    
    return spell;
}

Spell createMagicShield() {
    Spell spell;
    spell.id = 201;
    spell.name = "Magic Shield";
    spell.description = "Create a magical barrier to reduce damage";
    spell.school = MagicSchool::ABJURATION;
    spell.manaCost = 30;  // Increased from 25
    spell.cooldown = 2;  // Increased from 1
    spell.level = 1;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::MAGE, CharacterClass::PALADIN };
    
    spell.appliedEffect.name = "Protected";
    spell.appliedEffect.description = "Shielded by magic";
    spell.appliedEffect.defenseMod = 8;
    spell.appliedEffect.shieldAmount = 30;
    spell.appliedEffect.turnsRemaining = 3;
    spell.appliedEffect.isDebuff = false;
    
    return spell;
}

Spell createReflectionAura() {
    Spell spell;
    spell.id = 202;
    spell.name = "Reflection Aura";
    spell.description = "Reflect a portion of damage back to attackers";
    spell.school = MagicSchool::ABJURATION;
    spell.manaCost = 48;
    spell.cooldown = 3;
    spell.level = 5;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::PALADIN };
    
    spell.appliedEffect.name = "Reflective";
    spell.appliedEffect.description = "Attacks bounce back damage";
    spell.appliedEffect.defenseMod = 5;
    spell.appliedEffect.turnsRemaining = 4;
    spell.appliedEffect.isDebuff = false;
    
    return spell;
}

Spell createStrengthBoost() {
    Spell spell;
    spell.id = 301;
    spell.name = "Strength Boost";
    spell.description = "Increase all damage output temporarily";
    spell.school = MagicSchool::TRANSMUTATION;
    spell.manaCost = 40;
    spell.cooldown = 2;
    spell.level = 2;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::MAGE, CharacterClass::PALADIN };
    
    spell.appliedEffect.name = "Empowered";
    spell.appliedEffect.description = "Increased damage output";
    spell.appliedEffect.attackMod = 6;
    spell.appliedEffect.turnsRemaining = 4;
    spell.appliedEffect.isDebuff = false;
    
    return spell;
}

Spell createEnfeeblement() {
    Spell spell;
    spell.id = 302;
    spell.name = "Enfeeblement";
    spell.description = "Weaken target's attacks";
    spell.school = MagicSchool::TRANSMUTATION;
    spell.manaCost = 35;
    spell.cooldown = 2;
    spell.level = 2;
    spell.damageMultiplier = 0.2f;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = true;
    spell.restrictedClasses = { CharacterClass::MAGE, CharacterClass::NECROMANCER };
    
    spell.appliedEffect.name = "Enfeebled";
    spell.appliedEffect.description = "Weakened and vulnerable";
    spell.appliedEffect.attackMod = -5;
    spell.appliedEffect.defenseMod = -2;
    spell.appliedEffect.turnsRemaining = 3;
    spell.appliedEffect.isDebuff = true;
    
    return spell;
}

Spell createHaste() {
    Spell spell;
    spell.id = 303;
    spell.name = "Haste";
    spell.description = "Increase speed and dodge chance";
    spell.school = MagicSchool::TRANSMUTATION;
    spell.manaCost = 45;
    spell.cooldown = 3;
    spell.level = 3;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::MAGE, CharacterClass::RANGER };
    
    spell.appliedEffect.name = "Hasted";
    spell.appliedEffect.description = "Moving with supernatural speed";
    spell.appliedEffect.speedMod = 5;
    spell.appliedEffect.dodgeMod = 4;
    spell.appliedEffect.turnsRemaining = 3;
    spell.appliedEffect.isDebuff = false;
    
    return spell;
}

Spell createSlowness() {
    Spell spell;
    spell.id = 304;
    spell.name = "Slowness";
    spell.description = "Reduce target's speed and dodge";
    spell.school = MagicSchool::TRANSMUTATION;
    spell.manaCost = 40;
    spell.cooldown = 2;
    spell.level = 3;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = true;
    spell.restrictedClasses = { CharacterClass::MAGE, CharacterClass::RANGER };
    
    spell.appliedEffect.name = "Slowed";
    spell.appliedEffect.description = "Moving through molasses";
    spell.appliedEffect.speedMod = -4;
    spell.appliedEffect.dodgeMod = -3;
    spell.appliedEffect.turnsRemaining = 3;
    spell.appliedEffect.isDebuff = true;
    
    return spell;
}

Spell createDrainLife() {
    Spell spell;
    spell.id = 401;
    spell.name = "Drain Life";
    spell.description = "Siphon life from target, healing yourself";
    spell.school = MagicSchool::NECROMANCY;
    spell.manaCost = 40;  // Increased from 35
    spell.cooldown = 2;
    spell.level = 3;
    spell.damageMultiplier = 1.1f;  // Reduced from 1.3f
    spell.healMultiplier = 0.4f;  // Reduced from 0.6f
    spell.isAOE = false;
    spell.isHeal = true;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::NECROMANCER };
    
    return spell;
}

Spell createCurseOfWeakness() {
    Spell spell;
    spell.id = 402;
    spell.name = "Curse of Weakness";
    spell.description = "Curse target with powerful weakness";
    spell.school = MagicSchool::NECROMANCY;
    spell.manaCost = 38;
    spell.cooldown = 2;
    spell.level = 2;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = true;
    spell.restrictedClasses = { CharacterClass::NECROMANCER };
    
    spell.appliedEffect.name = "Cursed";
    spell.appliedEffect.description = "Weakened by dark magic";
    spell.appliedEffect.attackMod = -7;
    spell.appliedEffect.defenseMod = -3;
    spell.appliedEffect.turnsRemaining = 4;
    spell.appliedEffect.isDebuff = true;
    
    return spell;
}

Spell createUndeadSummon() {
    Spell spell;
    spell.id = 403;
    spell.name = "Summon Undead";
    spell.description = "Summon undead minion to aid in combat";
    spell.school = MagicSchool::NECROMANCY;
    spell.manaCost = 65;
    spell.cooldown = 4;
    spell.level = 6;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::NECROMANCER };
    
    return spell;
}

Spell createDeathWish() {
    Spell spell;
    spell.id = 404;
    spell.name = "Death Wish";
    spell.description = "Extreme damage at great personal risk";
    spell.school = MagicSchool::NECROMANCY;
    spell.manaCost = 75;  // Increased from 60
    spell.cooldown = 5;  // Increased from 4
    spell.level = 10;
    spell.damageMultiplier = 2.2f;  // Reduced from 3.0f
    spell.isAOE = true;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::NECROMANCER };
    
    spell.appliedEffect.name = "Cursed";
    spell.appliedEffect.description = "Magical backlash from forbidden magic";
    spell.appliedEffect.attackMod = -4;
    spell.appliedEffect.defenseMod = -4;
    spell.appliedEffect.turnsRemaining = 2;
    spell.appliedEffect.isDebuff = true;
    
    return spell;
}

Spell createHeal() {
    Spell spell;
    spell.id = 501;
    spell.name = "Heal";
    spell.description = "Restore health to self or ally";
    spell.school = MagicSchool::RESTORATION;
    spell.manaCost = 35;  // Increased from 25
    spell.cooldown = 2;  // Increased from 1
    spell.level = 1;
    spell.healMultiplier = 1.2f;  // Reduced from 1.5f
    spell.isAOE = false;
    spell.isHeal = true;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::PALADIN, CharacterClass::RANGER };
    
    return spell;
}

Spell createGreaterHeal() {
    Spell spell;
    spell.id = 502;
    spell.name = "Greater Heal";
    spell.description = "Restore substantial health";
    spell.school = MagicSchool::RESTORATION;
    spell.manaCost = 55;  // Increased from 45
    spell.cooldown = 3;  // Increased from 2
    spell.level = 5;
    spell.healMultiplier = 1.8f;  // Reduced from 2.5f
    spell.isAOE = false;
    spell.isHeal = true;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::PALADIN, CharacterClass::RANGER };
    
    return spell;
}

Spell createDispelMagic() {
    Spell spell;
    spell.id = 503;
    spell.name = "Dispel Magic";
    spell.description = "Remove magical effects from target";
    spell.school = MagicSchool::RESTORATION;
    spell.manaCost = 42;
    spell.cooldown = 2;
    spell.level = 4;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::PALADIN };
    
    return spell;
}

Spell createArcaneMissiles() {
    Spell spell;
    spell.id = 601;
    spell.name = "Arcane Missiles";
    spell.description = "Hail of magical projectiles";
    spell.school = MagicSchool::CONJURATION;
    spell.manaCost = 38;
    spell.cooldown = 2;
    spell.level = 2;
    spell.damageMultiplier = 0.65f;
    spell.isAOE = true;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::MAGE };
    
    return spell;
}

Spell createMeteorShower() {
    Spell spell;
    spell.id = 602;
    spell.name = "Meteor Shower";
    spell.description = "Rain meteors on all enemies";
    spell.school = MagicSchool::CONJURATION;
    spell.manaCost = 65;
    spell.cooldown = 4;
    spell.level = 7;
    spell.damageMultiplier = 1.4f;
    spell.isAOE = true;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::MAGE };
    
    return spell;
}

Spell createAnalyzeWeakness() {
    Spell spell;
    spell.id = 701;
    spell.name = "Analyze Weakness";
    spell.description = "Reveal enemy weaknesses and increase crit chance";
    spell.school = MagicSchool::DIVINATION;
    spell.manaCost = 28;
    spell.cooldown = 3;
    spell.level = 2;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = false;
    spell.restrictedClasses = { CharacterClass::RANGER };
    
    spell.appliedEffect.name = "Analyzed";
    spell.appliedEffect.description = "Weaknesses revealed, increased vulnerability";
    spell.appliedEffect.critMod = 8;
    spell.appliedEffect.turnsRemaining = 3;
    spell.appliedEffect.isDebuff = false;
    
    return spell;
}

Spell createFascinate() {
    Spell spell;
    spell.id = 801;
    spell.name = "Fascinate";
    spell.description = "Mesmerize an enemy, reducing their accuracy";
    spell.school = MagicSchool::ENCHANTMENT;
    spell.manaCost = 36;
    spell.cooldown = 3;
    spell.level = 3;
    spell.isAOE = false;
    spell.isHeal = false;
    spell.isDebuff = true;
    spell.restrictedClasses = { CharacterClass::PALADIN };
    
    spell.appliedEffect.name = "Fascinated";
    spell.appliedEffect.description = "Mesmerized and confused";
    spell.appliedEffect.dodgeMod = -5;
    spell.appliedEffect.attackMod = -4;
    spell.appliedEffect.turnsRemaining = 2;
    spell.appliedEffect.isDebuff = true;
    
    return spell;
}

} // namespace SpellLibrary

} // namespace AQE
