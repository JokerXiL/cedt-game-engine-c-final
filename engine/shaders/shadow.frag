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
    } else {
        // Directional/spot light: use the default depth from rasterizer
        // Must explicitly write gl_FragDepth in all branches when used in any branch
        gl_FragDepth = gl_FragCoord.z;
    }
}
