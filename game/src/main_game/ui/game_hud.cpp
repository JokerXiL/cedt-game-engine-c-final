#include <game/main_game/ui/game_hud.hpp>
#include <game/main_game/game_state.hpp>
#include <game/main_game/player.hpp>
#include <game/main_game/weapon.hpp>
#include <engine/window/window_system.hpp>
#include <engine/ui/ui_system.hpp>

namespace main_game::ui {

void GameHUD::render(const GameState& state) {
    const auto& player = state.player;
    const auto& progression = state.progression_system;

    render_health_bar(player.health(), player.max_health());
    render_stamina_bar(player.stamina(), player.max_stamina());
    render_xp_bar(progression.level(), progression.xp_progress());
    render_weapons_display(player.main_weapon(), player.sub_weapon());
    render_crosshair();
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

void GameHUD::render_xp_bar(int level, float progress) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoBackground |
                             ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(20, 80));
    ImGui::SetNextWindowSize(ImVec2(250, 40));

    ImGui::Begin("XPBar", nullptr, flags);

    // Level label
    char level_text[16];
    snprintf(level_text, sizeof(level_text), "LV%d", level);
    ImGui::TextColored(ImVec4(0.8f, 0.6f, 1.0f, 1.0f), "%s", level_text);
    ImGui::SameLine();

    // XP progress bar (purple/gold colors)
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.6f, 0.4f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.1f, 0.2f, 0.8f));
    ImGui::ProgressBar(progress, ImVec2(180, 14), "");
    ImGui::PopStyleColor(2);

    ImGui::End();
}

void GameHUD::render_weapons_display(const Weapon& main_weapon, const Weapon& sub_weapon) {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoSavedSettings;

    float screen_width = static_cast<float>(engine::window::SCR_WIDTH);
    float screen_height = static_cast<float>(engine::window::SCR_HEIGHT);

    ImGui::SetNextWindowPos(ImVec2(screen_width - 200, screen_height - 100));
    ImGui::SetNextWindowSize(ImVec2(180, 80));

    ImGui::Begin("WeaponDisplay", nullptr, flags);

    // Main weapon (LMB)
    ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.4f, 1.0f), "LMB: %s", main_weapon.name().c_str());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(%d)", main_weapon.damage());

    // Sub weapon (RMB)
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.9f, 1.0f), "RMB: %s", sub_weapon.name().c_str());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(%d)", sub_weapon.damage());

    ImGui::End();
}

void GameHUD::render_crosshair() {
    float screen_width = static_cast<float>(engine::window::SCR_WIDTH);
    float screen_height = static_cast<float>(engine::window::SCR_HEIGHT);

    float center_x = screen_width / 2.0f;
    float center_y = screen_height / 2.0f;

    // Crosshair size
    float size = 10.0f;
    float thickness = 2.0f;
    float gap = 4.0f;

    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    ImU32 color = IM_COL32(255, 255, 255, 200);

    // Horizontal lines
    draw_list->AddLine(
        ImVec2(center_x - size - gap, center_y),
        ImVec2(center_x - gap, center_y),
        color, thickness);
    draw_list->AddLine(
        ImVec2(center_x + gap, center_y),
        ImVec2(center_x + size + gap, center_y),
        color, thickness);

    // Vertical lines
    draw_list->AddLine(
        ImVec2(center_x, center_y - size - gap),
        ImVec2(center_x, center_y - gap),
        color, thickness);
    draw_list->AddLine(
        ImVec2(center_x, center_y + gap),
        ImVec2(center_x, center_y + size + gap),
        color, thickness);

    // Center dot
    draw_list->AddCircleFilled(ImVec2(center_x, center_y), 2.0f, color);
}

}  // namespace main_game::ui
