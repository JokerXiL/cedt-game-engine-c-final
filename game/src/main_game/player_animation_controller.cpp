#include <game/main_game/player_animation_controller.hpp>

namespace main_game {

void PlayerAnimationController::set_clips(
    std::shared_ptr<engine::pbr::AnimationClip> idle,
    std::shared_ptr<engine::pbr::AnimationClip> run,
    std::shared_ptr<engine::pbr::AnimationClip> melee_attack,
    std::shared_ptr<engine::pbr::AnimationClip> range_attack)
{
    _idle = idle;
    _run = run;
    _melee_attack = melee_attack;
    _range_attack = range_attack;

    // Start with idle animation
    if (_idle) {
        _state.set_clip(_idle);
        _state.set_looping(true);
        _state.play();
    }
}

void PlayerAnimationController::update(
    float delta,
    bool is_moving,
    bool melee_cooldown_active,
    bool range_cooldown_active)
{
    // Check if attack animation finished
    if (_is_attack_playing) {
        bool attack_finished = false;
        if (_current_type == PlayerAnimationType::MeleeAttack) {
            attack_finished = !melee_cooldown_active;
        } else if (_current_type == PlayerAnimationType::RangeAttack) {
            attack_finished = !range_cooldown_active;
        }

        if (attack_finished) {
            _is_attack_playing = false;
            transition_to(is_moving ? PlayerAnimationType::Running : PlayerAnimationType::Idle);
        }
    } else {
        // Update based on movement when not attacking
        if (is_moving && _current_type != PlayerAnimationType::Running) {
            transition_to(PlayerAnimationType::Running);
        } else if (!is_moving && _current_type != PlayerAnimationType::Idle) {
            transition_to(PlayerAnimationType::Idle);
        }
    }

    // Update animation time
    _state.update(delta);
}

void PlayerAnimationController::apply(engine::pbr::Skeleton& skeleton) {
    if (_state.clip()) {
        _state.apply(skeleton);
    }
}

void PlayerAnimationController::trigger_melee_attack() {
    _is_attack_playing = true;
    transition_to(PlayerAnimationType::MeleeAttack);
}

void PlayerAnimationController::trigger_range_attack() {
    _is_attack_playing = true;
    transition_to(PlayerAnimationType::RangeAttack);
}

void PlayerAnimationController::transition_to(PlayerAnimationType new_type) {
    if (_current_type == new_type) return;

    _current_type = new_type;
    auto clip = get_clip_for_type(new_type);

    if (clip) {
        // Use crossfade for smoother transitions
        constexpr float BLEND_DURATION = 0.15f;
        _state.crossfade_to(clip, BLEND_DURATION, should_loop(new_type));
    }
}

std::shared_ptr<engine::pbr::AnimationClip> PlayerAnimationController::get_clip_for_type(
    PlayerAnimationType type) const
{
    switch (type) {
        case PlayerAnimationType::Idle:       return _idle;
        case PlayerAnimationType::Running:    return _run;
        case PlayerAnimationType::MeleeAttack: return _melee_attack;
        case PlayerAnimationType::RangeAttack: return _range_attack;
    }
    return nullptr;
}

bool PlayerAnimationController::should_loop(PlayerAnimationType type) const {
    return type == PlayerAnimationType::Idle || type == PlayerAnimationType::Running;
}

}  // namespace main_game
