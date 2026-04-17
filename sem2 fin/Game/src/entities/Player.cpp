#include "entities/Player.h"

#include <algorithm>
#include <sstream>

namespace aq {

void Player::SetName(const std::string& name) {
    name_ = name;
}

const std::string& Player::Name() const {
    return name_;
}

void Player::ResetRoomPosition() {
    position_ = {0.0f, 1.0f, 0.0f};
}

void Player::MoveWithinRoom(const Vector3& delta, float roomHalfExtent) {
    position_.x = std::clamp(position_.x + delta.x, -(roomHalfExtent - 1.0f), roomHalfExtent - 1.0f);
    position_.z = std::clamp(position_.z + delta.z, -(roomHalfExtent - 1.0f), roomHalfExtent - 1.0f);
}

const Vector3& Player::Position() const {
    return position_;
}

int Player::Health() const {
    return health_;
}

int Player::MaxHealth() const {
    return maxHealth_;
}

void Player::SetHealth(int health) {
    health_ = std::clamp(health, 0, maxHealth_);
}

void Player::TakeDamage(int amount) {
    health_ = std::max(0, health_ - amount);
}

void Player::Heal(int amount) {
    health_ = std::min(maxHealth_, health_ + amount);
}

void Player::AddItem(const std::string& itemName) {
    if (!HasItem(itemName)) {
        inventory_.push_back(itemName);
    }
}

bool Player::HasItem(const std::string& itemName) const {
    return std::find(inventory_.begin(), inventory_.end(), itemName) != inventory_.end();
}

std::string Player::InventorySummary() const {
    std::ostringstream oss;
    for (size_t i = 0; i < inventory_.size(); ++i) {
        oss << inventory_[i];
        if (i + 1 < inventory_.size()) {
            oss << ", ";
        }
    }

    return oss.str();
}

}  // namespace aq
