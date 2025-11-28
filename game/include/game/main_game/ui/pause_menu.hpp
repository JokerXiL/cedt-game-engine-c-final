#pragma once

namespace main_game::ui {

enum class PauseAction { None, Resume, Settings, Quit };

class PauseMenu {
public:
    PauseAction render();
    bool is_open() const { return _is_open; }
    void toggle() { _is_open = !_is_open; }
    void open() { _is_open = true; }
    void close() { _is_open = false; }

private:
    bool _is_open = false;
};

}  // namespace main_game::ui
