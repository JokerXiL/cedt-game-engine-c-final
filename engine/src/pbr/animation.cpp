#include <engine/pbr/animation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <cmath>

namespace engine::pbr {

namespace {

/// Find the index of the keyframe just before the given time
template<typename KeyType>
size_t find_key_index(const std::vector<KeyType>& keys, float time) {
    if (keys.empty()) return 0;

    for (size_t i = 0; i < keys.size() - 1; ++i) {
        if (time < keys[i + 1].time) {
            return i;
        }
    }
    return keys.size() - 1;
}

/// Linear interpolation factor between two keyframes
float get_interpolation_factor(float time, float time0, float time1) {
    if (time1 <= time0) return 0.0f;
    return (time - time0) / (time1 - time0);
}

/// Recursively calculate bone transforms in the skeleton hierarchy
void calculate_bone_transform(
    Skeleton& skeleton,
    const SkeletonNode* node,
    const glm::mat4& parent_transform,
    const std::unordered_map<std::string, size_t>& channel_cache,
    const std::vector<AnimationChannel>& channels,
    float animation_time)
{
    if (!node) return;

    glm::mat4 node_transform = node->transformation;

    // Use animated transform if available (using cached index lookup)
    auto it = channel_cache.find(node->name);
    if (it != channel_cache.end()) {
        node_transform = channels[it->second].evaluate(animation_time);
    }

    glm::mat4 global_transform = parent_transform * node_transform;

    // If this node is a bone, update its transform
    int bone_index = skeleton.get_bone_index(node->name);
    if (bone_index >= 0) {
        skeleton.set_bone_transform(bone_index, global_transform * (*skeleton.bindpose)[bone_index]);
    }

    // Process children
    for (const auto& child : node->children) {
        calculate_bone_transform(skeleton, &child, global_transform, channel_cache, channels, animation_time);
    }
}

}  // namespace

// ============================================================================
// AnimationChannel
// ============================================================================

glm::vec3 AnimationChannel::interpolate_position(float time) const {
    if (position_keys.empty()) {
        return glm::vec3(0.0f);
    }

    if (position_keys.size() == 1) {
        return position_keys[0].value;
    }

    size_t index = find_key_index(position_keys, time);

    if (index >= position_keys.size() - 1) {
        return position_keys.back().value;
    }

    const auto& key0 = position_keys[index];
    const auto& key1 = position_keys[index + 1];
    float factor = get_interpolation_factor(time, key0.time, key1.time);

    return glm::mix(key0.value, key1.value, factor);
}

glm::quat AnimationChannel::interpolate_rotation(float time) const {
    if (rotation_keys.empty()) {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    if (rotation_keys.size() == 1) {
        return rotation_keys[0].value;
    }

    size_t index = find_key_index(rotation_keys, time);

    if (index >= rotation_keys.size() - 1) {
        return rotation_keys.back().value;
    }

    const auto& key0 = rotation_keys[index];
    const auto& key1 = rotation_keys[index + 1];
    float factor = get_interpolation_factor(time, key0.time, key1.time);

    return glm::slerp(key0.value, key1.value, factor);
}

glm::vec3 AnimationChannel::interpolate_scale(float time) const {
    if (scale_keys.empty()) {
        return glm::vec3(1.0f);
    }

    if (scale_keys.size() == 1) {
        return scale_keys[0].value;
    }

    size_t index = find_key_index(scale_keys, time);

    if (index >= scale_keys.size() - 1) {
        return scale_keys.back().value;
    }

    const auto& key0 = scale_keys[index];
    const auto& key1 = scale_keys[index + 1];
    float factor = get_interpolation_factor(time, key0.time, key1.time);

    return glm::mix(key0.value, key1.value, factor);
}

glm::mat4 AnimationChannel::evaluate(float time) const {
    glm::vec3 position = interpolate_position(time);
    glm::quat rotation = interpolate_rotation(time);
    glm::vec3 scale = interpolate_scale(time);

    // Build transformation matrix: T * R * S
    glm::mat4 transform = glm::scale(glm::mat4(1.0f), scale);
    transform = glm::toMat4(rotation) * transform;
    transform = glm::translate(glm::mat4(1.0f), position) * transform;

    return transform;
}

// ============================================================================
// AnimationClip
// ============================================================================

void AnimationClip::build_cache(const Skeleton& /*skeleton*/) const {
    if (_cache_built) return;

    _channel_index_cache.clear();
    for (size_t i = 0; i < _channels.size(); ++i) {
        _channel_index_cache[_channels[i].bone_name] = i;
    }
    _cache_built = true;
}

void AnimationClip::apply(Skeleton& skeleton, float time, float /*blend_factor*/) const {
    if (_channels.empty() || !skeleton.bone_index_map) {
        return;
    }

    // Build cache lazily on first use
    if (!_cache_built) {
        build_cache(skeleton);
    }

    // Wrap time to animation duration
    float animation_time = time;
    if (_duration > 0.0f) {
        animation_time = std::fmod(time, _duration);
        if (animation_time < 0.0f) {
            animation_time += _duration;
        }
    }

    calculate_bone_transform(skeleton, skeleton.root_node.get(), glm::mat4(1.0f),
                            _channel_index_cache, _channels, animation_time);
}

// ============================================================================
// AnimationState
// ============================================================================

void AnimationState::update(float delta_time) {
    if (!_playing || !_clip) {
        return;
    }

    float ticks_per_second = _clip->get_ticks_per_second();
    if (ticks_per_second <= 0.0f) {
        ticks_per_second = 25.0f;
    }

    float delta_ticks = delta_time * ticks_per_second * _speed;
    _current_time += delta_ticks;

    float duration = _clip->get_duration();
    if (duration > 0.0f) {
        if (_looping) {
            _current_time = std::fmod(_current_time, duration);
            if (_current_time < 0.0f) {
                _current_time += duration;
            }
        } else {
            if (_current_time >= duration) {
                _current_time = duration;
                _playing = false;
            } else if (_current_time < 0.0f) {
                _current_time = 0.0f;
                _playing = false;
            }
        }
    }
}

void AnimationState::apply(Skeleton& skeleton) const {
    if (_clip) {
        _clip->apply(skeleton, _current_time);
    }
}

float AnimationState::get_progress() const {
    if (!_clip) return 0.0f;

    float duration = _clip->get_duration();
    if (duration <= 0.0f) return 0.0f;

    return _current_time / duration;
}

}  // namespace engine::pbr
