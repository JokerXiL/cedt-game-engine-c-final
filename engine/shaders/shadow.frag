#version 330 core

// Depth is written automatically by the rasterizer
// This shader is intentionally empty for depth-only rendering

void main() {
    // gl_FragDepth = gl_FragCoord.z; // Implicit
}
