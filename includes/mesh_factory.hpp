#pragma once

#include "mesh.hpp"

#include <memory>

namespace mesh_factory {

// Create a cube mesh (1x1x1, centered at origin)
std::unique_ptr<Mesh> create_cube(float size = 1.0f);

// Create a plane mesh (extends in XZ plane, centered at origin)
std::unique_ptr<Mesh> create_plane(float width = 1.0f, float height = 1.0f);

// Create a sphere mesh
std::unique_ptr<Mesh> create_sphere(float radius = 1.0f, int segments = 32);

}  // namespace mesh_factory
