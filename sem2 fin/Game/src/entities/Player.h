#pragma once

#include "raylib.h"

#include <string>
#include <vector>

namespace aq {

class Player {
  public:
    void SetName(const std::string& name);
    const std::string& Name() const;

    void ResetRoomPosition();
    void MoveWithinRoom(const Vector3& delta, float roomHalfExtent);

    const Vector3& Position() const;

    int Health() const;
    int MaxHealth() const;
    void SetHealth(int health);
    void TakeDamage(int amount);
    void Heal(int amount);

    void AddItem(const std::string& itemName);
    bool HasItem(const std::string& itemName) const;
    std::string InventorySummary() const;

  private:
    std::string name_ = "Hero";
    Vector3 position_{0.0f, 1.0f, 0.0f};
    int health_ = 100;
    int maxHealth_ = 100;
    std::vector<std::string> inventory_{"Traveler's Journal"};
};

}  // namespace aq
