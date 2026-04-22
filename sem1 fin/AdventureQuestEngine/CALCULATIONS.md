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
   $$BaseDamage = AttackerAttack + Random(-1, 3) + 1$$  
   _Variance is tighter to reduce high-roll spikes while keeping attacks punchy._

3. **Defense Mitigation Ratio**:  
   $$Mitigation = \frac{TargetDefense}{TargetDefense + 72.0}$$  
   _Constraints_: Bounded between 0% (0.0) and 65% (0.65). Defense provides diminishing returns scaling off the constant `72`.

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
Crafting now uses layered progression (player level, crafting rank, recipe mastery, station mastery, and pity from failure streak).

$$
\begin{aligned}
CraftSuccessChance &= Recipe.BaseChance \\
&+ 0.02 \cdot \max(0, PlayerLevel - RequiredLevel) \\
&- 0.03 \cdot \max(0, RequiredLevel - PlayerLevel) \\
&+ 0.012 \cdot CraftSkillLevel \\
&+ 0.015 \cdot RecipeMastery \\
&+ 0.004 \cdot StationMastery \\
&+ 0.03 \cdot FailureStreak
\end{aligned}
$$

_Constraints:_ Minimum 10%, maximum 99.5%.

### Material Preservation on Failed Craft
Failed attempts can partially refund materials (50% of each input, rounded up) based on progression.

$$
PreserveChance(\%) = \min\big(55,\; 4 + CraftSkillLevel + 2 \cdot RecipeMastery + 2 \cdot FailureStreak + \max(0, PlayerLevel-RequiredLevel)\big)
$$

### Masterwork Outcome
Successful crafts can roll a masterwork result that boosts rarity and item level, with better odds for specialized crafters.

$$
MasterworkChance(\%) = \min\big(40,\; 3 + CraftSkillLevel + 2 \cdot RecipeMastery + 0.5 \cdot StationMastery\big)
$$

Masterwork effect summary:
- Promotes rarity by 1 tier (up to Legendary).
- Adds +2 item level (capped by normal item scaling caps).
- Adds +50% value.
- Consumables gain +1 extra output quantity.

### Crafting Skill Progression
Crafting has an independent skill track that advances from craft-generated XP.

$$XPToNextCraftLevel = 120 + 55 \cdot (CraftSkillLevel - 1)$$

Craft ranks are title-based by level (Novice -> Apprentice -> Journeyman -> Artisan -> Master -> Grandmaster).

### Batch Crafting Capacity
Maximum craft batch is constrained by both gold and materials.

$$
MaxBatch = \min\left(\left\lfloor\frac{Gold}{RecipeGoldCost}\right\rfloor,\;\min_i\left\lfloor\frac{Material_i}{RequiredMaterial_i}\right\rfloor\right)
$$

### Crafted Item Level Scaling
Crafted item level is now dynamic and scales from player progression at time of craft.

$$
CraftedItemLevel = clamp\Big(\max(RequiredLevel,\; RollItemLevel(PlayerLevel, Rarity) + CraftSkillBonus + RecipeMasteryBonus),\; 1,\; 60\Big)
$$

Implementation notes:
- `RollItemLevel` introduces small controlled variance around current player level.
- `CraftSkillBonus` and `RecipeMasteryBonus` provide upward pressure as crafting specialization grows.
- Maw recipes use boss-style scaling context for stronger endgame rolls.

### Item Overrides (Effective Stats)
During all calculations throughout the engine, a unit's `"Effective Stats"` represent their raw base characteristics added mathematically via items equipped and status effects applied.
- Ex: `Effective_Speed = Base_Speed + Armor.SpeedBonus + StatusEffect.SpeedMod`

## 7. Event Modifiers
Certain map rooms introduce randomized variables interacting instantly with the player character pool (without combat):
- **Healing Nodes/Shrines**: Recovers `Random(15, 35)` Hit Points.
- **Random Gold Finds**: Typical floors `Random(10, 50)` | Chest Rooms `Random(20, 75)`
- **Hidden Trap Encounters**: Base Trap applies `Random(10, 20)` Physical Damage.
- **Room Ambushes**: Unmitigated surprise damage of `Random(5, 15)`.

## 8. The Maw (Infinite Map Mode)

### Separate Maw Map Generation
The Maw is generated as an independent stratum graph and no longer reuses a simple linear floor loop.

$$
RoomsPerStratum = 8 + \min\left(10, \left\lfloor\frac{StratumDepth}{2}\right\rfloor\right)
$$

Each stratum guarantees:
- `1` Entry room
- `1` Descent room (to generate next stratum)
- random specialized rooms (Altar, Forge, Rift, Sanctum, Cache)
- guaranteed connectivity via base chain + extra randomized links

$$
ExtraLinks = 2 + \min\left(8, \left\lfloor\frac{StratumDepth}{2}\right\rfloor\right)
$$

### Maw Instability Pressure
Instability grows as the player moves and in Rift rooms. At high values it inflicts ambient true damage.

Movement instability increment:

$$
\Delta Instability = 1 + \left\lfloor\frac{StratumDepth}{3}\right\rfloor
$$

Ambient true damage when instability is high:

$$
AmbientTrueDamage = \max\left(1, \left\lfloor\frac{Instability}{4}\right\rfloor + \left\lfloor\frac{StratumDepth}{10}\right\rfloor\right),\;\;Instability \ge 8
$$

### Maw-Exclusive Features
- **Echo Altar:** Choose healing, instability reduction, or insight XP + recipe discovery chance.
- **Abyss Forge:** Access full crafting while inside Maw and gain Maw materials.
- **Ruin Cache:** Non-combat salvage node for materials and gold.
- **Sovereign Sanctum:** Elevated combat difficulty/reward node.
- **Waygate Extraction:** Exit to Portal Room from any explored point without ending your run.