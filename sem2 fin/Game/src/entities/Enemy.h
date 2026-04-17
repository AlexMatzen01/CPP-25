#pragma once

#include <string>

#include "raylib.h"

namespace aq {

class Enemy {
  public:
    Enemy() = default;
    Enemy(std::string name, int maxHealth, int minDamage, int maxDamage, Color tint = RED);

    const std::string& Name() const;
    int Health() const;
    int MaxHealth() const;
    int MinDamage() const;
    int MaxDamage() const;
    Color Tint() const;

    void Reset();
    void TakeDamage(int amount);
    bool IsDefeated() const;
    int RollDamage() const;

  private:
    std::string name_ = "Enemy";
    int health_ = 1;
    int maxHealth_ = 1;
    int minDamage_ = 1;
    int maxDamage_ = 2;
    Color tint_ = RED;
};

}  // namespace aq
