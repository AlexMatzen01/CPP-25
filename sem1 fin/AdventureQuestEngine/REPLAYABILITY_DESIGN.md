# Adventure Quest Engine: Replayability and Endgame Features

This document outlines design concepts and features intended to significantly increase the playability, longevity, and replay continuously of the Adventure Quest Engine.

## 1. Infinite Procedural Dungeons (Endless Mode)
Currently, the game supports generated maps, but introducing an **Endless Mode** will push players to survive as long as possible.
*   **Procedural Scaling:** As the player descends deeper into the dungeon (Floor 10, Floor 20, etc.), enemy stats (HP, Attack, Defense) scale infinitely using a multiplier formula.
*   **Floor Modifiers:** Every 5th floor introduces a permanent modifier to that floor (e.g., "Toxic Atmosphere" - lose 1 HP per turn, "Anti-Magic Zone" - mana costs doubled).
*   **Leaderboard Integration:** Score is calculated based on the maximum floor reached, enemies defeated, and turns survived, storing the data securely in `data/leaderboard.txt`.

## 2. New Game Plus (NG+) and Challenge Runs
Building upon the `NewGamePlusModifiers` structure within the game, players who complete the main campaign can loop their save file with increased difficulty.
*   **Retain Progression:** Players keep their character level, gold, and optionally inventory items and skill tree talents.
*   **Cursed Items & Traits:** The NG+ mode introduces corrupted items that offer massive stat boosts but come with severe drawbacks (e.g., "Cursed Blade": +50 Attack, but you cannot be healed by potions).
*   **Challenge Flags:** Support for optional challenges: *Permadeath*, *Time Limit*, *Randomized Classes* on floor change, and *No Healing*. 

## 3. Town/Hub Meta-Progression
To reward players even when they fail a run, introduce a permanent hub world that exists outside the dungeon.
*   **Meta Currency:** Introduce "Soul Shards" or "Ancient Coins" that drop from bosses and carry over between deaths.
*   **Upgradable NPCs:** 
    *   *The Blacksmith:* Unlocks better starting weapons for future runs.
    *   *The Alchemist:* Permanently increases the potency of health potions.
    *   *The Trainer:* Grants a small, permanent XP boost modifier across all new characters.

## 4. Advanced Crafting & Enchanting System
Giving players a reason to farm enemies and hunt for specific biomes.
*   **Material Harvesting:** Enemies drop specific crafting materials depending on their type (e.g., Goblin Ears, Dragon Scales) stored in the newly implemented `items.json`.
*   **The Forge Room:** Players can merge duplicate items to increase their `upgradeLevel` (from +1 up to +10) or apply magical enchantments that grant lifesteal, elemental damage, or cleave (AoE) effects.

## 5. Dynamic Quests and Bounties
Static quests can become repetitive. We can introduce a dynamic quest board.
*   **Bounty Generation:** "Kill X enemies of type Y" or "Retrieve the Lost Relic from Floor Z."
*   **Merchant Contracts:** The traveling merchant requests specific rare items in exchange for unique Legendary-tier keys or accessories. 

## 6. Boss Rush & Arena Mode
A specialized combat-only mode tailored to test player builds without the exploration phase.
*   **Wave-Based Survival:** Players face increasingly difficult waves of enemies with short shop breaks in between.
*   **Mutators:** Random buffs/debuffs applied every 5 waves, forcing players to adapt their combat strategies using unused spells or items.

## 7. Extensive JSON Modding Support
With the data already migrated to `data/items.json` and `data/enemies.json`, players can effectively create their own "Expansions".
*   **Custom Campaigns:** Modders can add new `RoomType` logic, custom `StatusEffect` abilities, and unique ASCII art without recompiling the C++ codebase.
*   **Community Sharing:** Simple JSON sharing allows players to trade dungeon seeds and enemy packs, vastly improving community longevity.

## 8. "The Maw" - The Endless Death Realm
To provide an ultimate endgame combat test and high-stakes exploration, introduce a secondary realm known as "The Maw".
*   **Unlock Condition:** Defeating the current final boss, the "Shadow Lord".
*   **Voluntary Access Point:** Upon the Shadow Lord's defeat, the player is not immediately thrust into this zone. Instead, a new location known as the "Portal Room" generates on the map. The player must actively choose to step through the anomaly portal to enter.
*   **Punishing Difficulty:** Enemies in The Maw boast drastically increased stats, unique intents, and deal unmitigable damage, making it a true survival challenge.
*   **Unrivaled Gear:** Exclusively houses the highest tier of gear (Legendary/Mythic) with unique enchantments, encouraging players to risk deeper dives in exchange for incredible loot.

## 9. Dynamic Item Level System
To ensure gear remains relevant throughout endless scaling and "The Maw", an Item Level (iLvl) system will be integrated.
*   **Scaling Drops:** Items dropped by enemies or found in chests will organically scale their base stats (attack, defense, heal amount) based on the current floor or the level of the defeated enemy.
*   **iLvl Tiers:** High iLvl gear can naturally roll with additional attribute bonuses or unique perks, giving players a long-term goal of hunting for "perfect rolls".
*   **Reforging:** The Forge Room will accommodate an option to increase an aging legendary item's iLvl to match current content, consuming massive amounts of gold and high-end materials.

## 10. Sub-Dungeons and Instanced Realms
To diversify exploration and provide focused combat scenarios, players can discover and enter completely unique, procedurally generated sub-dungeons.
*   **Dungeon Rooms:** These special entry points can spawn organically across any game mode, including the normal base map. Interacting with the entrance instantly transports the player into a self-contained, instanced dungeon map featuring unique room layouts and themes.
*   **Trash Mobs:** These sub-dungeons are heavily guarded by packs of "trash mobs." While numerous, they carry a low but noticeable chance to drop highly valuable loot, encouraging full clears.
*   **Variable Boss Encounters:** Each sub-dungeon procedurally generates with a randomized number of bosses (1 to 3 per instance), making every run unpredictable in length and difficulty.
*   **Scaled Boss Loot:** Defeating a sub-dungeon boss offers a massive 75% drop rate for high Item Level (iLvl) gear. Crucially, the stats and tier of this gear scale dynamically based on a combination of the player's current character level and their currently equipped average item level.
