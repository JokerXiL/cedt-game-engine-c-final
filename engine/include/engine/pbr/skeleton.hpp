#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace engine::pbr {

/// Node in the skeleton hierarchy (can be a bone or non-bone node)
struct SkeletonNode {
    std::string name;
    glm::mat4 transformation;  // Local transform of this node
    std::vector<SkeletonNode> children;
};

class Skeleton {
public:
    // Current bone transformations (global space, already multiplied by inverse bind pose)
    std::vector<glm::mat4> transforms;

    // Bind pose - inverse bind matrices (shared across instances)
    std::shared_ptr<std::vector<glm::mat4>> bindpose;

    // Bone name to index mapping
    std::shared_ptr<std::unordered_map<std::string, int>> bone_index_map;

    // Root node of the skeleton hierarchy tree
    std::shared_ptr<SkeletonNode> root_node;

    Skeleton() = default;

    explicit Skeleton(size_t bone_count)
        : transforms(bone_count, glm::mat4(1.0f)),
          bindpose(std::make_shared<std::vector<glm::mat4>>(bone_count, glm::mat4(1.0f))) {}

    Skeleton(size_t bone_count, std::shared_ptr<std::vector<glm::mat4>> shared_bindpose)
        : transforms(bone_count, glm::mat4(1.0f)),
          bindpose(shared_bindpose) {}

    size_t get_bone_count() const {
        return transforms.size();
    }

    // Get final bone matrices ready for shader upload
    const std::vector<glm::mat4>& get_final_transforms() const {
        return transforms;
    }

    void reset_transforms() {
        for (auto& transform : transforms) {
            transform = glm::mat4(1.0f);
        }
    }

    void set_bone_transform(size_t bone_index, const glm::mat4& transform) {
        if (bone_index < transforms.size()) {
            transforms[bone_index] = transform;
        }
    }

    const glm::mat4& get_bone_transform(size_t bone_index) const {
        static const glm::mat4 identity(1.0f);
        if (bone_index < transforms.size()) {
            return transforms[bone_index];
        }
        return identity;
    }

    int get_bone_index(const std::string& name) const {
        if (!bone_index_map) return -1;
        auto it = bone_index_map->find(name);
        return it != bone_index_map->end() ? it->second : -1;
    }

    void set_bone_transform(const std::string& name, const glm::mat4& transform) {
        int idx = get_bone_index(name);
        if (idx >= 0) {
            transforms[idx] = transform;
        }
    }
};

}  // namespace engine::pbr
