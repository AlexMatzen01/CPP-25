#include "entities/Enemy.h"

#include <algorithm>

namespace aq {

Enemy::Enemy(std::string name, int maxHealth, int minDamage, int maxDamage, Color tint)
    : name_(std::move(name)),
      health_(maxHealth),
      maxHealth_(maxHealth),
      minDamage_(minDamage),
      maxDamage_(maxDamage),
      tint_(tint) {}

const std::string& Enemy::Name() const {
    return name_;
}

int Enemy::Health() const {
    return health_;
}

int Enemy::MaxHealth() const {
    return maxHealth_;
}

int Enemy::MinDamage() const {
    return minDamage_;
}

int Enemy::MaxDamage() const {
    return maxDamage_;
}

Color Enemy::Tint() const {
    return tint_;
}

void Enemy::Reset() {
    health_ = maxHealth_;
}

void Enemy::TakeDamage(int amount) {
    health_ = std::max(0, health_ - amount);
}

bool Enemy::IsDefeated() const {
    return health_ <= 0;
}

int Enemy::RollDamage() const {
    return GetRandomValue(minDamage_, maxDamage_);
}

}  // namespace aq
