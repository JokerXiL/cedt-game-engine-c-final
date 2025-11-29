#version 330 core

in vec3 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
    // Calculate linear distance from light to fragment
    float lightDistance = length(FragPos - lightPos);

    // Normalize to [0, 1] range using far plane
    gl_FragDepth = lightDistance / farPlane;
}
