#pragma once

namespace main_game {
class GameState;
struct Weapon;
}

namespace main_game::ui {

class GameHUD {
public:
    void render(const GameState& state);

private:
    void render_health_bar(float health, float max_health);
    void render_stamina_bar(float stamina, float max_stamina);
    void render_weapon_display(const Weapon& weapon);
};

}  // namespace main_game::ui
