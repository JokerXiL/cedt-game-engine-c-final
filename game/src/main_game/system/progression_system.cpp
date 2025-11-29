#include <game/main_game/system/progression_system.hpp>
#include <game/main_game/perk.hpp>
#include <game/main_game/player.hpp>

#include <algorithm>
#include <cmath>
#include <random>

namespace main_game {

namespace {
    // XP constants
    constexpr int BASE_XP_THRESHOLD = 100;
    constexpr float XP_SCALING = 1.5f;

    // Enemy XP values
    constexpr int MELEE_ENEMY_XP = 15;
    constexpr int RANGED_ENEMY_XP = 10;

    // Number of perk choices per level up
    constexpr size_t PERK_CHOICES = 3;
}

void ProgressionSystem::on_enemy_killed(EnemyType type) {
    if (_pending_level_up) return;  // Don't accumulate XP while waiting for perk selection

    _experience += get_xp_for_enemy(type);
    check_level_up();
}

void ProgressionSystem::apply_perk(Player& player, int choice_index) {
    if (choice_index < 0 || choice_index >= static_cast<int>(_current_choices.size())) {
        return;
    }

    const Perk* selected = _current_choices[choice_index];
    if (selected) {
        // Apply the perk effect
        selected->apply(player);

        // Track the acquired perk
        _acquired_perk_ids.push_back(selected->id);
    }

    // Clear pending state
    _pending_level_up = false;
    _current_choices.clear();

    // Check if we leveled up again while waiting
    check_level_up();
}

int ProgressionSystem::xp_for_next_level() const {
    return calculate_xp_threshold(_level);
}

int ProgressionSystem::xp_for_current_level() const {
    if (_level <= 1) return 0;
    return calculate_xp_threshold(_level - 1);
}

float ProgressionSystem::xp_progress() const {
    int current_threshold = xp_for_current_level();
    int next_threshold = xp_for_next_level();
    int xp_in_level = _experience - current_threshold;
    int xp_needed = next_threshold - current_threshold;

    if (xp_needed <= 0) return 1.0f;
    return static_cast<float>(xp_in_level) / static_cast<float>(xp_needed);
}

void ProgressionSystem::check_level_up() {
    if (_pending_level_up) return;

    int threshold = xp_for_next_level();
    if (_experience >= threshold) {
        _level++;
        _pending_level_up = true;
        generate_perk_choices();
    }
}

void ProgressionSystem::generate_perk_choices() {
    _current_choices.clear();

    const auto& registry = PerkRegistry::instance();
    const auto& all_perks = registry.all_perks();

    // Build list of available perks (excluding maxed-out ones)
    std::vector<const Perk*> available;
    for (const auto& perk : all_perks) {
        if (can_acquire_perk(perk.id)) {
            available.push_back(&perk);
        }
    }

    // Shuffle and pick up to PERK_CHOICES perks
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(available.begin(), available.end(), gen);

    size_t count = std::min(PERK_CHOICES, available.size());
    for (size_t i = 0; i < count; ++i) {
        _current_choices.push_back(available[i]);
    }
}

int ProgressionSystem::calculate_xp_threshold(int level) const {
    // Formula: BASE * SCALING^(level-1)
    // Level 1->2: 100 XP
    // Level 2->3: 150 XP
    // Level 3->4: 225 XP, etc.
    return static_cast<int>(BASE_XP_THRESHOLD * std::pow(XP_SCALING, level - 1));
}

int ProgressionSystem::get_xp_for_enemy(EnemyType type) const {
    switch (type) {
        case EnemyType::Melee:
            return MELEE_ENEMY_XP;
        case EnemyType::Ranged:
            return RANGED_ENEMY_XP;
        default:
            return RANGED_ENEMY_XP;
    }
}

bool ProgressionSystem::can_acquire_perk(int perk_id) const {
    const Perk* perk = PerkRegistry::instance().get_perk(perk_id);
    if (!perk) return false;

    // Count how many times we've acquired this perk
    int count = static_cast<int>(std::count(_acquired_perk_ids.begin(),
                                             _acquired_perk_ids.end(),
                                             perk_id));

    // Check against max stacks (0 = unlimited)
    if (perk->max_stacks > 0 && count >= perk->max_stacks) {
        return false;
    }

    return true;
}

}  // namespace main_game
