#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace engine::pbr {

class Skeleton {
public:
    // Current bone transformations (model space or bone space)
    std::vector<glm::mat4> transforms;

    // Bind pose - inverse bind matrices (shared across instances)
    // Multiple skeletons can share the same bind pose
    std::shared_ptr<std::vector<glm::mat4>> bindpose;

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
        std::vector<glm::mat4> final_transforms(transforms.size());
        for (size_t i = 0; i < transforms.size(); ++i) {
            if (bindpose && i < bindpose->size()) {
                final_transforms[i] = transforms[i] * (*bindpose)[i];
            } else {
                final_transforms[i] = transforms[i];
            }
        }
        return final_transforms;
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
};

}  // namespace engine::pbr
