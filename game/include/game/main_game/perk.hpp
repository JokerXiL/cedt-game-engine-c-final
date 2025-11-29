#pragma once

#include <functional>
#include <string>
#include <vector>

namespace main_game {

class Player;

enum class PerkCategory {
    StatBoost,      // Health, stamina, speed increases
    WeaponUpgrade,  // Damage, attack speed, range improvements
    Ability         // Special passive abilities
};

struct Perk {
    int id;
    std::string name;
    std::string description;
    PerkCategory category;
    bool is_unique;    // Can only be acquired once
    int max_stacks;    // For stackable perks (0 = unlimited)

    // Function to apply the perk effect to the player
    std::function<void(Player&)> apply;
};

// Registry of all available perks (singleton)
class PerkRegistry {
public:
    static const PerkRegistry& instance();

    const std::vector<Perk>& all_perks() const { return _perks; }
    const Perk* get_perk(int id) const;
    size_t perk_count() const { return _perks.size(); }

private:
    PerkRegistry();
    void register_perks();

    std::vector<Perk> _perks;
};

}  // namespace main_game
