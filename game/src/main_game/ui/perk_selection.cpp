#include <game/main_game/ui/perk_selection.hpp>
#include <game/main_game/perk.hpp>
#include <engine/window/window_system.hpp>
#include <engine/ui/ui_system.hpp>

namespace main_game::ui {

namespace {
    // Card dimensions
    constexpr float CARD_WIDTH = 220.0f;
    constexpr float CARD_HEIGHT = 300.0f;
    constexpr float CARD_GAP = 30.0f;

    // Category colors
    ImVec4 get_category_color(PerkCategory category) {
        switch (category) {
            case PerkCategory::StatBoost:
                return ImVec4(0.2f, 0.7f, 0.3f, 1.0f);  // Green
            case PerkCategory::WeaponUpgrade:
                return ImVec4(0.9f, 0.5f, 0.1f, 1.0f);  // Orange
            case PerkCategory::Ability:
                return ImVec4(0.3f, 0.5f, 0.9f, 1.0f);  // Blue
            default:
                return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);  // Gray
        }
    }

    const char* get_category_name(PerkCategory category) {
        switch (category) {
            case PerkCategory::StatBoost:
                return "STAT";
            case PerkCategory::WeaponUpgrade:
                return "WEAPON";
            case PerkCategory::Ability:
                return "ABILITY";
            default:
                return "PERK";
        }
    }
}

PerkSelectionAction PerkSelection::render(const std::vector<const Perk*>& choices) {
    if (!_is_open) return PerkSelectionAction::None;

    PerkSelectionAction action = PerkSelectionAction::None;

    float screen_width = static_cast<float>(engine::window::SCR_WIDTH);
    float screen_height = static_cast<float>(engine::window::SCR_HEIGHT);

    // Dim background overlay
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(screen_width, screen_height));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));
    ImGui::Begin("PerkOverlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoInputs);
    ImGui::End();
    ImGui::PopStyleColor();

    // Calculate total width needed for cards
    size_t num_cards = choices.size();
    float total_width = num_cards * CARD_WIDTH + (num_cards - 1) * CARD_GAP;
    float start_x = (screen_width - total_width) / 2.0f;
    float start_y = (screen_height - CARD_HEIGHT) / 2.0f + 40.0f;  // Offset for title

    // Title window
    float title_height = 80.0f;
    ImGui::SetNextWindowPos(ImVec2((screen_width - 400) / 2, start_y - title_height - 20));
    ImGui::SetNextWindowSize(ImVec2(400, title_height));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::Begin("PerkTitle", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoInputs);

    // Level up text
    ImGui::PushFont(nullptr);  // Use default font
    float title_text_width = ImGui::CalcTextSize("LEVEL UP!").x;
    ImGui::SetCursorPosX((400 - title_text_width) / 2);
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "LEVEL UP!");

    float sub_text_width = ImGui::CalcTextSize("Choose a perk").x;
    ImGui::SetCursorPosX((400 - sub_text_width) / 2);
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Choose a perk");
    ImGui::PopFont();

    ImGui::End();
    ImGui::PopStyleColor();

    // Render each perk card
    for (size_t i = 0; i < choices.size(); ++i) {
        const Perk* perk = choices[i];
        if (!perk) continue;

        float card_x = start_x + i * (CARD_WIDTH + CARD_GAP);
        float card_y = start_y;

        // Unique window name for each card
        char window_name[32];
        snprintf(window_name, sizeof(window_name), "PerkCard%zu", i);

        ImGui::SetNextWindowPos(ImVec2(card_x, card_y));
        ImGui::SetNextWindowSize(ImVec2(CARD_WIDTH, CARD_HEIGHT));

        // Card background color (darker for non-hovered)
        ImVec4 bg_color = ImVec4(0.12f, 0.12f, 0.15f, 0.95f);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, bg_color);
        ImGui::PushStyleColor(ImGuiCol_Border, get_category_color(perk->category));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));

        ImGuiWindowFlags card_flags = ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoScrollbar |
                                       ImGuiWindowFlags_NoCollapse;

        ImGui::Begin(window_name, nullptr, card_flags);

        // Category badge at top
        ImVec4 cat_color = get_category_color(perk->category);
        ImGui::PushStyleColor(ImGuiCol_Button, cat_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, cat_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, cat_color);

        float badge_width = ImGui::CalcTextSize(get_category_name(perk->category)).x + 20;
        ImGui::SetCursorPosX((CARD_WIDTH - badge_width - 30) / 2);
        ImGui::SmallButton(get_category_name(perk->category));
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        // Perk name (centered)
        float name_width = ImGui::CalcTextSize(perk->name.c_str()).x;
        ImGui::SetCursorPosX((CARD_WIDTH - name_width - 30) / 2);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", perk->name.c_str());

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        // Description (wrapped)
        ImGui::PushTextWrapPos(CARD_WIDTH - 30);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", perk->description.c_str());
        ImGui::PopTextWrapPos();

        // Unique badge if applicable
        if (perk->is_unique) {
            ImGui::Spacing();
            ImGui::Spacing();
            float unique_width = ImGui::CalcTextSize("UNIQUE").x;
            ImGui::SetCursorPosX((CARD_WIDTH - unique_width - 30) / 2);
            ImGui::TextColored(ImVec4(0.8f, 0.6f, 1.0f, 1.0f), "UNIQUE");
        }

        // Select button at bottom
        ImGui::SetCursorPosY(CARD_HEIGHT - 60);
        float button_width = CARD_WIDTH - 60;
        ImGui::SetCursorPosX((CARD_WIDTH - button_width - 30) / 2);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.7f, 1.0f));

        if (ImGui::Button("SELECT", ImVec2(button_width, 35))) {
            _selected_index = static_cast<int>(i);
            action = PerkSelectionAction::Selected;
        }
        ImGui::PopStyleColor(3);

        ImGui::End();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }

    return action;
}

}  // namespace main_game::ui
