#pragma once

#include <glm/glm.hpp>
#include <memory>

// Forward declarations
namespace engine {
class Mesh;
class Material;
class Skeleton;
}  // namespace engine

// Simple Model class that combines a Mesh with a Material
// Provides a convenient interface for rendering objects
class Model {
public:
    Model(std::shared_ptr<engine::Mesh> mesh, std::shared_ptr<engine::Material> material)
        : _mesh(mesh), _material(material) {}

    // Draw the model with the given transformation
    void draw(const glm::mat4& transform);

    // Draw the model with skeletal animation
    void draw_skinned(const glm::mat4& transform, const engine::Skeleton& skeleton);

    // Getters
    std::shared_ptr<engine::Mesh> mesh() const { return _mesh; }
    std::shared_ptr<engine::Material> material() const { return _material; }

    // Setters
    void set_mesh(std::shared_ptr<engine::Mesh> mesh) { _mesh = mesh; }
    void set_material(std::shared_ptr<engine::Material> material) { _material = material; }

private:
    std::shared_ptr<engine::Mesh> _mesh;
    std::shared_ptr<engine::Material> _material;
};

// Future: StaticModel and SkeletalModel can be added here
// They would extend Model with additional features like:
// - Model loading from files (Assimp integration)
// - Animation support (for SkeletalModel)
// - Multiple meshes / sub-meshes
// - Material management