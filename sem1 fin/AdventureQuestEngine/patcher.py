import sys

def main():
    with open('src/FileIO.cpp', 'r') as f:
        lines = f.readlines()

    start_item = -1
    end_item = -1
    start_enemy = -1
    end_enemy = -1

    for i, line in enumerate(lines):
        if "std::vector<Item> FileIO::loadItems" in line:
            start_item = i
        if "std::vector<Enemy> FileIO::loadEnemies" in line:
            end_item = i
            start_enemy = i
        if "std::vector<Riddle> FileIO::loadPuzzles" in line:
            end_enemy = i
            break

    if start_item == -1 or end_item == -1 or start_enemy == -1 or end_enemy == -1:
        print("Could not find function boundaries")
        return

    new_items_func = """std::vector<Item> FileIO::loadItems(const std::string& filename) {
    std::vector<Item> items;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Could not open " << filename << ". Using default items.\\n";
        return createDefaultItems();
    }
    
    try {
        nlohmann::json j;
        file >> j;
        items = j.get<std::vector<Item>>();
    } catch (const std::exception& e) {
        std::cerr << "Error parsing items JSON: " << e.what() << "\\nUsing defaults instead.\\n";
        return createDefaultItems();
    }
    return items;
}

"""
    new_enemies_func = """std::vector<Enemy> FileIO::loadEnemies(const std::string& filename) {
    std::vector<Enemy> enemies;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Could not open " << filename << ". Using default enemies.\\n";
        return createDefaultEnemies();
    }
    
    try {
        nlohmann::json j;
        file >> j;
        enemies = j.get<std::vector<Enemy>>();
    } catch (const std::exception& e) {
        std::cerr << "Error parsing enemies JSON: " << e.what() << "\\nUsing defaults instead.\\n";
        return createDefaultEnemies();
    }
    return enemies;
}

"""

    new_lines = lines[:start_item] + [new_items_func] + [new_enemies_func] + lines[end_enemy:]
    
    # Also change loadAllGameData to use .json instead of .txt
    for i, line in enumerate(new_lines):
        if 'gameState.itemTemplates = loadItems(dataPath + "items.txt");' in line:
            new_lines[i] = '    gameState.itemTemplates = loadItems(dataPath + "items.json");\n'
        elif 'gameState.enemyTemplates = loadEnemies(dataPath + "enemies.txt");' in line:
            new_lines[i] = '    gameState.enemyTemplates = loadEnemies(dataPath + "enemies.json");\n'


    with open('src/FileIO.cpp', 'w') as f:
        f.writelines(new_lines)
    print("Patched correctly.")

if __name__ == "__main__":
    main()
