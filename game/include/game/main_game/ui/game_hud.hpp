#pragma once

namespace main_game {
class GameState;
class Weapon;
}

namespace main_game::ui {

class GameHUD {
public:
    void render(const GameState& state);

private:
    void render_health_bar(float health, float max_health);
    void render_stamina_bar(float stamina, float max_stamina);
    void render_weapons_display(const Weapon& main_weapon, const Weapon& sub_weapon);
    void render_crosshair();
};

}  // namespace main_game::ui
