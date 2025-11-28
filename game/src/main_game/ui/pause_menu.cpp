#include <game/main_game/ui/pause_menu.hpp>
#include <engine/window_system.hpp>

#include <imgui.h>

namespace main_game::ui {

PauseAction PauseMenu::render() {
    if (!_is_open) return PauseAction::None;

    PauseAction action = PauseAction::None;

    float screen_width = static_cast<float>(engine::SCR_WIDTH);
    float screen_height = static_cast<float>(engine::SCR_HEIGHT);

    // Dim background
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(screen_width, screen_height));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
    ImGui::Begin("PauseOverlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoInputs);
    ImGui::End();
    ImGui::PopStyleColor();

    // Menu window
    float menu_width = 300;
    float menu_height = 200;

    ImGui::SetNextWindowPos(ImVec2((screen_width - menu_width) / 2,
                                    (screen_height - menu_height) / 2));
    ImGui::SetNextWindowSize(ImVec2(menu_width, menu_height));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Paused", nullptr, flags);

    // Center the title
    float title_width = ImGui::CalcTextSize("PAUSED").x;
    ImGui::SetCursorPosX((menu_width - title_width) / 2);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "PAUSED");

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();

    // Button styling
    float button_width = 200;
    float button_height = 40;
    float center_x = (menu_width - button_width) / 2;

    ImGui::SetCursorPosX(center_x);
    if (ImGui::Button("Resume", ImVec2(button_width, button_height))) {
        action = PauseAction::Resume;
    }

    ImGui::Spacing();

    ImGui::SetCursorPosX(center_x);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("Quit", ImVec2(button_width, button_height))) {
        action = PauseAction::Quit;
    }
    ImGui::PopStyleColor(2);

    ImGui::End();

    return action;
}

}  // namespace main_game::ui
