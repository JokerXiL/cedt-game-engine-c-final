#include <game/main_game.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>

#include <game/game_summary.hpp>
#include <game/main_game/game_state.hpp>
#include <game/main_game/renderer.hpp>
#include <game/main_game/ui/game_hud.hpp>
#include <game/main_game/ui/pause_menu.hpp>
#include <game/main_game/ui/perk_selection.hpp>
#include <engine/window/window_system.hpp>
#include <engine/input/input_system.hpp>
#include <engine/input/key_codes.hpp>

namespace main_game {

std::unique_ptr<State> MainGame::run() {
    auto& window_system = engine::window::WindowSystem::get_instance();
    auto& input_system = engine::input::InputSystem::get_instance();

    GameState game_state;
    game_state.start_game();  // Start the wave system

    // Set up camera projection
    float aspect = static_cast<float>(engine::window::SCR_WIDTH) / static_cast<float>(engine::window::SCR_HEIGHT);
    game_state.camera.orbit_camera().set_projection(glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f));

    Renderer renderer;
    game_state.set_renderer(&renderer);

    // UI components
    ui::GameHUD hud;
    ui::PauseMenu pause_menu;
    ui::PerkSelection perk_selection;

    float delta_time = 0.0f;
    float last_frame = 0.0f;

    while (true) {
        // Poll windows and input
        window_system.poll_events();

        // Check for window close
        if (window_system.should_close()) {
            break;
        }

        // Calculate delta time
        float current_frame = static_cast<float>(window_system.get_time());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // Check for level up (open perk selection)
        if (game_state.progression_system.is_level_up_pending() && !perk_selection.is_open()) {
            perk_selection.open();
            // Release mouse first, then center cursor to prevent accidental perk selection
            input_system.release_mouse();
            input_system.center_cursor();
            input_system.clear_mouse_button_states();
        }

        // Handle UI input (only pause menu toggle when not selecting perk)
        if (input_system.is_key_just_pressed(engine::input::KeyCode::Escape) && !perk_selection.is_open()) {
            pause_menu.toggle();
        }

        // Toggle mouse capture based on UI state
        if (pause_menu.is_open() || perk_selection.is_open()) {
            input_system.release_mouse();
        } else {
            input_system.capture_mouse();
        }

        // Process input and update game (skip if paused or selecting perk)
        if (!pause_menu.is_open() && !perk_selection.is_open()) {
            game_state.process_input();
            game_state.update(delta_time);
            renderer.update(delta_time);
        }

        // Track UI actions for after render
        ui::PauseAction pause_action = ui::PauseAction::None;
        ui::PerkSelectionAction perk_action = ui::PerkSelectionAction::None;

        // Render everything (3D + UI) through the pipeline
        renderer.render(game_state, [&]() {
            if (perk_selection.is_open()) {
                perk_action = perk_selection.render(
                    game_state.progression_system.get_perk_choices());
            } else if (pause_menu.is_open()) {
                pause_action = pause_menu.render();
            } else {
                hud.render(game_state);
            }
        });

        // Handle perk selection
        if (perk_action == ui::PerkSelectionAction::Selected) {
            game_state.progression_system.apply_perk(
                game_state.player, perk_selection.selected_index());
            perk_selection.close();
        }

        // Handle pause menu actions
        if (pause_action == ui::PauseAction::Quit) {
            break;
        }
        if (pause_action == ui::PauseAction::Resume) {
            pause_menu.close();
        }

        input_system.end_frame();
    }

    auto result = std::make_unique<game_summary::GameSummary>();
    return result;
}

}  // namespace main_game
