#pragma once

#include <vector>

namespace main_game {
struct Perk;
}

namespace main_game::ui {

enum class PerkSelectionAction { None, Selected };

class PerkSelection {
public:
    PerkSelectionAction render(const std::vector<const Perk*>& choices);

    bool is_open() const { return _is_open; }
    void open() { _is_open = true; }
    void close() { _is_open = false; _selected_index = -1; }

    int selected_index() const { return _selected_index; }

private:
    bool _is_open = false;
    int _selected_index = -1;
};

}  // namespace main_game::ui
