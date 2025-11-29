#include <engine/ui/pass/ui_pass.hpp>
#include <engine/ui/ui_system.hpp>
#include <engine/pbr/pass/pbr_render_pass.hpp>

namespace engine::ui {

UIPass::UIPass() : RenderPass("ui") {
    reads_resource(pbr::RenderResource::ColorBuffer);
}

void UIPass::execute() {
    if (!_callback) return;

    auto& ui_system = UISystem::get_instance();
    ui_system.begin_frame();
    _callback();
    ui_system.end_frame();

    // Clear callback after use (one-shot per frame)
    _callback = nullptr;
}

}  // namespace engine::ui
