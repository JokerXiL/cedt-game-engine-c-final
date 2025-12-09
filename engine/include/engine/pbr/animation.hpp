#pragma once

#include <engine/pbr/skeleton.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace engine::pbr {

/// A keyframe for position animation
struct PositionKey {
    float time;
    glm::vec3 value;
};

/// A keyframe for rotation animation
struct RotationKey {
    float time;
    glm::quat value;
};

/// A keyframe for scale animation
struct ScaleKey {
    float time;
    glm::vec3 value;
};

/// Animation channel for a single bone
struct AnimationChannel {
    std::string bone_name;
    std::vector<PositionKey> position_keys;
    std::vector<RotationKey> rotation_keys;
    std::vector<ScaleKey> scale_keys;

    /// Evaluate the channel at a given time and return the transformation matrix
    glm::mat4 evaluate(float time) const;

private:
    glm::vec3 interpolate_position(float time) const;
    glm::quat interpolate_rotation(float time) const;
    glm::vec3 interpolate_scale(float time) const;
};

/// An animation clip containing multiple channels
class AnimationClip {
public:
    AnimationClip() = default;

    // Getters
    const std::string& name() const { return _name; }
    float duration() const { return _duration; }
    float ticks_per_second() const { return _ticks_per_second; }
    const std::vector<AnimationChannel>& channels() const { return _channels; }

    // Setters (for loading)
    void set_name(std::string name) { _name = std::move(name); }
    void set_duration(float duration) { _duration = duration; }
    void set_ticks_per_second(float tps) { _ticks_per_second = tps; }
    void add_channel(AnimationChannel channel) {
        _channels.push_back(std::move(channel));
        _cache_built = false;
    }

    /// Apply animation to a skeleton at the given time
    void apply(Skeleton& skeleton, float time) const;

    /// Sample bone transforms at given time (for blending)
    void sample(float time, std::vector<glm::mat4>& out_transforms, const Skeleton& skeleton) const;

    /// Build channel-to-node cache for a skeleton (call once per skeleton)
    void build_cache(const Skeleton& skeleton) const;

private:
    std::string _name;
    float _duration = 0.0f;
    float _ticks_per_second = 25.0f;
    std::vector<AnimationChannel> _channels;

    // Cached channel name to index mapping (built lazily)
    mutable std::unordered_map<std::string, size_t> _channel_index_cache;
    mutable bool _cache_built = false;
};

/// Animation state machine for controlling animation playback
class AnimationState {
public:
    AnimationState() = default;

    /// Set the animation clip to play (no blending)
    void set_clip(std::shared_ptr<AnimationClip> clip) {
        _clip = clip;
        _current_time = 0.0f;
    }

    /// Crossfade to a new animation clip over the specified duration
    void crossfade_to(std::shared_ptr<AnimationClip> clip, float duration, bool loop = true);

    /// Get current animation clip
    std::shared_ptr<AnimationClip> clip() const { return _clip; }

    /// Update animation state
    void update(float delta_time);

    /// Apply current animation state to skeleton
    void apply(Skeleton& skeleton) const;

    /// Play the animation
    void play() { _playing = true; }

    /// Pause the animation
    void pause() { _playing = false; }

    /// Stop and reset the animation
    void stop() {
        _playing = false;
        _current_time = 0.0f;
    }

    /// Check if animation is playing
    bool is_playing() const { return _playing; }

    /// Set whether animation should loop
    void set_looping(bool loop) { _looping = loop; }

    /// Check if animation is looping
    bool is_looping() const { return _looping; }

    /// Set playback speed
    void set_speed(float speed) { _speed = speed; }
    float speed() const { return _speed; }

    /// Current time in animation
    void set_current_time(float time) { _current_time = time; }
    float current_time() const { return _current_time; }

    /// Get animation progress (0.0 to 1.0)
    float progress() const;

    /// Check if currently blending between animations
    bool is_blending() const { return _blend_factor < 1.0f; }

private:
    std::shared_ptr<AnimationClip> _clip;
    float _current_time = 0.0f;
    float _speed = 1.0f;
    bool _playing = false;
    bool _looping = true;

    // Blending state
    std::shared_ptr<AnimationClip> _prev_clip;
    float _prev_time = 0.0f;
    bool _prev_looping = true;
    float _blend_factor = 1.0f;  // 0 = fully prev, 1 = fully current
    float _blend_duration = 0.0f;
};

}  // namespace engine::pbr
