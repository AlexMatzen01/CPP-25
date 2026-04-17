#pragma once

#include "combat/CombatManager.h"
#include "entities/Player.h"
#include "systems/AssetManager.h"
#include "systems/SaveManager.h"
#include "util/Logger.h"
#include "world/WorldMap.h"

#include <string>

#include "raylib.h"

namespace aq {

enum class GameState { Boot, MainMenu, Exploration, Combat, Pause, GameOver, Victory };

class GameEngine {
  public:
    GameEngine() = default;
    ~GameEngine();

    bool Initialize();
    void Run();
    void Shutdown();

  private:
    void Update(float dt);
    void Draw();

    void UpdateMainMenu();
    void UpdateExploration(float dt);
    void UpdateCombat();
    void UpdatePause();
    void UpdateEndScreens();

    void DrawMainMenu() const;
    void DrawExploration() const;
    void DrawCombat() const;
    void DrawPause() const;
    void DrawGameOver() const;
    void DrawVictory() const;

    void DrawMiniMap(int originX, int originY, int cellSize) const;
    void DrawRoomPortals(const Room& room) const;

    void ParseAndExecuteCommand(const std::string& command);
    void TryMoveRoom(Direction direction, bool fromCommand);
    void ResetRun(uint32_t forcedSeed = 0);

    static std::string NormalizeCommand(const std::string& raw);

    bool initialized_ = false;
    bool running_ = false;
    GameState state_ = GameState::Boot;

    int screenWidth_ = 1280;
    int screenHeight_ = 720;

    Camera3D camera_{};

    WorldMap world_;
    Player player_;
    CombatManager combat_;
    AssetManager assets_;

    std::string commandBuffer_;
    std::string statusMessage_ = "Press ENTER to begin.";
    bool showControls_ = true;
    bool cryptEncounterCleared_ = false;
    std::string manifestVersion_ = "unversioned";
};

}  // namespace aq
