#version 330 core

in vec2 TexCoords;
in vec3 ViewDir;

out vec4 FragColor;

uniform vec3 skyColorTop;      // Color at zenith
uniform vec3 skyColorHorizon;  // Color at horizon
uniform vec3 sunDirection;     // Normalized sun direction (pointing toward sun)
uniform vec3 sunColor;         // Sun disk color
uniform float sunSize;         // Sun angular size (cosine of angle)

void main() {
    vec3 dir = normalize(ViewDir);

    // Calculate vertical gradient factor (0 at horizon, 1 at zenith)
    float horizonFactor = max(dir.y, 0.0);

    // Smooth the gradient with a power curve for more natural look
    float gradientFactor = pow(horizonFactor, 0.4);

    // Base sky color - gradient from horizon to top
    vec3 skyColor = mix(skyColorHorizon, skyColorTop, gradientFactor);

    // Add atmospheric scattering near horizon (subtle orange/pink tint)
    float horizonGlow = pow(1.0 - abs(dir.y), 8.0);
    vec3 horizonTint = vec3(1.0, 0.7, 0.5) * horizonGlow * 0.15;
    skyColor += horizonTint;

    // Sun disk
    float sunDot = dot(dir, sunDirection);
    if (sunDot > sunSize) {
        // Core of sun
        float sunIntensity = smoothstep(sunSize, sunSize + 0.002, sunDot);
        skyColor = mix(skyColor, sunColor, sunIntensity);
    }

    // Sun glow/halo
    float sunGlow = pow(max(sunDot, 0.0), 64.0) * 0.5;
    skyColor += sunColor * sunGlow;

    // Below horizon - dark ground color (should be covered by ground plane)
    if (dir.y < 0.0) {
        float belowFactor = pow(-dir.y, 0.5);
        skyColor = mix(skyColor, vec3(0.1, 0.12, 0.1), belowFactor);
    }

    FragColor = vec4(skyColor, 1.0);
}
