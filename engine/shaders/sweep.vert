#version 330 core

layout(location = 0) in vec3 aPos;

out float AngleNormalized;  // 0 to 1 across the arc
out float DistFromCenter;   // 0 at center, 1 at edge

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const float PI = 3.14159265359;

void main() {
    // Calculate distance from center (0,0) in local XZ space
    DistFromCenter = length(aPos.xz);

    // Calculate angle from local position
    // atan2 returns angle in radians, arc is 90 degrees centered at +Z
    // Arc goes from -45 degrees to +45 degrees (centered on Z axis)
    float angle = atan(aPos.x, aPos.z);  // atan2(x, z) gives angle from +Z axis

    // Normalize to 0-1 range: -PI/4 to +PI/4 -> 0 to 1
    // -45 deg = -PI/4, +45 deg = +PI/4
    AngleNormalized = (angle + PI * 0.25) / (PI * 0.5);
    AngleNormalized = clamp(AngleNormalized, 0.0, 1.0);

    gl_Position = projection * view * model * vec4(aPos + vec3(0.0,0.5,0.0), 1.0);
}
