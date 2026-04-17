#pragma once

#include "entities/Enemy.h"
#include "entities/Player.h"

#include <string>

namespace aq {

class CombatManager {
  public:
    enum class ActionType { QuickAttack, HeavyAttack, Defend };

    struct TurnResult {
        bool ongoing = true;
        bool playerWon = false;
        bool playerDefeated = false;
    };

    void StartEncounter(const Enemy& enemy);
    bool IsActive() const;

    const Enemy& CurrentEnemy() const;
    int ComboMeter() const;
    bool IsPlayerDefending() const;
    const std::string& LastEvent() const;

    TurnResult ResolvePlayerAction(ActionType action, Player& player);

  private:
    Enemy enemy_{};
    bool active_ = false;
    int comboMeter_ = 0;
    bool defending_ = false;
    std::string lastEvent_;
};

}  // namespace aq
