class InputSystem {
public:
    // Singleton instance
    static InputSystem& getInstance();

    // Initialize with GLFW window
    void init(GLFWwindow* window);

    // Key state
    bool isKeyPressed(int key) const;
    void setKeyState(int key, bool pressed);

    // Mouse state
    double getMouseX() const { return mouseX; }
    double getMouseY() const { return mouseY; }
    double getLastMouseX() const { return lastMouseX; }
    double getLastMouseY() const { return lastMouseY; }
    void updateMousePosition(double xpos, double ypos);

    // Mouse buttons
    bool isRightMousePressed() const { return rightMousePressed; }
    void setRightMousePressed(bool pressed) { rightMousePressed = pressed; }
    bool isFirstMouseMove() const { return firstMouseMove; }
    void setFirstMouseMove(bool first) { firstMouseMove = first; }

private:
    InputSystem() = default;
    ~InputSystem() = default;
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

    bool keys[1024] = {false};
    double mouseX = 0.0;
    double mouseY = 0.0;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool rightMousePressed = false;
    bool firstMouseMove = true;
};
