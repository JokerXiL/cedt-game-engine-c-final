#include <engine/pbr/animation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <iostream>

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
    
    // If we're at or past the last keyframe, return the last value
    if (index >= position_keys.size() - 1) {
        return position_keys.back().value;
    }
    
    // Linear interpolation between keyframes
    const auto& key0 = position_keys[index];
    const auto& key1 = position_keys[index + 1];
    float factor = get_interpolation_factor(time, key0.time, key1.time);
    
    return glm::mix(key0.value, key1.value, factor);
}

glm::quat AnimationChannel::interpolate_rotation(float time) const {
    if (rotation_keys.empty()) {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // Identity quaternion
    }
    
    if (rotation_keys.size() == 1) {
        return rotation_keys[0].value;
    }
    
    size_t index = find_key_index(rotation_keys, time);
    
    // If we're at or past the last keyframe, return the last value
    if (index >= rotation_keys.size() - 1) {
        return rotation_keys.back().value;
    }
    
    // Spherical linear interpolation (slerp) between keyframes
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
    
    // If we're at or past the last keyframe, return the last value
    if (index >= scale_keys.size() - 1) {
        return scale_keys.back().value;
    }
    
    // Linear interpolation between keyframes
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
    // First create scale matrix
    glm::mat4 transform = glm::scale(glm::mat4(1.0f), scale);
    // Then apply rotation
    transform = glm::toMat4(rotation) * transform;
    // Finally apply translation
    transform = glm::translate(glm::mat4(1.0f), position) * transform;
    
    return transform;
}

// ============================================================================
// AnimationClip
// ============================================================================

/// Recursively calculate bone transforms in the skeleton hierarchy
/// This traverses the skeleton tree and applies animation transforms
static void calculate_bone_transform(
    Skeleton& skeleton,
    const SkeletonNode* node,
    const glm::mat4& parent_transform,
    const std::unordered_map<std::string, glm::mat4>& animated_transforms)
{
    if (!node) return;
    
    std::string node_name = node->name;
    glm::mat4 node_transform = node->transformation;
    
    // Check if this node has an animated transform
    auto it = animated_transforms.find(node_name);
    if (it != animated_transforms.end()) {
        node_transform = it->second;
    }
    
    // Calculate global transformation
    glm::mat4 global_transformation = parent_transform * node_transform;
    // If this node is a bone, update its transform in the skeleton
    int bone_index = skeleton.get_bone_index(node_name);
    if (bone_index >= 0) {
        
        skeleton.set_bone_transform(bone_index, global_transformation * (*skeleton.bindpose)[bone_index]);
    }
    
    // Recursively process all children
    for (const auto& child : node->children) {
        calculate_bone_transform(skeleton, &child, global_transformation, animated_transforms);
    }
}

void AnimationClip::apply(Skeleton& skeleton, float time, float blend_factor) const {
    if (_channels.empty() || !skeleton.bone_index_map) {
        return;
    }

    // Wrap time to animation duration if looping
    float animation_time = time;
    if (_duration > 0.0f) {
        animation_time = std::fmod(time, _duration);
        if (animation_time < 0.0f) {
            animation_time += _duration;
        }
    }
    
    // Build map of animated transforms for this time
    std::unordered_map<std::string, glm::mat4> animated_transforms;
    for (const auto& channel : _channels) {
        animated_transforms[channel.bone_name] = channel.evaluate(animation_time);
    }
    
    if (blend_factor >= 1.0f) {
        // Full animation - apply hierarchically
        calculate_bone_transform(skeleton, skeleton.root_node.get(), glm::mat4(1.0f), animated_transforms);
    } else if (blend_factor > 0.0f) {
      
    }
}

// ============================================================================
// AnimationState
// ============================================================================

void AnimationState::update(float delta_time) {
    if (!_playing || !_clip) {
        return;
    }
    // Convert delta time to ticks
    float ticks_per_second = _clip->get_ticks_per_second();
    if (ticks_per_second <= 0.0f) {
        ticks_per_second = 25.0f;  // Default fallback
    }
    
    float delta_ticks = delta_time * ticks_per_second * _speed;
    _current_time += delta_ticks;
    
    // Handle looping or clamping
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
                _playing = false;  // Stop at end if not looping
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
