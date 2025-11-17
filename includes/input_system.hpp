#pragma once

class InputSystem {
public:
    // Singleton instance
    static InputSystem& get_instance();

    void init();

    // Key state
    bool is_key_pressed(int key) const {
        if (key >= 0 && key < 1024) return _keys[key];
        return false;
    }
    void set_key_state(int key, bool pressed) {
        if (key >= 0 && key < 1024) _keys[key] = pressed;
    }

    // Mouse state
    double mouse_x() const { return _mouse_x; }
    double mouse_y() const { return _mouse_y; }
    double last_mouse_x() const { return _last_mouse_x; }
    double last_mouse_y() const { return _last_mouse_y; }
    void update_mouse_position(double xpos, double ypos) {
        _last_mouse_x = _mouse_x;
        _last_mouse_y = _mouse_y;
        _mouse_x = xpos;
        _mouse_y = ypos;
    }

    // Mouse buttons
    bool is_right_mouse_pressed() const { return _right_mouse_pressed; }
    void set_right_mouse_pressed(bool pressed) {
        _right_mouse_pressed = pressed;
    }
    bool is_first_mouse_move() const { return _first_mouse_move; }
    void set_first_mouse_move(bool first) { _first_mouse_move = first; }

private:
    InputSystem() = default;
    ~InputSystem() = default;
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

    bool _keys[1024] = {false};
    double _mouse_x = 0.0;
    double _mouse_y = 0.0;
    double _last_mouse_x = 0.0;
    double _last_mouse_y = 0.0;
    bool _right_mouse_pressed = false;
    bool _first_mouse_move = true;
};
