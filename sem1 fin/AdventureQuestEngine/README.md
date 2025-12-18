# AdventureQuestEngine

**Requirements**
- g++ (C++17 support)
- make
- Optional: cmake (some subprojects include CMake files)

**Quick build & run**
Open a terminal and run:

```bash
cd "sem1 fin/AdventureQuestEngine"
make -j4
./bin/AdventureQuestEngine
```

**Common commands**
- Clean build artifacts:

```bash
make clean
```

- Rebuild (clean + build):

```bash
make clean && make -j4
```

**Notes**
- The binary is placed at `bin/AdventureQuestEngine` relative to the project folder.
- Game data files (leaderboard, item lists) are under `data/` inside the same folder (e.g. `data/leaderboard.txt`, `data/rare_vendor_items.txt`).
- To enable debug-only features (cheats, debug displays), open `Menu -> Settings -> Debug Mode` in-game, then use the `cheats` command while playing.
