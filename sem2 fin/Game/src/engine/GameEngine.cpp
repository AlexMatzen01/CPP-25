#include "engine/GameEngine.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <sstream>

namespace aq {

namespace {
constexpr float kRoomHalfExtent = 10.0f;
constexpr float kPlayerSpeed = 5.0f;
constexpr const char* kSaveFileName = "savegame.txt";
constexpr const char* kAssetManifestPath = "assets/manifest.txt";

std::string ResolveManifestPath() {
    const std::array<const char*, 4> candidates{{
        "assets/manifest.txt",
        "../assets/manifest.txt",
        "../../assets/manifest.txt",
        kAssetManifestPath,
    }};

    for (const char* candidate : candidates) {
        if (FileExists(candidate)) {
            return candidate;
        }
    }

    return kAssetManifestPath;
}

bool TryParseSeed(const std::string& value, uint32_t& outSeed) {
    try {
        const unsigned long parsed = std::stoul(value);
        if (parsed == 0 || parsed > 0xffffffffUL) {
            return false;
        }

        outSeed = static_cast<uint32_t>(parsed);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
}  // namespace

GameEngine::~GameEngine() {
    Shutdown();
}

bool GameEngine::Initialize() {
    if (initialized_) {
        return true;
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth_, screenHeight_, "Adventure Quest 3D - Raylib");

    if (!IsWindowReady()) {
        Logger::Error("Failed to initialize window.");
        return false;
    }

    SetTargetFPS(60);

    camera_.position = {0.0f, 12.0f, 16.0f};
    camera_.target = {0.0f, 0.0f, 0.0f};
    camera_.up = {0.0f, 1.0f, 0.0f};
    camera_.fovy = 55.0f;
    camera_.projection = CAMERA_PERSPECTIVE;

    const std::string manifestPath = ResolveManifestPath();

    std::string manifestError;
    if (!assets_.LoadManifest(manifestPath, manifestError)) {
        Logger::Error(manifestError);
    }

    std::string assetInfo;
    assets_.LoadAvailableTextures(assetInfo);
    if (!assetInfo.empty()) {
        Logger::Info(assetInfo);
    }

    manifestVersion_ = assets_.ManifestVersion();

    world_.GenerateProceduralLayoutAuto();
    player_.ResetRoomPosition();

    state_ = GameState::MainMenu;
    initialized_ = true;
    Logger::Info("GameEngine initialized.");
    return true;
}

void GameEngine::Run() {
    if (!initialized_) {
        Logger::Error("Run called before successful Initialize.");
        return;
    }

    running_ = true;
    while (running_ && !WindowShouldClose()) {
        const float dt = GetFrameTime();
        Update(dt);

        BeginDrawing();
        Draw();
        EndDrawing();
    }
}

void GameEngine::Shutdown() {
    if (!initialized_) {
        return;
    }

    assets_.UnloadAll();
    CloseWindow();
    initialized_ = false;
    running_ = false;
}

void GameEngine::Update(float dt) {
    switch (state_) {
        case GameState::MainMenu:
            UpdateMainMenu();
            break;
        case GameState::Exploration:
            UpdateExploration(dt);
            break;
        case GameState::Combat:
            UpdateCombat();
            break;
        case GameState::Pause:
            UpdatePause();
            break;
        case GameState::GameOver:
        case GameState::Victory:
            UpdateEndScreens();
            break;
        case GameState::Boot:
            state_ = GameState::MainMenu;
            break;
    }
}

void GameEngine::Draw() {
    switch (state_) {
        case GameState::MainMenu:
            DrawMainMenu();
            break;
        case GameState::Exploration:
            DrawExploration();
            break;
        case GameState::Combat:
            DrawCombat();
            break;
        case GameState::Pause:
            DrawPause();
            break;
        case GameState::GameOver:
            DrawGameOver();
            break;
        case GameState::Victory:
            DrawVictory();
            break;
        case GameState::Boot:
            ClearBackground(BLACK);
            break;
    }
}

void GameEngine::UpdateMainMenu() {
    if (IsKeyPressed(KEY_ENTER)) {
        statusMessage_ =
            "Explore the world and recover the Ancient Sigil. Seed " + std::to_string(world_.Seed()) + ".";
        state_ = GameState::Exploration;
    }
}

void GameEngine::UpdateExploration(float dt) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        state_ = GameState::Pause;
        return;
    }

    if (IsKeyPressed(KEY_H)) {
        showControls_ = !showControls_;
    }

    Vector3 moveDelta{0.0f, 0.0f, 0.0f};
    if (IsKeyDown(KEY_W)) {
        moveDelta.z -= kPlayerSpeed * dt;
    }
    if (IsKeyDown(KEY_S)) {
        moveDelta.z += kPlayerSpeed * dt;
    }
    if (IsKeyDown(KEY_A)) {
        moveDelta.x -= kPlayerSpeed * dt;
    }
    if (IsKeyDown(KEY_D)) {
        moveDelta.x += kPlayerSpeed * dt;
    }

    player_.MoveWithinRoom(moveDelta, kRoomHalfExtent);

    if (IsKeyPressed(KEY_UP)) {
        TryMoveRoom(Direction::North, false);
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        TryMoveRoom(Direction::East, false);
    }
    if (IsKeyPressed(KEY_DOWN)) {
        TryMoveRoom(Direction::South, false);
    }
    if (IsKeyPressed(KEY_LEFT)) {
        TryMoveRoom(Direction::West, false);
    }

    for (int key = GetCharPressed(); key > 0; key = GetCharPressed()) {
        if (key >= 32 && key <= 126 && commandBuffer_.size() < 72) {
            commandBuffer_.push_back(static_cast<char>(key));
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !commandBuffer_.empty()) {
        commandBuffer_.pop_back();
    }

    if (IsKeyPressed(KEY_ENTER) && !commandBuffer_.empty()) {
        ParseAndExecuteCommand(commandBuffer_);
        commandBuffer_.clear();
    }

    const Vector3& pos = player_.Position();
    camera_.target = {pos.x, 1.0f, pos.z};
    camera_.position = {pos.x, 11.0f, pos.z + 14.0f};
}

void GameEngine::UpdatePause() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        state_ = GameState::Exploration;
    }
}

void GameEngine::UpdateEndScreens() {
    if (IsKeyPressed(KEY_R)) {
        ResetRun(world_.Seed());
        state_ = GameState::Exploration;
    }

    if (IsKeyPressed(KEY_M)) {
        ResetRun(0);
        state_ = GameState::MainMenu;
    }
}

void GameEngine::DrawMainMenu() const {
    ClearBackground({24, 28, 36, 255});

    DrawText("ADVENTURE QUEST 3D", 60, 80, 52, RAYWHITE);
    DrawText("C++ + Raylib implementation", 64, 146, 24, {194, 214, 228, 255});

    DrawText("Goal: recover the Ancient Sigil and reach Storm Watch.", 64, 238, 28, {228, 224, 181, 255});
    DrawText("Controls: WASD move in room, Arrow keys change rooms.", 64, 278, 24, {194, 214, 228, 255});
    DrawText("Type commands: go north, inventory, status, seed, regen 42, save, load.", 64, 312, 24, {194, 214, 228, 255});

    DrawText("Press ENTER to start", 64, 390, 34, {180, 232, 180, 255});
}

void GameEngine::DrawExploration() const {
    const Room& room = world_.CurrentRoom();

    ClearBackground({28, 34, 41, 255});

    BeginMode3D(camera_);
    const Texture2D* floorTexture = assets_.GetTexture(room.FloorAssetId());
    const Texture2D* wallTexture = assets_.GetTexture(room.WallAssetId());

    DrawPlane({0.0f, 0.0f, 0.0f}, {20.0f, 20.0f}, room.FloorColor());

    const Color wall = room.WallColor();
    DrawCube({0.0f, 5.0f, -kRoomHalfExtent}, 20.0f, 10.0f, 1.0f, wall);
    DrawCube({0.0f, 5.0f, kRoomHalfExtent}, 20.0f, 10.0f, 1.0f, wall);
    DrawCube({-kRoomHalfExtent, 5.0f, 0.0f}, 1.0f, 10.0f, 20.0f, wall);
    DrawCube({kRoomHalfExtent, 5.0f, 0.0f}, 1.0f, 10.0f, 20.0f, wall);

    DrawRoomPortals(room);

    const Vector3& playerPos = player_.Position();
    DrawCube({playerPos.x, 0.9f, playerPos.z}, 0.9f, 1.8f, 0.9f, {84, 164, 255, 255});
    DrawCubeWires({playerPos.x, 0.9f, playerPos.z}, 0.9f, 1.8f, 0.9f, WHITE);

    DrawGrid(20, 1.0f);
    EndMode3D();

    DrawRectangle(0, 0, GetScreenWidth(), 118, Fade(BLACK, 0.35f));
    DrawText(room.Name().c_str(), 20, 16, 34, RAYWHITE);
    DrawText(room.Description().c_str(), 20, 60, 22, {218, 218, 218, 255});
    std::string seedLine = "Seed: " + std::to_string(world_.Seed()) + " | Manifest: " + manifestVersion_;
    DrawText(seedLine.c_str(), 20, 90, 18, {188, 214, 222, 255});

    std::ostringstream hp;
    hp << "HP: " << player_.Health() << "/" << player_.MaxHealth();
    DrawText(hp.str().c_str(), 20, GetScreenHeight() - 86, 26, {238, 186, 186, 255});

    DrawRectangle(0, GetScreenHeight() - 54, GetScreenWidth(), 54, Fade(BLACK, 0.45f));
    std::string prompt = "> " + commandBuffer_;
    DrawText(prompt.c_str(), 18, GetScreenHeight() - 44, 24, {198, 232, 198, 255});

    DrawText(statusMessage_.c_str(), 330, GetScreenHeight() - 44, 22, {235, 235, 196, 255});

    if (floorTexture != nullptr) {
        DrawText("Floor asset", GetScreenWidth() - 184, 182, 18, {220, 220, 220, 255});
        DrawTextureEx(*floorTexture, {(float)GetScreenWidth() - 190.0f, 202.0f}, 0.0f, 0.2f, WHITE);
    }

    if (wallTexture != nullptr) {
        DrawText("Wall asset", GetScreenWidth() - 184, 256, 18, {220, 220, 220, 255});
        DrawTextureEx(*wallTexture, {(float)GetScreenWidth() - 190.0f, 276.0f}, 0.0f, 0.2f, WHITE);
    }

    if (showControls_) {
        DrawRectangle(GetScreenWidth() - 410, 12, 396, 160, Fade(BLACK, 0.5f));
        DrawText("Controls", GetScreenWidth() - 390, 20, 28, RAYWHITE);
        DrawText("WASD: move inside room", GetScreenWidth() - 390, 56, 20, {210, 210, 210, 255});
        DrawText("Arrow keys: move between rooms", GetScreenWidth() - 390, 82, 20, {210, 210, 210, 255});
        DrawText("H: toggle this panel", GetScreenWidth() - 390, 108, 20, {210, 210, 210, 255});
        DrawText("save/load via command line", GetScreenWidth() - 390, 134, 20, {210, 210, 210, 255});
    }

    DrawMiniMap(GetScreenWidth() - 210, GetScreenHeight() - 170, 36);
}

void GameEngine::DrawCombat() const {
    ClearBackground({20, 17, 24, 255});

    Camera3D combatCamera{};
    combatCamera.position = {0.0f, 10.0f, 18.0f};
    combatCamera.target = {0.0f, 1.0f, 0.0f};
    combatCamera.up = {0.0f, 1.0f, 0.0f};
    combatCamera.fovy = 50.0f;
    combatCamera.projection = CAMERA_PERSPECTIVE;

    BeginMode3D(combatCamera);
    DrawPlane({0.0f, 0.0f, 0.0f}, {22.0f, 22.0f}, {64, 58, 72, 255});
    DrawCube({-4.5f, 1.0f, 0.0f}, 1.1f, 2.0f, 1.1f, {84, 164, 255, 255});
    DrawCubeWires({-4.5f, 1.0f, 0.0f}, 1.1f, 2.0f, 1.1f, WHITE);

    if (combat_.IsActive()) {
        const Enemy& enemy = combat_.CurrentEnemy();
        DrawCube({4.5f, 1.2f, 0.0f}, 1.6f, 2.4f, 1.6f, enemy.Tint());
        DrawCubeWires({4.5f, 1.2f, 0.0f}, 1.6f, 2.4f, 1.6f, WHITE);
    }

    DrawGrid(20, 1.0f);
    EndMode3D();

    DrawRectangle(0, 0, GetScreenWidth(), 150, Fade(BLACK, 0.35f));
    DrawText("TACTICAL COMBAT", 20, 20, 42, RAYWHITE);

    if (combat_.IsActive()) {
        const Enemy& enemy = combat_.CurrentEnemy();

        std::ostringstream enemyHp;
        enemyHp << enemy.Name() << " HP: " << enemy.Health() << "/" << enemy.MaxHealth();
        DrawText(enemyHp.str().c_str(), 26, 78, 28, {236, 170, 170, 255});
    }

    std::ostringstream heroHp;
    heroHp << "Hero HP: " << player_.Health() << "/" << player_.MaxHealth() << " | Combo: " << combat_.ComboMeter();
    DrawText(heroHp.str().c_str(), 26, 112, 26, {170, 211, 255, 255});

    DrawRectangle(0, GetScreenHeight() - 94, GetScreenWidth(), 94, Fade(BLACK, 0.45f));
    DrawText("1 Quick Attack   2 Heavy Attack   3 Defend", 20, GetScreenHeight() - 82, 30, {220, 230, 220, 255});
    DrawText(statusMessage_.c_str(), 20, GetScreenHeight() - 46, 24, {240, 221, 170, 255});
}

void GameEngine::UpdateCombat() {
    if (!combat_.IsActive()) {
        state_ = GameState::Exploration;
        return;
    }

    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
        auto result = combat_.ResolvePlayerAction(CombatManager::ActionType::QuickAttack, player_);
        statusMessage_ = combat_.LastEvent();
        if (result.playerWon) {
            cryptEncounterCleared_ = true;
            state_ = GameState::Exploration;
            statusMessage_ = "You defeated the Crypt Warden.";
        } else if (result.playerDefeated) {
            state_ = GameState::GameOver;
            statusMessage_ = "You were slain in combat.";
        }
        return;
    }

    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
        auto result = combat_.ResolvePlayerAction(CombatManager::ActionType::HeavyAttack, player_);
        statusMessage_ = combat_.LastEvent();
        if (result.playerWon) {
            cryptEncounterCleared_ = true;
            state_ = GameState::Exploration;
            statusMessage_ = "You crushed the Crypt Warden with a final blow.";
        } else if (result.playerDefeated) {
            state_ = GameState::GameOver;
            statusMessage_ = "You were slain in combat.";
        }
        return;
    }

    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
        auto result = combat_.ResolvePlayerAction(CombatManager::ActionType::Defend, player_);
        statusMessage_ = combat_.LastEvent();
        if (result.playerWon) {
            cryptEncounterCleared_ = true;
            state_ = GameState::Exploration;
            statusMessage_ = "The Crypt Warden falls.";
        } else if (result.playerDefeated) {
            state_ = GameState::GameOver;
            statusMessage_ = "You were slain in combat.";
        }
    }
}

void GameEngine::DrawPause() const {
    DrawExploration();
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));
    DrawText("PAUSED", GetScreenWidth() / 2 - 80, GetScreenHeight() / 2 - 54, 46, RAYWHITE);
    DrawText("Press ESC to resume", GetScreenWidth() / 2 - 120, GetScreenHeight() / 2 + 8, 28, {220, 220, 220, 255});
}

void GameEngine::DrawGameOver() const {
    ClearBackground({26, 10, 14, 255});
    DrawText("YOU WERE DEFEATED", 64, 120, 62, {237, 136, 136, 255});
    DrawText(statusMessage_.c_str(), 70, 224, 30, {220, 220, 220, 255});
    DrawText("Press R to restart run", 70, 310, 30, {186, 214, 248, 255});
    DrawText("Press M for main menu", 70, 352, 30, {186, 214, 248, 255});
}

void GameEngine::DrawVictory() const {
    ClearBackground({18, 36, 22, 255});
    DrawText("VICTORY", 64, 120, 78, {178, 244, 180, 255});
    DrawText(statusMessage_.c_str(), 70, 232, 30, {220, 220, 220, 255});
    DrawText("Press R to play again", 70, 316, 30, {186, 214, 248, 255});
    DrawText("Press M for main menu", 70, 358, 30, {186, 214, 248, 255});
}

void GameEngine::DrawMiniMap(int originX, int originY, int cellSize) const {
    DrawText("MAP", originX, originY - 30, 24, RAYWHITE);

    for (int y = 0; y < WorldMap::kHeight; ++y) {
        for (int x = 0; x < WorldMap::kWidth; ++x) {
            const int px = originX + x * cellSize;
            const int py = originY + y * cellSize;

            if (!world_.HasRoomAt(x, y)) {
                DrawRectangle(px, py, cellSize - 2, cellSize - 2, {20, 20, 20, 160});
                continue;
            }

            Color fill = world_.IsDiscovered(x, y) ? Color{96, 140, 175, 230} : Color{54, 60, 68, 230};
            if (x == world_.CurrentX() && y == world_.CurrentY()) {
                fill = {232, 204, 118, 255};
            }

            DrawRectangle(px, py, cellSize - 2, cellSize - 2, fill);
            DrawRectangleLines(px, py, cellSize - 2, cellSize - 2, {28, 28, 28, 255});
        }
    }
}

void GameEngine::DrawRoomPortals(const Room& room) const {
    const Color openColor{98, 214, 122, 255};
    if (room.HasExit(Direction::North)) {
        DrawCube({0.0f, 3.0f, -kRoomHalfExtent + 0.2f}, 4.2f, 5.0f, 0.2f, openColor);
    }
    if (room.HasExit(Direction::South)) {
        DrawCube({0.0f, 3.0f, kRoomHalfExtent - 0.2f}, 4.2f, 5.0f, 0.2f, openColor);
    }
    if (room.HasExit(Direction::West)) {
        DrawCube({-kRoomHalfExtent + 0.2f, 3.0f, 0.0f}, 0.2f, 5.0f, 4.2f, openColor);
    }
    if (room.HasExit(Direction::East)) {
        DrawCube({kRoomHalfExtent - 0.2f, 3.0f, 0.0f}, 0.2f, 5.0f, 4.2f, openColor);
    }
}

void GameEngine::ParseAndExecuteCommand(const std::string& command) {
    const std::string cmd = NormalizeCommand(command);

    if (cmd == "help") {
        statusMessage_ =
            "Commands: go n/s/e/w, inventory, status, seed, regen [seed], reload assets, save, load, quit";
        return;
    }

    if (cmd == "inventory" || cmd == "inv") {
        statusMessage_ = "Inventory: " + player_.InventorySummary();
        return;
    }

    if (cmd == "status") {
        std::ostringstream oss;
        oss << "HP " << player_.Health() << "/" << player_.MaxHealth() << " | Room: " << world_.CurrentRoom().Name()
            << " | Seed: " << world_.Seed();
        statusMessage_ = oss.str();
        return;
    }

    if (cmd == "seed") {
        statusMessage_ = "Current world seed: " + std::to_string(world_.Seed());
        return;
    }

    if (cmd == "manifest") {
        statusMessage_ = "Manifest version: " + manifestVersion_;
        return;
    }

    if (cmd == "reload assets" || cmd == "reloadassets") {
        const std::string manifestPath = ResolveManifestPath();
        std::string manifestError;
        if (!assets_.LoadManifest(manifestPath, manifestError)) {
            statusMessage_ = "Asset reload failed: " + manifestError;
            return;
        }

        std::string assetInfo;
        assets_.LoadAvailableTextures(assetInfo);
        manifestVersion_ = assets_.ManifestVersion();
        statusMessage_ = "Assets reloaded. " + assetInfo;
        return;
    }

    if (cmd == "regen") {
        ResetRun(0);
        state_ = GameState::Exploration;
        statusMessage_ = "Regenerated world with seed " + std::to_string(world_.Seed()) + ".";
        return;
    }

    if (cmd.rfind("regen ", 0) == 0) {
        const std::string seedText = cmd.substr(6);
        uint32_t parsedSeed = 0;
        if (!TryParseSeed(seedText, parsedSeed)) {
            statusMessage_ = "Invalid seed. Use a positive integer like: regen 424242";
            return;
        }

        ResetRun(parsedSeed);
        state_ = GameState::Exploration;
        statusMessage_ = "Regenerated world with seed " + std::to_string(world_.Seed()) + ".";
        return;
    }

    if (cmd == "controls") {
        showControls_ = !showControls_;
        statusMessage_ = showControls_ ? "Controls shown." : "Controls hidden.";
        return;
    }

    if (cmd == "restart") {
        ResetRun(world_.Seed());
        state_ = GameState::Exploration;
        statusMessage_ = "Run restarted.";
        return;
    }

    if (cmd == "save") {
        SaveState state{};
        state.roomX = world_.CurrentX();
        state.roomY = world_.CurrentY();
        state.health = player_.Health();
        state.hasAncientSigil = player_.HasItem("Ancient Sigil");
        state.cryptEncounterCleared = cryptEncounterCleared_;
        state.worldSeed = world_.Seed();
        state.manifestVersion = manifestVersion_;

        std::string error;
        if (SaveManager::SaveToFile(kSaveFileName, state, error)) {
            statusMessage_ = "Game saved to savegame.txt.";
        } else {
            statusMessage_ = "Save failed: " + error;
        }
        return;
    }

    if (cmd == "load") {
        SaveState loaded{};
        std::string error;
        if (!SaveManager::LoadFromFile(kSaveFileName, loaded, error)) {
            statusMessage_ = "Load failed: " + error;
            return;
        }

        ResetRun(loaded.worldSeed);
        if (!world_.SetCurrentCell(loaded.roomX, loaded.roomY)) {
            statusMessage_ = "Load failed: save contains invalid room coordinates.";
            return;
        }

        player_.SetHealth(loaded.health);
        if (loaded.hasAncientSigil) {
            player_.AddItem("Ancient Sigil");
        }
        cryptEncounterCleared_ = loaded.cryptEncounterCleared;

        if (player_.Health() <= 0) {
            state_ = GameState::GameOver;
            statusMessage_ = "Loaded state is fatal. You are defeated.";
        } else {
            state_ = GameState::Exploration;
            statusMessage_ = "Game loaded from savegame.txt.";
            if (loaded.manifestVersion != manifestVersion_) {
                statusMessage_ += " Manifest differs from save; fallback visuals may vary.";
            }
        }
        return;
    }

    if (cmd == "quit") {
        running_ = false;
        return;
    }

    if (cmd == "north" || cmd == "n" || cmd == "go north") {
        TryMoveRoom(Direction::North, true);
        return;
    }

    if (cmd == "south" || cmd == "s" || cmd == "go south") {
        TryMoveRoom(Direction::South, true);
        return;
    }

    if (cmd == "east" || cmd == "e" || cmd == "go east") {
        TryMoveRoom(Direction::East, true);
        return;
    }

    if (cmd == "west" || cmd == "w" || cmd == "go west") {
        TryMoveRoom(Direction::West, true);
        return;
    }

    statusMessage_ = "Unknown command. Type help.";
}

void GameEngine::TryMoveRoom(Direction direction, bool fromCommand) {
    if (!world_.CanMove(direction)) {
        statusMessage_ = "No passage " + std::string(DirectionToString(direction)) + " from here.";
        return;
    }

    world_.Move(direction);
    player_.ResetRoomPosition();

    const Room& room = world_.CurrentRoom();
    statusMessage_ = std::string("You move ") + DirectionToString(direction) + " into " + room.Name() + ".";

    if (room.Role() == RoomRole::SigilVault && !player_.HasItem("Ancient Sigil")) {
        player_.AddItem("Ancient Sigil");
        statusMessage_ += " You recovered the Ancient Sigil.";
    }

    if (room.Role() == RoomRole::CryptGate && !cryptEncounterCleared_) {
        combat_.StartEncounter(Enemy{"Crypt Warden", 85, 8, 16, {188, 92, 92, 255}});
        state_ = GameState::Combat;
        statusMessage_ = "A Crypt Warden blocks your path. Choose: 1 Quick, 2 Heavy, 3 Defend.";
        return;
    }

    if (player_.Health() <= 0) {
        state_ = GameState::GameOver;
        statusMessage_ = "You collapse from your wounds.";
        return;
    }

    if (room.Role() == RoomRole::ObjectiveSanctum) {
        if (player_.HasItem("Ancient Sigil")) {
            state_ = GameState::Victory;
            statusMessage_ = "You restored the Ancient Sigil at the sanctum and secured victory.";
        } else {
            statusMessage_ += " The altar is inert. Search for the Ancient Sigil in the vault room.";
        }
    }

    if (fromCommand) {
        commandBuffer_.clear();
    }
}

void GameEngine::ResetRun(uint32_t forcedSeed) {
    if (forcedSeed == 0) {
        world_.GenerateProceduralLayoutAuto();
    } else {
        world_.GenerateProceduralLayout(forcedSeed);
    }

    player_ = Player{};
    player_.SetName("Hero");
    player_.ResetRoomPosition();
    cryptEncounterCleared_ = false;
    commandBuffer_.clear();
    statusMessage_ = "New run started. Seed " + std::to_string(world_.Seed()) + ".";
}

std::string GameEngine::NormalizeCommand(const std::string& raw) {
    std::string out;
    out.reserve(raw.size());

    bool previousSpace = true;
    for (const unsigned char c : raw) {
        if (std::isspace(c)) {
            if (!previousSpace) {
                out.push_back(' ');
                previousSpace = true;
            }
            continue;
        }

        out.push_back(static_cast<char>(std::tolower(c)));
        previousSpace = false;
    }

    if (!out.empty() && out.back() == ' ') {
        out.pop_back();
    }

    return out;
}

}  // namespace aq
