#pragma once

#include <string>
#include <cstdint>

namespace main_game {

enum class ItemType { Consumable, Weapon, Armor, Material, Quest };
enum class ItemRarity { Common, Uncommon, Rare, Epic, Legendary };

struct Item {
    uint32_t id = 0;
    std::string name;
    std::string description;
    ItemType type = ItemType::Material;
    ItemRarity rarity = ItemRarity::Common;
    int stack_count = 1;
    int max_stack = 99;

    // Type-specific data
    int heal_amount = 0;   // For consumables
    int damage = 0;        // For weapons
    int defense = 0;       // For armor
};

}  // namespace main_game
