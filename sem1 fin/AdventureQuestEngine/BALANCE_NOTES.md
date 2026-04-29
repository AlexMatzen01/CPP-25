# Combat & Item Balance Adjustments

## Date: April 29, 2026

### Summary
Comprehensive rebalancing of combat and item statistics to improve gameplay balance, class diversity, and progression scaling.

---

## 1. CLASS STAT REBALANCING

### Philosophy
Each class now has clear strengths, weaknesses, and opportunity costs to prevent any single class from being strictly superior.

### Changes by Class

#### Warrior
- **HP**: 200 → 210 (slightly better tank)
- **ATK**: 22 → 20 (reduced from overpower position)
- **DEF**: 14 → 16 (improved defense)
- **Overall**: Maintained as solid balanced melee fighter

#### Mage
- **HP**: 120 → 110 (higher glass cannon risk)
- **ATK**: 10 → 9 (reduced physical damage)
- **DEF**: 8 → 6 (more fragile)
- **MAGIC**: 35 → 38 (increased magic power)
- **SPEED**: 12 → 14 (added mobility)
- **Overall**: True glass cannon with speed bonus, higher magic scaling

#### Rogue
- **HP**: 150 → 140 (more fragile)
- **ATK**: 18 → 16 (reduced base damage)
- **CRIT**: 28 → 32 (enhanced critical focus)
- **DODGE**: 25 → 28 (better evasion)
- **SPEED**: 18 → 20 (improved mobility)
- **Overall**: Pure precision specialist - hit hard when you hit, dodge attacks

#### Paladin
- **HP**: 180 → 200 (improved tankiness)
- **ATK**: 18 → 16 (reduced pure damage)
- **DEF**: 16 → 20 (highest defense in game)
- **MAGIC**: 22 → 28 (better healing/support)
- **Overall**: Defensive tank with support/healing capabilities

#### Ranger
- **HP**: 160 → 155 (slightly more fragile)
- **ATK**: 20 → 19 (adjusted balance)
- **CRIT**: 25 → 28 (better ranged crit)
- **SPEED**: 20 → 24 (fastest non-Monk class)
- **Overall**: Speed-based ranged attacker with high mobility

#### Necromancer
- **HP**: 130 → 135 (slightly more durable)
- **MAGIC**: 32 → 36 (increased magic power)
- **Overall**: Magic specialist with better scaling

#### Barbarian
- **HP**: 250 → 240 (reduced extreme HP pool)
- **ATK**: 28 → 26 (adjusted from overpowered)
- **DEF**: 6 → 10 (improved survivability while maintaining risk/reward)
- **Overall**: High risk/high reward - powerful offense but weak defense

#### Monk
- **HP**: 170 → 165 (adjusted balance)
- **SPEED**: 22 → 25 (fastest class)
- **DODGE**: 25 → 28 (highest evasion)
- **Overall**: Evasion master with incredible speed

---

## 2. ITEM STAT BONUSES

### Issue Fixed
Items previously had **all stat bonuses set to 0**, making equipment essentially cosmetic. This has been corrected with scaling bonuses.

### Bonus Structure

#### Common Items
- Iron Sword: +3 ATK
- Leather Armor: +2 DEF, +2 DODGE

#### Uncommon Items
- Battle Axe: +7 ATK
- Chainmail: +6 DEF, +1 DODGE

#### Rare Items
- Arcane Staff: +4 ATK, +2 CRIT
- Plate Armor: +12 DEF, -1 DODGE (trade-off)

#### Legendary Items
- Dragonslayer: +15 ATK, +3 CRIT
- Frostmourne: +16 ATK, +5 CRIT
- Demonsteel Armor: +2 ATK, +18 DEF

### Scaling Philosophy
- Rarity directly correlates with bonus strength
- Armor items have trade-offs (high DEF reduces DODGE)
- Weapon bonuses increase with rarity
- Total bonuses scale to make late-game equipment feel impactful

---

## 3. CONSUMABLE HEALING VALUE UPDATES

### Potion Healing Values
- **Minor Health Potion**: 25 HP → 30 HP
- **Health Potion**: 50 HP → 60 HP
- **Greater Health Potion**: 100 HP → 120 HP

### Scaling Logic
- Early game (Level 1-15): Common/Uncommon potions adequate
- Mid game (Level 15-40): Rare potions scale with player HP
- Late game (Level 40+): Epic/Legendary potions provide meaningful value

---

## 4. DIFFICULTY MULTIPLIER IMPROVEMENTS

### Previous System
- Easy: 0.7x ATK, 0.7x DEF, 0.85x HP
- Hard: 1.15x ATK, 1.1x DEF, 1.1x HP

### New System (Constants.h)
```cpp
// Easy Mode
EASY_DAMAGE_MULT = 0.75f
EASY_DEFENSE_MULT = 0.80f
EASY_HP_MULT = 0.90f

// Hard Mode
HARD_DAMAGE_MULT = 1.25f  // Increased for more challenge
HARD_DEFENSE_MULT = 1.15f
HARD_HP_MULT = 1.20f      // Enemies are noticeably harder
```

### Changes Made
- **Easy**: Slightly improved defense reduction (0.7 → 0.75 ATK) for better player experience
- **Hard**: Increased damage multiplier (1.15 → 1.25) and HP multiplier (1.1 → 1.2) for true difficulty
- **All difficulties**: Added separate defense multiplier for more nuanced challenge

---

## 5. COMBAT CALCULATION ADJUSTMENTS

### Defense Mitigation Formula
- Remains: `mitigation = defense / (defense + 72)`, capped at 65%
- Formula ensures defense scaling maintains value at all levels
- Cap prevents over-mitigation in late game

### Critical Hit Damage
- Base multiplier: 2x damage
- Added constant `BASE_CRIT_BONUS_MULTIPLIER = 1.25f` for potential future scaling

### Status Effects
- Vulnerability damage bonus calculation improved
- Shield absorption mechanics preserved

---

## 6. BALANCE METRICS

### Attack Power Comparisons (Base Stats)
- Barbarian: 26 ATK (highest damage potential)
- Warrior: 20 ATK (balanced)
- Ranger: 19 ATK (speed compensates)
- Rogue: 16 ATK (crit/dodge compensate)
- Paladin: 16 ATK (defense compensates)
- Necromancer: 11 ATK (magic scaling)
- Mage: 9 ATK (pure magic focus)

### Defense Comparisons
- Paladin: 20 DEF (tank specialist)
- Plate Armor: +12 DEF (endgame defense option)
- Monk/Rogue: High DODGE as alternative defense

### Speed Comparisons
- Monk: 25 SPEED (fastest for turn order)
- Ranger: 24 SPEED (high mobility)
- Rogue: 20 SPEED (balanced mobility)
- Standard: 8-14 SPEED for other classes

---

## 7. RECOMMENDATIONS FOR FUTURE BALANCE UPDATES

1. **Monitor player win rates** by class to identify over/underpowered classes
2. **Adjust skill cooldowns** if certain abilities dominate combat
3. **Consider spell scaling** - current magic damage may need tuning
4. **Review boss enemy stats** - ensure they scale appropriately with player progression
5. **Test high-level gear combinations** - legendary item stacking may create unintended power spikes
6. **Track player feedback** on class identity and feel during combat

---

## 8. TESTING CHECKLIST

- [ ] Create new character of each class and verify starting stats
- [ ] Equip items and verify stat bonuses are applied
- [ ] Test combat on Easy/Normal/Hard difficulties
- [ ] Verify enemy scaling with difficulty modifiers
- [ ] Test potion healing values scale appropriately
- [ ] Verify critical hits do proper damage (2x base)
- [ ] Test equipment swaps mid-game
- [ ] Verify late-game gear feels impactful

---

## Files Modified

1. `include/Constants.h`
   - Updated class base stats
   - Adjusted difficulty multipliers
   - Added new constants for combat scaling

2. `data/items.json`
   - Added stat bonuses to all weapons and armor
   - Updated potion healing values
   - Verified rarity-based scaling

3. `src/Combat.cpp`
   - Applied new difficulty multiplier constants
   - Maintained existing damage formulas

---

## Version History

- **v1.0** (April 29, 2026): Initial comprehensive rebalance
  - Class stat overhaul
  - Item bonus implementation
  - Difficulty scaling improvements
