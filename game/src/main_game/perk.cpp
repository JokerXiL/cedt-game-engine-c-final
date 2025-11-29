#include <game/main_game/perk.hpp>
#include <game/main_game/player.hpp>

namespace main_game {

const PerkRegistry& PerkRegistry::instance() {
    static PerkRegistry registry;
    return registry;
}

const Perk* PerkRegistry::get_perk(int id) const {
    for (const auto& perk : _perks) {
        if (perk.id == id) {
            return &perk;
        }
    }
    return nullptr;
}

PerkRegistry::PerkRegistry() {
    register_perks();
}

void PerkRegistry::register_perks() {
    // =====================================================
    // STAT BOOST PERKS (Stackable)
    // =====================================================

    _perks.push_back({
        .id = 1,
        .name = "Vitality",
        .description = "+25 Max HP",
        .category = PerkCategory::StatBoost,
        .is_unique = false,
        .max_stacks = 0,
        .apply = [](Player& player) {
            player.add_max_health(25.0f);
        }
    });

    _perks.push_back({
        .id = 2,
        .name = "Endurance",
        .description = "+20 Max Stamina",
        .category = PerkCategory::StatBoost,
        .is_unique = false,
        .max_stacks = 0,
        .apply = [](Player& player) {
            player.add_max_stamina(20.0f);
        }
    });

    _perks.push_back({
        .id = 3,
        .name = "Swiftness",
        .description = "+10% Move Speed",
        .category = PerkCategory::StatBoost,
        .is_unique = false,
        .max_stacks = 5,
        .apply = [](Player& player) {
            player.multiply_move_speed(1.10f);
        }
    });

    _perks.push_back({
        .id = 4,
        .name = "Regeneration",
        .description = "+2 HP/sec",
        .category = PerkCategory::StatBoost,
        .is_unique = false,
        .max_stacks = 5,
        .apply = [](Player& player) {
            player.add_health_regen(2.0f);
        }
    });

    // =====================================================
    // WEAPON UPGRADE PERKS (Stackable)
    // =====================================================

    _perks.push_back({
        .id = 5,
        .name = "Power Strike",
        .description = "+15% Damage",
        .category = PerkCategory::WeaponUpgrade,
        .is_unique = false,
        .max_stacks = 5,
        .apply = [](Player& player) {
            player.multiply_damage(1.15f);
        }
    });

    _perks.push_back({
        .id = 6,
        .name = "Quick Hands",
        .description = "+10% Attack Speed",
        .category = PerkCategory::WeaponUpgrade,
        .is_unique = false,
        .max_stacks = 5,
        .apply = [](Player& player) {
            player.multiply_attack_speed(1.10f);
        }
    });

    // =====================================================
    // ABILITY PERKS (Unique)
    // =====================================================

    _perks.push_back({
        .id = 7,
        .name = "Blood Thirst",
        .description = "5% Lifesteal",
        .category = PerkCategory::Ability,
        .is_unique = true,
        .max_stacks = 1,
        .apply = [](Player& player) {
            player.add_lifesteal(0.05f);
        }
    });

    _perks.push_back({
        .id = 8,
        .name = "Thick Skin",
        .description = "15% Damage Reduction",
        .category = PerkCategory::Ability,
        .is_unique = true,
        .max_stacks = 1,
        .apply = [](Player& player) {
            player.add_damage_reduction(0.15f);
        }
    });

    _perks.push_back({
        .id = 9,
        .name = "Iron Will",
        .description = "+50 Max HP, +10% Damage Reduction",
        .category = PerkCategory::Ability,
        .is_unique = true,
        .max_stacks = 1,
        .apply = [](Player& player) {
            player.add_max_health(50.0f);
            player.add_damage_reduction(0.10f);
        }
    });

    _perks.push_back({
        .id = 10,
        .name = "Berserker",
        .description = "+30% Damage, +20% Attack Speed",
        .category = PerkCategory::Ability,
        .is_unique = true,
        .max_stacks = 1,
        .apply = [](Player& player) {
            player.multiply_damage(1.30f);
            player.multiply_attack_speed(1.20f);
        }
    });

    _perks.push_back({
        .id = 11,
        .name = "Vampire",
        .description = "10% Lifesteal, -20 Max HP",
        .category = PerkCategory::Ability,
        .is_unique = true,
        .max_stacks = 1,
        .apply = [](Player& player) {
            player.add_lifesteal(0.10f);
            player.add_max_health(-20.0f);
        }
    });

    _perks.push_back({
        .id = 12,
        .name = "Glass Cannon",
        .description = "+50% Damage, -30% Max HP",
        .category = PerkCategory::Ability,
        .is_unique = true,
        .max_stacks = 1,
        .apply = [](Player& player) {
            player.multiply_damage(1.50f);
            // Reduce max health by 30%
            float current_max = player.max_health();
            player.add_max_health(-current_max * 0.30f);
        }
    });
}

}  // namespace main_game
