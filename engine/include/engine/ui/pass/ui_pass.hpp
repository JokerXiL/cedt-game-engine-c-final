#pragma once

#include <engine/render/render_graph.hpp>

#include <functional>

namespace engine::ui {

/// UI rendering pass (ImGui)
/// Reads: color_buffer (renders on top)
class UIPass : public RenderPass {
public:
    UIPass();

    void execute() override;

    /// Set the UI callback for this frame
    void set_callback(std::function<void()> callback) {
        _callback = std::move(callback);
    }

private:
    std::function<void()> _callback;
};

}  // namespace engine::ui
