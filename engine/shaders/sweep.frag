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
    // Sweep effect that starts from center (DistFromCenter = 0) and expands outward
    // The sweep edge is at sweepProgress distance from center
    float sweepEdge = sweepProgress;
    
    // Calculate how far this fragment is behind the sweep edge
    float distanceBehindEdge = sweepEdge - DistFromCenter;
    
    // Discard fragments that are ahead of the sweep
    if (distanceBehindEdge < 0.0) {
        discard;
    }
    
    // Calculate opacity based on distance behind the edge
    // Fragments right at the edge are brightest, older fragments fade out
    float fadeStart = trailLength;
    float edgeFade = smoothstep(fadeStart, 0.0, distanceBehindEdge);
    
    // Add extra brightness at the leading edge
    float edgeGlow = smoothstep(0.2, 0.0, distanceBehindEdge) * 0.5;
    
    // Combine base alpha with fade and edge glow
    float finalAlpha = alpha * edgeFade * (1.0 + edgeGlow);
    
    // Output color with calculated alpha
    FragColor = vec4(color * (1.0 + edgeGlow), finalAlpha);
}
