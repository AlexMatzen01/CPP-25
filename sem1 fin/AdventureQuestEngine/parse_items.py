import json

path = "/mnt/c/Users/mintt/Documents/c++/CPP-25/sem1 fin/AdventureQuestEngine/data/items.json"
with open(path, 'r') as f:
    items = json.load(f)

total_items = len(items)
levels = [item.get('itemLevel', 0) for item in items]
min_level = min(levels) if levels else 0
max_level = max(levels) if levels else 0

rarity_stats = {}
primary_stats_count = 0
primary_stat_fields = ['healAmount', 'attackBonus', 'defenseBonus', 'critBonus', 'dodgeBonus', 'speedBonus']

for item in items:
    # Rarity stats
    rarity = item.get('rarity', 'Unknown')
    level = item.get('itemLevel', 0)
    if rarity not in rarity_stats:
        rarity_stats[rarity] = {'count': 0, 'min_level': float('inf'), 'max_level': float('-inf')}
    rarity_stats[rarity]['count'] += 1
    rarity_stats[rarity]['min_level'] = min(rarity_stats[rarity]['min_level'], level)
    rarity_stats[rarity]['max_level'] = max(rarity_stats[rarity]['max_level'], level)

    # Primary stats count
    has_primary = False
    for field in primary_stat_fields:
        if item.get(field, 0) != 0:
            has_primary = True
            break
    if has_primary:
        primary_stats_count += 1

print(f"Total items: {total_items}")
print(f"Min/Max itemLevel: {min_level}/{max_level}")
print("Counts by rarity with min/max level per rarity:")
for rarity, stats in rarity_stats.items():
    print(f"  {rarity}: {stats['count']} items (Level {stats['min_level']}-{stats['max_level']})")
print(f"Items with non-zero primary stat: {primary_stats_count}")
