#include <game/main_game/ui/game_hud.hpp>
#include <game/main_game/game_state.hpp>
#include <game/main_game/player.hpp>
#include <game/main_game/weapon.hpp>
#include <engine/window/window_system.hpp>

#include <imgui.h>

namespace main_game::ui {

void GameHUD::render(const GameState& state) {
    const auto& player = state.player;

    render_health_bar(player.health(), player.max_health());
    render_stamina_bar(player.stamina(), player.max_stamina());
    render_weapon_display(player.main_weapon());
}

void GameHUD::render_health_bar(float health, float max_health) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoBackground |
                             ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(20, 20));
    ImGui::SetNextWindowSize(ImVec2(250, 60));

    ImGui::Begin("HealthBar", nullptr, flags);

    // Health label
    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "HP");
    ImGui::SameLine();

    // Health bar
    float fraction = health / max_health;
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.1f, 0.1f, 0.8f));
    ImGui::ProgressBar(fraction, ImVec2(180, 20), "");
    ImGui::PopStyleColor(2);

    // Health text
    ImGui::SameLine();
    ImGui::Text("%.0f/%.0f", health, max_health);

    ImGui::End();
}

void GameHUD::render_stamina_bar(float stamina, float max_stamina) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoBackground |
                             ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(20, 50));
    ImGui::SetNextWindowSize(ImVec2(250, 40));

    ImGui::Begin("StaminaBar", nullptr, flags);

    // Stamina label
    ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "SP");
    ImGui::SameLine();

    // Stamina bar
    float fraction = stamina / max_stamina;
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.2f, 0.1f, 0.8f));
    ImGui::ProgressBar(fraction, ImVec2(180, 16), "");
    ImGui::PopStyleColor(2);

    ImGui::End();
}

void GameHUD::render_weapon_display(const Weapon& weapon) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoSavedSettings;

    float screen_width = static_cast<float>(engine::window::SCR_WIDTH);
    float screen_height = static_cast<float>(engine::window::SCR_HEIGHT);

    ImGui::SetNextWindowPos(ImVec2(screen_width - 170, screen_height - 80));
    ImGui::SetNextWindowSize(ImVec2(150, 60));

    ImGui::Begin("WeaponDisplay", nullptr, flags);

    ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.4f, 1.0f), "%s", weapon.name.c_str());
    ImGui::Text("DMG: %d", weapon.damage);

    ImGui::End();
}

}  // namespace main_game::ui
