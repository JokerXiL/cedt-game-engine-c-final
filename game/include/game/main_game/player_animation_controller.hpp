#pragma once

#include <engine/pbr/skeleton.hpp>
#include <engine/pbr/animation.hpp>

#include <memory>

namespace main_game {

/// Animation state types for the player
enum class PlayerAnimationType {
    Idle,
    Running,
    MeleeAttack,
    RangeAttack
};

/// Controls player animation state machine and transitions
class PlayerAnimationController {
public:
    PlayerAnimationController() = default;

    /// Set the animation clips used by the controller
    void set_clips(
        std::shared_ptr<engine::pbr::AnimationClip> idle,
        std::shared_ptr<engine::pbr::AnimationClip> run,
        std::shared_ptr<engine::pbr::AnimationClip> melee_attack,
        std::shared_ptr<engine::pbr::AnimationClip> range_attack);

    /// Update animation based on player state
    /// @param delta Time since last frame
    /// @param is_moving Whether the player is currently moving
    /// @param melee_cooldown_active Whether melee attack cooldown is active
    /// @param range_cooldown_active Whether range attack cooldown is active
    void update(float delta, bool is_moving, bool melee_cooldown_active, bool range_cooldown_active);

    /// Apply current animation to skeleton
    void apply(engine::pbr::Skeleton& skeleton);

    /// Trigger melee attack animation
    void trigger_melee_attack();

    /// Trigger range attack animation
    void trigger_range_attack();

    /// Get current animation type
    PlayerAnimationType current_type() const { return _current_type; }

    /// Check if an attack animation is playing
    bool is_attack_playing() const { return _is_attack_playing; }

private:
    void transition_to(PlayerAnimationType new_type);
    std::shared_ptr<engine::pbr::AnimationClip> get_clip_for_type(PlayerAnimationType type) const;
    bool should_loop(PlayerAnimationType type) const;

    engine::pbr::AnimationState _state;
    PlayerAnimationType _current_type = PlayerAnimationType::Idle;
    bool _is_attack_playing = false;

    // Animation clips
    std::shared_ptr<engine::pbr::AnimationClip> _idle;
    std::shared_ptr<engine::pbr::AnimationClip> _run;
    std::shared_ptr<engine::pbr::AnimationClip> _melee_attack;
    std::shared_ptr<engine::pbr::AnimationClip> _range_attack;
};

}  // namespace main_game
