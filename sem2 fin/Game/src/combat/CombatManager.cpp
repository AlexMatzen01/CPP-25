#include "combat/CombatManager.h"

#include <algorithm>
#include <sstream>

namespace aq {

void CombatManager::StartEncounter(const Enemy& enemy) {
    enemy_ = enemy;
    enemy_.Reset();
    active_ = true;
    comboMeter_ = 0;
    defending_ = false;
    lastEvent_ = "A " + enemy_.Name() + " appears!";
}

bool CombatManager::IsActive() const {
    return active_;
}

const Enemy& CombatManager::CurrentEnemy() const {
    return enemy_;
}

int CombatManager::ComboMeter() const {
    return comboMeter_;
}

bool CombatManager::IsPlayerDefending() const {
    return defending_;
}

const std::string& CombatManager::LastEvent() const {
    return lastEvent_;
}

CombatManager::TurnResult CombatManager::ResolvePlayerAction(ActionType action, Player& player) {
    TurnResult result;
    if (!active_) {
        result.ongoing = false;
        return result;
    }

    std::ostringstream event;

    int hitChance = 0;
    int damage = 0;

    if (action == ActionType::QuickAttack) {
        hitChance = 85;
        damage = GetRandomValue(8, 14);
        comboMeter_ += 1;
    } else if (action == ActionType::HeavyAttack) {
        hitChance = 65;
        damage = GetRandomValue(14, 24);
        comboMeter_ += 2;
        if (comboMeter_ >= 4) {
            damage += 10;
            comboMeter_ = 0;
            event << "Combo finisher! ";
        }
    } else {
        defending_ = true;
        event << "You brace for impact. ";
    }

    if (action != ActionType::Defend) {
        const int roll = GetRandomValue(1, 100);
        if (roll <= hitChance) {
            const bool critical = GetRandomValue(1, 100) <= 15;
            if (critical) {
                damage *= 2;
                event << "Critical hit! ";
            }

            enemy_.TakeDamage(damage);
            event << "You deal " << damage << " damage.";
        } else {
            event << "Your attack misses.";
        }
    }

    if (enemy_.IsDefeated()) {
        active_ = false;
        result.ongoing = false;
        result.playerWon = true;
        lastEvent_ = event.str() + " Enemy defeated.";
        return result;
    }

    const int enemyHitRoll = GetRandomValue(1, 100);
    if (enemyHitRoll <= 18) {
        event << " The " << enemy_.Name() << " misses.";
    } else {
        int enemyDamage = enemy_.RollDamage();
        if (defending_) {
            enemyDamage = std::max(1, enemyDamage / 2);
        }
        player.TakeDamage(enemyDamage);
        event << " " << enemy_.Name() << " hits you for " << enemyDamage << ".";
    }

    defending_ = false;

    if (player.Health() <= 0) {
        active_ = false;
        result.ongoing = false;
        result.playerDefeated = true;
        event << " You collapse.";
    }

    lastEvent_ = event.str();
    return result;
}

}  // namespace aq
