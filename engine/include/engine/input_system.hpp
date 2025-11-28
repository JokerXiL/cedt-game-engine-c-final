#pragma once

namespace engine {

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

    // Mouse state
    double mouse_x() const { return _mouse_x; }
    double mouse_y() const { return _mouse_y; }
    double last_mouse_x() const { return _last_mouse_x; }
    double last_mouse_y() const { return _last_mouse_y; }

    // Mouse capture
    void capture_mouse();
    void release_mouse();

    // Get mouse delta and reset for next frame
    void get_mouse_delta(float& dx, float& dy);

private:
    InputSystem() = default;
    ~InputSystem() = default;
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

    void set_key_state(int key, bool pressed) {
        if (key >= 0 && key < 1024) _keys[key] = pressed;
    }

    void update_mouse_position(double xpos, double ypos) {
        _last_mouse_x = _mouse_x;
        _last_mouse_y = _mouse_y;
        _mouse_x = xpos;
        _mouse_y = ypos;
    }

    bool _keys[1024] = {false};
    double _mouse_x = 0.0;
    double _mouse_y = 0.0;
    double _last_mouse_x = 0.0;
    double _last_mouse_y = 0.0;
};

}  // namespace engine
