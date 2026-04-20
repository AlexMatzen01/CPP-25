#include "include/Types.h"
#include "include/json.hpp"
#include "include/FileIO.h"

using json = nlohmann::json;

namespace AQE {
    NLOHMANN_JSON_SERIALIZE_ENUM(ItemType, {
        {ItemType::CONSUMABLE, "CONSUMABLE"},
        {ItemType::WEAPON, "WEAPON"},
        {ItemType::ARMOR, "ARMOR"},
        {ItemType::ACCESSORY, "ACCESSORY"},
        {ItemType::KEY, "KEY"},
        {ItemType::QUEST, "QUEST"},
        {ItemType::SPECIAL, "SPECIAL"}
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(ItemRarity, {
        {ItemRarity::COMMON, "COMMON"},
        {ItemRarity::UNCOMMON, "UNCOMMON"},
        {ItemRarity::RARE, "RARE"},
        {ItemRarity::EPIC, "EPIC"},
        {ItemRarity::LEGENDARY, "LEGENDARY"}
    })

    void from_json(const json& j, Item& i) {
        j.at("id").get_to(i.id);
        j.at("name").get_to(i.name);
        j.at("description").get_to(i.description);
        j.at("type").get_to(i.type);
        j.at("rarity").get_to(i.rarity);
        j.at("value").get_to(i.value);
        if (j.contains("quantity")) j.at("quantity").get_to(i.quantity);
        if (j.contains("healAmount")) j.at("healAmount").get_to(i.healAmount);
        if (j.contains("attackBonus")) j.at("attackBonus").get_to(i.attackBonus);
        if (j.contains("defenseBonus")) j.at("defenseBonus").get_to(i.defenseBonus);
        if (j.contains("critBonus")) j.at("critBonus").get_to(i.critBonus);
        if (j.contains("dodgeBonus")) j.at("dodgeBonus").get_to(i.dodgeBonus);
        if (j.contains("speedBonus")) j.at("speedBonus").get_to(i.speedBonus);
        if (j.contains("duration")) j.at("duration").get_to(i.duration);
        if (j.contains("questId")) j.at("questId").get_to(i.questId);
        if (j.contains("isEquipped")) j.at("isEquipped").get_to(i.isEquipped);
        if (j.contains("isUsable")) j.at("isUsable").get_to(i.isUsable);
        if (j.contains("isStackable")) j.at("isStackable").get_to(i.isStackable);
        if (j.contains("durability")) j.at("durability").get_to(i.durability);
        if (j.contains("maxDurability")) j.at("maxDurability").get_to(i.maxDurability);
        if (j.contains("upgradeLevel")) j.at("upgradeLevel").get_to(i.upgradeLevel);
        if (j.contains("isEnchanted")) j.at("isEnchanted").get_to(i.isEnchanted);
        if (j.contains("enchantmentName")) j.at("enchantmentName").get_to(i.enchantmentName);
    }
    
    void from_json(const json& j, StatusEffect& s) {
        j.at("name").get_to(s.name);
        j.at("description").get_to(s.description);
        j.at("attackMod").get_to(s.attackMod);
        j.at("defenseMod").get_to(s.defenseMod);
        j.at("critMod").get_to(s.critMod);
        j.at("dodgeMod").get_to(s.dodgeMod);
        j.at("speedMod").get_to(s.speedMod);
        j.at("turnsRemaining").get_to(s.turnsRemaining);
        j.at("isDebuff").get_to(s.isDebuff);
    }
    
    NLOHMANN_JSON_SERIALIZE_ENUM(EnemyIntent, {
        {EnemyIntent::UNKNOWN, "UNKNOWN"},
        {EnemyIntent::ATTACK_LIGHT, "ATTACK_LIGHT"},
        {EnemyIntent::ATTACK_HEAVY, "ATTACK_HEAVY"},
        {EnemyIntent::DEFEND, "DEFEND"},
        {EnemyIntent::DEBUFF, "DEBUFF"},
        {EnemyIntent::BUFF, "BUFF"},
        {EnemyIntent::HEAL, "HEAL"},
        {EnemyIntent::CONTROL, "CONTROL"},
        {EnemyIntent::SUMMON, "SUMMON"},
        {EnemyIntent::CHARGE, "CHARGE"}
    })

    void from_json(const json& j, CombatStats& c) {
        j.at("maxHP").get_to(c.maxHP);
        j.at("currentHP").get_to(c.currentHP);
        j.at("maxMana").get_to(c.maxMana);
        j.at("currentMana").get_to(c.currentMana);
        j.at("attack").get_to(c.attack);
        j.at("defense").get_to(c.defense);
        j.at("magicPower").get_to(c.magicPower);
        j.at("speed").get_to(c.speed);
        j.at("critChance").get_to(c.critChance);
        j.at("dodgeChance").get_to(c.dodgeChance);
    }

    void from_json(const json& j, EnemyAbility& a) {
        j.at("name").get_to(a.name);
        j.at("description").get_to(a.description);
        j.at("damage").get_to(a.damage);
        j.at("cooldown").get_to(a.cooldown);
        if (j.contains("currentCooldown")) j.at("currentCooldown").get_to(a.currentCooldown);
        j.at("triggerChance").get_to(a.triggerChance);
        j.at("heals").get_to(a.heals);
        j.at("healAmount").get_to(a.healAmount);
    }

    void from_json(const json& j, Enemy& e) {
        j.at("id").get_to(e.id);
        j.at("name").get_to(e.name);
        j.at("description").get_to(e.description);
        if (j.contains("asciiArt")) j.at("asciiArt").get_to(e.asciiArt);
        j.at("stats").get_to(e.stats);
        if (j.contains("abilities")) j.at("abilities").get_to(e.abilities);
        if (j.contains("experienceReward")) j.at("experienceReward").get_to(e.experienceReward);
        if (j.contains("goldReward")) j.at("goldReward").get_to(e.goldReward);
        if (j.contains("lootTable")) j.at("lootTable").get_to(e.lootTable);
        if (j.contains("lootChances")) j.at("lootChances").get_to(e.lootChances);
        if (j.contains("isBoss")) j.at("isBoss").get_to(e.isBoss);
        if (j.contains("dialogue")) j.at("dialogue").get_to(e.dialogue);
        if (j.contains("activeEffects")) j.at("activeEffects").get_to(e.activeEffects);
        if (j.contains("nextIntent")) j.at("nextIntent").get_to(e.nextIntent);
        if (j.contains("intentStrength")) j.at("intentStrength").get_to(e.intentStrength);
    }
}
