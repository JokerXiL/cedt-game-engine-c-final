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
    // Current bone transformations (global space)
    std::vector<glm::mat4> transforms;

    // Bind pose - inverse bind matrices (shared across instances)
    // Multiple skeletons can share the same bind pose
    std::shared_ptr<std::vector<glm::mat4>> bindpose;

    // Map<bone_name , index>
    std::shared_ptr<std::unordered_map<std::string, int>> bone_index_map;
    
    // Root node of the skeleton hierarchy tree
    std::shared_ptr<SkeletonNode> root_node;
    
    // DEPRECATED: Bone hierarchy data (kept for compatibility)
    // Bone hierarchy: parent index for each bone (-1 for root bones)
    std::shared_ptr<std::vector<int>> parent_indices;
    
    // Bone hierarchy: children indices for each bone
    std::shared_ptr<std::vector<std::vector<int>>> children_indices;
    
    // Node transformations from the original model (for bones without animation)
    std::shared_ptr<std::vector<glm::mat4>> node_transforms; 

    // Default constructor
    Skeleton() = default;

    // Constructor with bone count
    explicit Skeleton(size_t bone_count)
        : transforms(bone_count, glm::mat4(1.0f)),
          bindpose(std::make_shared<std::vector<glm::mat4>>(bone_count, glm::mat4(1.0f))) {}

    // Constructor with shared bind pose
    Skeleton(size_t bone_count, std::shared_ptr<std::vector<glm::mat4>> shared_bindpose)
        : transforms(bone_count, glm::mat4(1.0f)),
          bindpose(shared_bindpose) {}

    // Get the number of bones
    size_t get_bone_count() const {
        return transforms.size();
    }

    // Get final bone matrices (transform * inverse bind pose)
    std::vector<glm::mat4> get_final_transforms() const {
        // std::vector<glm::mat4> final_transforms(transforms.size());
        // for (size_t i = 0; i < transforms.size(); ++i) {
            // if (bindpose && i < bindpose->size()) {
                // final_transforms[i] = transforms[i] * (*bindpose)[i];
            // } else {
                // final_transforms[i] = transforms[i];
            // }
        // }
        return transforms;
    }

    // Set all transforms to identity
    void reset_transforms() {
        for (auto& transform : transforms) {
            transform = glm::mat4(1.0f);
        }
    }

    // Set a specific bone transform
    void set_bone_transform(size_t bone_index, const glm::mat4& transform) {
        if (bone_index < transforms.size()) {
            transforms[bone_index] = transform;
        }
    }

    // Get a specific bone transform
    const glm::mat4& get_bone_transform(size_t bone_index) const {
        static const glm::mat4 identity(1.0f);
        if (bone_index < transforms.size()) {
            return transforms[bone_index];
        }
        return identity;
    }

        // Get bone index by name
    int get_bone_index(const std::string& name) const {
        if (!bone_index_map) return -1;
        auto it = bone_index_map->find(name);
        return it != bone_index_map->end() ? it->second : -1;
    }

    // Set bone transform by name
    void set_bone_transform(const std::string& name, const glm::mat4& transform) {
        int idx = get_bone_index(name);
        if (idx >= 0) {
            transforms[idx] = transform;
        }
    }
};

}  // namespace engine::pbr
