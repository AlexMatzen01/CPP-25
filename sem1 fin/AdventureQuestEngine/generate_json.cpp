#include "include/Types.h"
#include "include/json.hpp"
#include "include/FileIO.h"
#include <iostream>
#include <fstream>

using json = nlohmann::json;
using namespace AQE;

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

    void to_json(json& j, const Item& i) {
        j = json{
            {"id", i.id},
            {"name", i.name},
            {"description", i.description},
            {"type", i.type},
            {"rarity", i.rarity},
            {"value", i.value},
            {"quantity", i.quantity},
            {"healAmount", i.healAmount},
            {"attackBonus", i.attackBonus},
            {"defenseBonus", i.defenseBonus},
            {"critBonus", i.critBonus},
            {"dodgeBonus", i.dodgeBonus},
            {"speedBonus", i.speedBonus},
            {"duration", i.duration},
            {"questId", i.questId},
            {"isEquipped", i.isEquipped},
            {"isUsable", i.isUsable},
            {"isStackable", i.isStackable},
            {"durability", i.durability},
            {"maxDurability", i.maxDurability},
            {"upgradeLevel", i.upgradeLevel},
            {"isEnchanted", i.isEnchanted},
            {"enchantmentName", i.enchantmentName}
        };
    }
    
    void to_json(json& j, const StatusEffect& s) {
        j = json{
            {"name", s.name},
            {"description", s.description},
            {"attackMod", s.attackMod},
            {"defenseMod", s.defenseMod},
            {"critMod", s.critMod},
            {"dodgeMod", s.dodgeMod},
            {"speedMod", s.speedMod},
            {"turnsRemaining", s.turnsRemaining},
            {"isDebuff", s.isDebuff}
        };
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

    void to_json(json& j, const CombatStats& c) {
        j = json{
            {"maxHP", c.maxHP}, {"currentHP", c.currentHP},
            {"maxMana", c.maxMana}, {"currentMana", c.currentMana},
            {"attack", c.attack}, {"defense", c.defense},
            {"magicPower", c.magicPower}, {"speed", c.speed},
            {"critChance", c.critChance}, {"dodgeChance", c.dodgeChance}
        };
    }

    void to_json(json& j, const EnemyAbility& a) {
        j = json{
            {"name", a.name}, {"description", a.description},
            {"damage", a.damage}, {"cooldown", a.cooldown},
            {"currentCooldown", a.currentCooldown}, {"triggerChance", a.triggerChance},
            {"heals", a.heals}, {"healAmount", a.healAmount}
        };
    }

    void to_json(json& j, const Enemy& e) {
        j = json{
            {"id", e.id}, {"name", e.name}, {"description", e.description},
            {"asciiArt", e.asciiArt}, {"stats", e.stats},
            {"abilities", e.abilities}, {"experienceReward", e.experienceReward},
            {"goldReward", e.goldReward}, {"lootTable", e.lootTable},
            {"lootChances", e.lootChances}, {"isBoss", e.isBoss},
            {"dialogue", e.dialogue}, {"activeEffects", e.activeEffects},
            {"nextIntent", e.nextIntent}, {"intentStrength", e.intentStrength}
        };
    }
}

int main() {
    auto items = AQE::createDefaultItems();
    json itemsJson = items;
    std::ofstream o1("data/items.json");
    o1 << itemsJson.dump(4);
    
    auto enemies = AQE::createDefaultEnemies();
    json enemiesJson = enemies;
    std::ofstream o2("data/enemies.json");
    o2 << enemiesJson.dump(4);
    
    std::cout << "Done" << std::endl;
    return 0;
}
