#pragma once

namespace engine::ui {

class UISystem {
public:
    static UISystem& get_instance();

    void begin_frame();
    void end_frame();

private:
    UISystem();
    ~UISystem();
    UISystem(const UISystem&) = delete;
    UISystem& operator=(const UISystem&) = delete;

    void apply_game_style();
};

}  // namespace engine::ui
