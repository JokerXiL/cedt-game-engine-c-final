#version 330 core

in vec3 FragPos;

uniform bool isPointLight;
uniform vec3 lightPos;
uniform float farPlane;

void main() {
    if (isPointLight) {
        // Point light: calculate linear distance from light to fragment
        float lightDistance = length(FragPos - lightPos);
        // Normalize to [0, 1] range using far plane
        gl_FragDepth = lightDistance / farPlane;
    }
    // Directional/spot light: depth is written automatically by rasterizer
    // gl_FragDepth = gl_FragCoord.z; // Implicit when not set
}
