#include <game/main_game/system/wave_system.hpp>
#include <game/main_game/game_state.hpp>

#include <cmath>
#include <random>

namespace main_game {

static std::random_device rd;
static std::mt19937 gen(rd());

void WaveSystem::update(GameState& game_state, float delta) {
    if (!_game_started) return;

    if (!_wave_active) {
        // Wait for delay between waves
        _wave_delay_timer -= delta;
        if (_wave_delay_timer <= 0.0f) {
            start_next_wave(game_state);
        }
        return;
    }

    // Wave is active - handle spawning
    if (_enemies_to_spawn > 0) {
        _spawn_timer -= delta;
        if (_spawn_timer <= 0.0f) {
            spawn_next_enemy(game_state);
            _spawn_timer = _current_spawn_delay;
        }
    }

    // Check if wave is complete (all enemies dead and none to spawn)
    if (_enemies_to_spawn == 0 && game_state.enemy_manager.alive_count() == 0) {
        _wave_active = false;
        _wave_delay_timer = WAVE_START_DELAY;
    }
}

void WaveSystem::start_game() {
    _game_started = true;
    _current_wave = 0;
    _wave_active = false;
    _wave_delay_timer = 1.0f;  // Short delay before first wave
}

void WaveSystem::reset() {
    _game_started = false;
    _current_wave = 0;
    _wave_active = false;
    _enemies_to_spawn = 0;
}

void WaveSystem::start_next_wave(GameState& game_state) {
    _current_wave++;
    _wave_active = true;

    WaveConfig config = calculate_wave_config(_current_wave);
    _melee_remaining = config.melee_count;
    _ranged_remaining = config.ranged_count;
    _enemies_to_spawn = _melee_remaining + _ranged_remaining;
    _current_spawn_delay = config.spawn_delay;
    _spawn_timer = 0.0f;  // Spawn first enemy immediately
}

void WaveSystem::spawn_next_enemy(GameState& game_state) {
    if (_enemies_to_spawn <= 0) return;

    // Determine enemy type (spawn melee first, then ranged)
    EnemyType type;
    if (_melee_remaining > 0) {
        type = EnemyType::Melee;
        _melee_remaining--;
    } else {
        type = EnemyType::Ranged;
        _ranged_remaining--;
    }

    glm::vec3 spawn_pos = get_spawn_position(game_state);
    game_state.enemy_manager.spawn_enemy(type, spawn_pos);

    _enemies_to_spawn--;
}

WaveConfig WaveSystem::calculate_wave_config(int wave_number) const {
    float scaling = std::pow(ENEMY_SCALING, wave_number - 1);

    WaveConfig config;
    config.melee_count = static_cast<int>(BASE_MELEE_COUNT * scaling);
    config.ranged_count = static_cast<int>(BASE_RANGED_COUNT * scaling);

    // Minimum counts
    if (config.melee_count < 1) config.melee_count = 1;

    // Ranged enemies start appearing from wave 2
    if (wave_number < 2) config.ranged_count = 0;

    // Faster spawning in later waves
    config.spawn_delay = std::max(0.1f, 0.3f - (wave_number * 0.03f));

    return config;
}

glm::vec3 WaveSystem::get_spawn_position(const GameState& game_state) const {
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> dist_dist(MIN_SPAWN_DISTANCE, MAX_SPAWN_DISTANCE);

    float angle = angle_dist(gen);
    float distance = dist_dist(gen);

    glm::vec3 player_pos = game_state.player.position();
    glm::vec3 offset(
        std::cos(angle) * distance,
        0.0f,
        std::sin(angle) * distance
    );

    return player_pos + offset;
}

}  // namespace main_game
