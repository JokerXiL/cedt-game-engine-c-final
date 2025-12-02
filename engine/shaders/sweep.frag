#version 330 core

out vec4 FragColor;

in float AngleNormalized;  // 0 to 1 across the arc
in float DistFromCenter;   // 0 at center, 1 at edge

// Sweep animation parameters
uniform vec3 color;           // Base color (e.g., orange-red)
uniform float alpha;          // Overall opacity
uniform float sweepProgress;  // 0.0 to 1.0 - how far the sweep has progressed
uniform float trailLength;    // How much trail to show behind the sweep edge (0.0 to 1.0)

void main() {
    // DEBUG: Just render solid color to test if mesh is visible
    FragColor = vec4(color, 1.0);
}
