





#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <vector>
#include <array>

namespace AQE {






namespace Color {
    const std::string RESET = "\033[0m";
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string DARK_GRAY = "\033[90m";
    const std::string BOLD = "\033[1m";
    const std::string UNDERLINE = "\033[4m";
    
    
    const std::string BRIGHT_BLACK = "\033[90m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
    const std::string BRIGHT_WHITE = "\033[97m";
}


constexpr int MIN_MAP_WIDTH = 5;
constexpr int MAX_MAP_WIDTH = 20;
constexpr int MIN_MAP_HEIGHT = 5;
constexpr int MAX_MAP_HEIGHT = 20;
constexpr int DEFAULT_MAP_WIDTH = 15;
constexpr int DEFAULT_MAP_HEIGHT = 15;



constexpr int TREASURE_ROOM_CHANCE = 12;
constexpr int BOSS_ROOM_CHANCE = 8;
constexpr int TRAP_ROOM_CHANCE = 10;
constexpr int PUZZLE_ROOM_CHANCE = 8;
constexpr int SECRET_ROOM_CHANCE = 5;
constexpr int SAFE_ROOM_CHANCE = 8;
constexpr int SHOP_ROOM_CHANCE = 4;
constexpr int LIBRARY_ROOM_CHANCE = 8;
constexpr int FORGE_ROOM_CHANCE = 4;
constexpr int SHRINE_ROOM_CHANCE = 7;
constexpr int LABORATORY_ROOM_CHANCE = 7;
constexpr int GRAVEYARD_ROOM_CHANCE = 6;
constexpr int ARMORY_ROOM_CHANCE = 6;
constexpr int PRISON_ROOM_CHANCE = 5;


constexpr int BASE_CRIT_MULTIPLIER = 2;
constexpr float BASE_CRIT_BONUS_MULTIPLIER = 1.25f;  
constexpr int MAX_DODGE_CHANCE = 75;
constexpr int MAX_CRIT_CHANCE = 50;
constexpr int FLEE_BASE_CHANCE = 40;
constexpr int SPEED_FLEE_BONUS = 2;  


constexpr int BASE_XP_TO_LEVEL = 350;
constexpr float XP_MULTIPLIER_PER_LEVEL = 1.55f;
constexpr int MAX_LEVEL = 60;
constexpr int MAX_ITEM_LEVEL = 120;
constexpr int STAT_POINTS_PER_LEVEL = 5;


constexpr int DEFAULT_INVENTORY_SIZE = 30;
constexpr int STACK_LIMIT = 99;


constexpr float SELL_PRICE_MULTIPLIER = 0.5f;
constexpr int STARTING_GOLD = 50;


constexpr int MORALITY_HERO_THRESHOLD = 50;
constexpr int MORALITY_EVIL_THRESHOLD = -50;


constexpr float EASY_DAMAGE_MULT = 0.75f;
constexpr float EASY_DEFENSE_MULT = 0.80f;
constexpr float EASY_HP_MULT = 0.90f;
constexpr float EASY_GOLD_MULT = 1.5f;
constexpr float HARD_DAMAGE_MULT = 1.25f;
constexpr float HARD_DEFENSE_MULT = 1.15f;
constexpr float HARD_HP_MULT = 1.20f;
constexpr float HARD_GOLD_MULT = 0.75f;


constexpr int SPEEDRUN_TURN_LIMIT = 50;
constexpr int COMPLETIONIST_QUEST_COUNT = 5;





constexpr int COMMON_DROP_RATE = 50;
constexpr int UNCOMMON_DROP_RATE = 30;
constexpr int RARE_DROP_RATE = 15;
constexpr int EPIC_DROP_RATE = 4;
constexpr int LEGENDARY_DROP_RATE = 1;






constexpr int WARRIOR_HP = 210;
constexpr int WARRIOR_ATK = 20;
constexpr int WARRIOR_DEF = 16;
constexpr int WARRIOR_CRIT = 10;
constexpr int WARRIOR_DODGE = 6;
constexpr int WARRIOR_SPEED = 10;
constexpr int WARRIOR_MAGIC = 10;
constexpr int WARRIOR_MAGRES = 10;


constexpr int MAGE_HP = 110;
constexpr int MAGE_ATK = 9;
constexpr int MAGE_DEF = 6;
constexpr int MAGE_CRIT = 12;
constexpr int MAGE_DODGE = 8;
constexpr int MAGE_SPEED = 14;
constexpr int MAGE_MAGIC = 38;
constexpr int MAGE_MAGRES = 22;


constexpr int ROGUE_HP = 140;
constexpr int ROGUE_ATK = 16;
constexpr int ROGUE_DEF = 8;
constexpr int ROGUE_CRIT = 32;
constexpr int ROGUE_DODGE = 28;
constexpr int ROGUE_SPEED = 20;
constexpr int ROGUE_MAGIC = 10;
constexpr int ROGUE_MAGRES = 8;


constexpr int PALADIN_HP = 200;
constexpr int PALADIN_ATK = 16;
constexpr int PALADIN_DEF = 20;
constexpr int PALADIN_CRIT = 8;
constexpr int PALADIN_DODGE = 6;
constexpr int PALADIN_SPEED = 8;
constexpr int PALADIN_MAGIC = 28;
constexpr int PALADIN_MAGRES = 20;


constexpr int RANGER_HP = 155;
constexpr int RANGER_ATK = 19;
constexpr int RANGER_DEF = 9;
constexpr int RANGER_CRIT = 28;
constexpr int RANGER_DODGE = 18;
constexpr int RANGER_SPEED = 24;
constexpr int RANGER_MAGIC = 9;
constexpr int RANGER_MAGRES = 9;


constexpr int NECROMANCER_HP = 135;
constexpr int NECROMANCER_ATK = 11;
constexpr int NECROMANCER_DEF = 7;
constexpr int NECROMANCER_CRIT = 14;
constexpr int NECROMANCER_DODGE = 10;
constexpr int NECROMANCER_SPEED = 13;
constexpr int NECROMANCER_MAGIC = 36;
constexpr int NECROMANCER_MAGRES = 20;


constexpr int BARBARIAN_HP = 240;
constexpr int BARBARIAN_ATK = 26;
constexpr int BARBARIAN_DEF = 10;
constexpr int BARBARIAN_CRIT = 14;
constexpr int BARBARIAN_DODGE = 6;
constexpr int BARBARIAN_SPEED = 11;
constexpr int BARBARIAN_MAGIC = 6;
constexpr int BARBARIAN_MAGRES = 7;


constexpr int MONK_HP = 165;
constexpr int MONK_ATK = 15;
constexpr int MONK_DEF = 11;
constexpr int MONK_CRIT = 16;
constexpr int MONK_DODGE = 28;
constexpr int MONK_SPEED = 25;
constexpr int MONK_MAGIC = 14;
constexpr int MONK_MAGRES = 14;





const std::string TITLE_ART = R"(
===============================================================================
    ____   _    _        _       ______   __      __  ______   _   _   _______ 
   / __ \ | |  | |      | |     |  ____| |  \    /  ||  ____| | \ | | |__   __|
  | |  | || |  | |      | |     | |__    |   \  /   || |__    |  \| |    | |   
  | |  | || |  | |  _   | |     |  __|   | |\ \/ /| ||  __|   | . ` |    | |   
  | |__| || |__| | | |__| |     | |____  | | \  / | || |____  | |\  |    | |   
   \___\_\ \____/   \____/      |______| |_|  \/  |_||______| |_| \_|    |_|   
                                                                               
   ___   ____   _    _  ______   _   _   _______  _    _   _____   ______     
  / _ \ |  _ \ | |  | ||  ____| | \ | | |__   __|| |  | | |  __ \ |  ____|    
 / /_\ \| | | || |  | || |__    |  \| |    | |   | |  | | | |__) || |__       
 |  _  || | | || |  | ||  __|   | . ` |    | |   | |  | | |  _  / |  __|      
 | | | || |_| || |__| || |____  | |\  |    | |   | |__| | | | \ \ | |____     
 |_| |_||____/  \____/ |______| |_| \_|    |_|    \____/  |_|  \_\|______|    
                                                                               
              ______   _   _    _____   _____   _   _   ______                 
             |  ____| | \ | |  / ____| |_   _| | \ | | |  ____|                
             | |__    |  \| | | |  __    | |   |  \| | | |__                   
             |  __|   | . ` | | | |_ |   | |   | . ` | |  __|                  
             | |____  | |\  | | |__| |  _| |_  | |\  | | |____                 
             |______| |_| \_|  \_____| |_____| |_| \_| |______|                
                                                                               
===============================================================================
)";

const std::string GAME_OVER_ART = R"(
    ___    __    _  _   ___     __   _   _  ___  ___ 
   / __|  /  \  | \/ | | __|   / _ \\ \ / /| __|| _ \
  | (_ | | /\ | | |\/| || _|   | (_) || V / | _| |   /
   \___| |_||_| |_|  |_||___|   \___/  \_/  |___||_|_\
                                                      
)";

const std::string VICTORY_ART = R"(
   _   _  ___   ___  _____   ___   ___  __   __
  \ \ / /|_ _| / __||_   _| / _ \ | _ \ \ \ / /
   \ V /  | | | (__   | |  | (_) ||   /  \   / 
    \_/  |___| \___|  |_|   \___/ |_|_\   |_|  
                                                
)";

const std::string PLAYER_ART = R"(
     O
    /|\
    / \
)";

const std::string WARRIOR_ART = R"(
       ,      ,
      /(.-""-.)\
  |\  \/      \/  /|
  | \ / =.  .= \ / |
  \( \   o\/o   / )/
   \_, '-/  \-' ,_/
     /   \__/   \
     \ \__/\__/ /
   ___\ \|--|/ /___
 /`    \      /    `\
/       '----'       \
)";

const std::string MAGE_ART = R"(
        /\
       /  \
      / ** \
     /  ()  \
    /   /\   \
   /   /  \   \
  /   /    \   \
 /____|    |____\
       \  /
        \/
)";

const std::string ROGUE_ART = R"(
      _
    _( }
   / _`>
  /_( _\__,
    <_   /
      | |
      | |
     _|_|_
)";

const std::string PALADIN_ART = R"(
       ___ 
      |   |
     _|___|_
    |  ___  |
    | |   | |
    | |   | |
    | |___| |
    |_______|
      /   \
     /     \
)";

const std::string SKELETON_ART = R"(
      .-.
     (o o)
     | O |
     |   |
    /|   |\
   (_|   |_)
)";

const std::string GOBLIN_ART = R"(
     /\_/\
    ( o.o )
     > ^ <
    /|   |\
   (_|   |_)
)";

const std::string DRAGON_ART = R"(
                \||/
                |  @___oo
      /\  /\   / (__,,,,|
     ) /^\) ^\/ _)
     )   /^\/   _)
     )   _ /  / _)
 /\  )/\/ ||  | )_)
<  >      |(,,) )__)
 ||      /    \)___)\
 | \____(googols     )____
  \______(___________)
)";

const std::string TREASURE_ART = R"(
    ____
   /    \
  | $$$$ |
  | $$$$ |
   \____/
)";

const std::string TRAP_ART = R"(
   /\  /\
  /  \/  \
 /   /\   \
/___/  \___\
    ^^^^
)";





const std::vector<std::string> ROOM_PREFIXES = {
    "Dark", "Dusty", "Ancient", "Forgotten", "Cursed", "Mystic",
    "Gloomy", "Haunted", "Sacred", "Hidden", "Crumbling", "Frozen"
};

const std::vector<std::string> ROOM_TYPES_TEXT = {
    "Chamber", "Hall", "Passage", "Corridor", "Cavern", "Room",
    "Vault", "Crypt", "Den", "Lair", "Sanctuary", "Antechamber"
};

const std::vector<std::string> ROOM_DESCRIPTORS = {
    "Cobwebs cover the ceiling, and the air smells of decay.",
    "Strange symbols are carved into the stone walls.",
    "A faint light flickers from an unknown source.",
    "The walls are slick with moisture and mold.",
    "Bones of previous adventurers litter the floor.",
    "An eerie silence fills the space.",
    "The sound of dripping water echoes endlessly.",
    "Ancient murals depict scenes of battle and glory.",
    "A cold draft blows from somewhere unseen.",
    "Torches line the walls, casting dancing shadows."
};

const std::vector<std::string> COMBAT_HIT_TEXT = {
    "strikes", "slashes", "hits", "attacks", "smashes",
    "cuts", "stabs", "bashes", "pummels", "wounds"
};

const std::vector<std::string> COMBAT_CRIT_TEXT = {
    "lands a devastating blow on",
    "critically strikes",
    "finds a weak spot on",
    "delivers a crushing hit to",
    "executes a perfect strike against"
};

const std::vector<std::string> COMBAT_MISS_TEXT = {
    "misses", "swings wide at", "fails to hit",
    "whiffs against", "is dodged by"
};

const std::vector<std::string> DEATH_TEXT = {
    "crumbles to dust!",
    "falls to the ground!",
    "is defeated!",
    "collapses!",
    "is vanquished!"
};







const std::vector<Riddle> RIDDLE_DATABASE = {
    {"I have cities, but no houses. I have mountains, but no trees. "
     "I have water, but no fish. What am I?", "map", "Think about what you use to navigate."},
    
    {"The more you take, the more you leave behind. What am I?", 
     "footsteps", "Consider what happens when you walk."},
    
    {"I speak without a mouth and hear without ears. I have no body, "
     "but I come alive with the wind. What am I?", "echo", "What returns sound?"},
    
    {"What has keys but no locks, space but no room, and you can enter "
     "but can't go inside?", "keyboard", "You're using one right now."},
    
    {"I have hands but cannot clap. What am I?", "clock", "I tell you something about time."},
    
    {"What can travel around the world while staying in a corner?", 
     "stamp", "Think about mail."},
    
    {"What has a head, a tail, but no body?", "coin", "You might flip me."},
    
    {"What gets wetter the more it dries?", "towel", "Used after bathing."},
    
    {"I can be cracked, made, told, and played. What am I?", "joke", "Ha ha!"},
    
    {"What has one eye but cannot see?", "needle", "Used in sewing."}
};





const std::string DATA_FOLDER = "data/";
const std::string ROOMS_FILE = "data/rooms.txt";
const std::string ITEMS_FILE = "data/items.txt";
const std::string ENEMIES_FILE = "data/enemies.txt";
const std::string EVENTS_FILE = "data/events.txt";
const std::string QUESTS_FILE = "data/quests.txt";
const std::string SAVE_FILE = "data/savegame.txt";
const std::string LOG_FILE = "data/gamelog.txt";
const std::string LEADERBOARD_FILE = "data/leaderboard.txt";

}  

#endif 
