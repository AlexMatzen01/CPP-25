# AdventureQuestEngine Core Calculations & Formulas

This document details all mathematical operations, underlying equations, and variables that govern the mechanics in AdventureQuestEngine.

## 1. Combat Formulas
Combat relies heavily on stat comparisons and introduces variance and mitigations depending on the attack type.

### A. Core Combat Traits
These are fundamental rules that bound combat mechanics:
- **Maximum Crit Chance:** 50%
- **Maximum Dodge Chance:** 75%
- **Critical Damage Multiplier:** 2x (`BASE_CRIT_MULTIPLIER`)

### B. Physical Attack Sequence
Physical strikes consist of evaluating a hit chance, determining base damage roll, and applying a diminishing-returns defensive mitigation scale.

1. **Hit Chance**:  
   $$HitChance = 90 + AttackerSpeed - TargetDodge$$  
   _Constraints_: Minimum 30%, Maximum 98%.

2. **Base Damage Roll**:  
   $$BaseDamage = AttackerAttack + Random(0, 4) + 2$$  
   _Variance represents slight differences in weapon strikes (-2 to +2 shifted up)._

3. **Defense Mitigation Ratio**:  
   $$Mitigation = \frac{TargetDefense}{TargetDefense + 60.0}$$  
   _Constraints_: Bounded between 0% (0.0) and 70% (0.7). Defense provides diminishing returns scaling off the constant `60`.

4. **Pre-Mitigation Damage**:  
   $$Damage_{Pre} = BaseDamage \times (isCritical ? 2.0 : 1.0)$$

5. **Final Physical Damage**:  
   $$FinalDamage = floor(Damage_{Pre} \times (1.0 - Mitigation))$$  
   _Minimum damage done must equal `1`._

6. **Vulnerability Scaling**:  
   If the target has an active vulnerable status effect, the amount is scaled upward:  
   $$FinalDamage_{Vuln} = FinalDamage + \frac{FinalDamage \times Vulnerable\%}{100}$$  
   _(After this, shields directly subtract from the incoming damage values until they break)._

### C. Magical Attack Sequence
Magical skills skip evasion mechanics and handle critical strikes & mitigations slightly differently.

1. **Hit Chance**:  
   Spells never miss (Hit Chance mathematically 100% and skips dodge evaluations).
   
2. **Base Magic Damage**:  
   $$BaseDamage = AttackerMagicPower \times Spell.DamageMultiplier$$

3. **Final Magic Damage**:  
   $$FinalDamage = BaseDamage - TargetMagicResist$$  
   _Minimum damage is 1._

4. **Critical Modifiers for Magic**:  
   Unlike physical combat where crits multiply base magnitude before mitigation, spell critical hits apply their multiplier (+2x) **after** magic resistance has deducted damage.  
   $$CritDamage_{Magic} = FinalDamage \times 2$$

## 2. Fleeting From Combat
Attempting to flee from an active combat scenario factors in the relative speeds of the participants.

$$AverageEnemySpeed = \frac{\sum EnemySpeed}{EnemyCount}$$  
$$FleeChance (\%) = 40 + [ (PlayerSpeed - AverageEnemySpeed) \times 2 ]$$  

_Constraints_: Flee chance guarantees at least a 10% bottom-out chance, climbing to a max of 80%.

## 3. Experience & Progression (Level Up)
Leveling requires exponentially higher XP thresholds according to standard RPG multiplicative formulas.

1. **Experience Formula (Required to Level Up)**:  
   $$RequiredXP = 100 \times (1.5^{(PlayerLevel - 1)})$$

2. **Experience Yields From Enemies**:  
   $$GainedXP_{Base} = \sum TargetEnemy.ExperienceReward$$
   - **Easy Mode**: $GainedXP_{Base} \times 0.75$
   - **Normal Mode**: $GainedXP_{Base} \times 1.0$
   - **Hard Mode**: $GainedXP_{Base} \times 1.5$

3. **Level Up Stat Modifications**:  
   When achieving a new level, the player's Max HP and Max Mana are fully restored. New stats are added based on the selected class:
   - **Default/Unassigned Baseline**: Max HP `+10`, Max Mana `+5`, Attack `+2`, Defense `+1`.
   - **Warrior Overrides**: Max HP `+15`, Attack `+3`, Defense `+2`.
   - **Mage Overrides**: Max Mana `+15`, Magic Power `+3`.
   - **Rogue Overrides**: Crit Chance `+2`, Speed `+2`.
   - **Paladin Overrides**: Max HP `+12`, Max Mana `+8`, Defense `+2`.

## 4. Gold Drops & Economy

1. **Base Enemy Yield**:  
   $$KillGold = Enemy.GoldReward + Random(-2, 5)$$

2. **Scaling Modifications**:  
   - **Easy Mode**: $TotalGold \times 1.5$
   - **Normal Mode**: $TotalGold \times 1.0$
   - **Hard Mode**: $TotalGold \times 0.75$  
   _(Negative gold generations are caught and zeroed)._

## 5. Enemy Difficulty & Stat Adjustment
Enemy profiles are parsed at normal baseline configurations, but adjusted dynamically going into combat.
- **Easy Mode (1)**: 
  - HP: $BaseHP \times 0.85$
  - Attack: $BaseAttack \times 0.70$
  - Defense: $BaseDefense \times 0.70$
- **Hard Mode (3)**: 
  - HP: $BaseHP \times 1.10$
  - Attack: $BaseAttack \times 1.15$
  - Defense: $BaseDefense \times 1.10$

## 6. Crafting & Items

### Crafting Success Probability
Every recipe contains a base success factor. Failing to craft destroys the materials unless specifically protected. Over-leveling an item adds a bonus margin to safety.

$$CraftSuccessChance (\%) = Recipe.BaseChance + [ (PlayerLevel - RequiredLevel) \times 2\% ]$$  
_Constraints:_ Capped at 100%.

### Item Overrides (Effective Stats)
During all calculations throughout the engine, a unit's `"Effective Stats"` represent their raw base characteristics added mathematically via items equipped and status effects applied.
- Ex: `Effective_Speed = Base_Speed + Armor.SpeedBonus + StatusEffect.SpeedMod`

## 7. Event Modifiers
Certain map rooms introduce randomized variables interacting instantly with the player character pool (without combat):
- **Healing Nodes/Shrines**: Recovers `Random(15, 35)` Hit Points.
- **Random Gold Finds**: Typical floors `Random(10, 50)` | Chest Rooms `Random(20, 75)`
- **Hidden Trap Encounters**: Base Trap applies `Random(10, 20)` Physical Damage.
- **Room Ambushes**: Unmitigated surprise damage of `Random(5, 15)`.