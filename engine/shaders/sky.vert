#version 330 core

// Fullscreen triangle vertex shader
// Uses vertex ID to generate positions without any vertex buffers

out vec2 TexCoords;
out vec3 ViewDir;

uniform mat4 inverseProjection;
uniform mat4 inverseView;

void main() {
    // Generate fullscreen triangle using vertex ID
    // This creates a triangle that covers the entire screen:
    // ID 0: (-1, -1)  ID 1: (3, -1)  ID 2: (-1, 3)
    vec2 pos = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    pos = pos * 2.0 - 1.0;

    TexCoords = pos * 0.5 + 0.5;

    // Calculate view direction in world space
    // Unproject from clip space to view space
    vec4 clipPos = vec4(pos, 1.0, 1.0);
    vec4 viewPos = inverseProjection * clipPos;
    viewPos.xyz /= viewPos.w;  // Perspective divide
    viewPos.w = 0.0;  // Direction, not position

    // Transform to world space
    ViewDir = (inverseView * viewPos).xyz;

    gl_Position = vec4(pos, 0.9999, 1.0);  // Near far plane
}
