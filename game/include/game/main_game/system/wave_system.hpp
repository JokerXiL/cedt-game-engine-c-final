#pragma once

#include <glm/glm.hpp>

namespace main_game {
class GameState;

struct WaveConfig {
    int melee_count;
    int ranged_count;
    float spawn_delay;
};

class WaveSystem {
public:
    WaveSystem() = default;

    void update(GameState& game_state, float delta);
    void start_game();
    void reset();

    // Accessors
    int current_wave() const { return _current_wave; }
    bool is_wave_active() const { return _wave_active; }
    bool is_game_started() const { return _game_started; }

private:
    void start_next_wave(GameState& game_state);
    void spawn_next_enemy(GameState& game_state);
    WaveConfig calculate_wave_config(int wave_number) const;
    glm::vec3 get_spawn_position(const GameState& game_state) const;

    // Wave state
    int _current_wave = 0;
    bool _wave_active = false;
    bool _game_started = false;

    // Spawn state
    int _enemies_to_spawn = 0;
    int _melee_remaining = 0;
    int _ranged_remaining = 0;

    // Timing
    float _spawn_timer = 0.0f;
    float _wave_delay_timer = 0.0f;
    float _current_spawn_delay = 0.5f;

    // Configuration
    static constexpr float WAVE_START_DELAY = 3.0f;
    static constexpr float MIN_SPAWN_DISTANCE = 10.0f;
    static constexpr float MAX_SPAWN_DISTANCE = 20.0f;

    // Difficulty scaling
    static constexpr int BASE_MELEE_COUNT = 3;
    static constexpr int BASE_RANGED_COUNT = 1;
    static constexpr float ENEMY_SCALING = 1.2f;
};

}  // namespace main_game
