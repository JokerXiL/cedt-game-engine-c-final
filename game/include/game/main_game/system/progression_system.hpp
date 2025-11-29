#pragma once

#include <game/main_game/enemy.hpp>

#include <vector>

namespace main_game {

class Player;
struct Perk;

class ProgressionSystem {
public:
    ProgressionSystem() = default;

    // Called when an enemy is killed - adds XP and checks for level up
    void on_enemy_killed(EnemyType type);

    // Check if level up is pending (needs perk selection)
    bool is_level_up_pending() const { return _pending_level_up; }

    // Apply the selected perk and clear pending state
    void apply_perk(Player& player, int choice_index);

    // Get the 3 perk choices for current level up
    const std::vector<const Perk*>& get_perk_choices() const { return _current_choices; }

    // Accessors
    int experience() const { return _experience; }
    int level() const { return _level; }
    int xp_for_next_level() const;
    int xp_for_current_level() const;
    float xp_progress() const;  // 0.0 to 1.0

private:
    void check_level_up();
    void generate_perk_choices();
    int calculate_xp_threshold(int level) const;
    int get_xp_for_enemy(EnemyType type) const;
    bool can_acquire_perk(int perk_id) const;

    int _experience = 0;
    int _level = 1;
    bool _pending_level_up = false;
    std::vector<const Perk*> _current_choices;
    std::vector<int> _acquired_perk_ids;  // Track perk acquisition counts
};

}  // namespace main_game
