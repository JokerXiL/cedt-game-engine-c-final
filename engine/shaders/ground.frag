#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 VertexColor;
in vec4 FragPosLightSpace[4];

out vec4 FragColor;

// Material properties
uniform vec3 albedo;
uniform vec3 specular;
uniform float metallic;
uniform float roughness;

// Pixelation settings
uniform float pixelSize;       // Size of each "pixel" in world units
uniform float colorVariation;  // How much random variation (0.0 - 1.0)
uniform float time;            // For optional animation

// Lighting
struct Light {
    int type;              // 0: directional, 1: point, 2: spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
    float cutoff;
    float outerCutoff;
    int shadowMapIndex;    // -1 if no shadow, 0-3 for 2D maps, 4-5 for cubemaps
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 ambient;

// Camera
uniform vec3 viewPos;

// Shadow maps (texture units 5-8 for 2D, 9-10 for cubemaps)
uniform sampler2D shadowMaps[4];
uniform samplerCube shadowCubeMaps[2];
uniform float pointLightFarPlanes[2];
uniform int numShadowMaps;
uniform int numShadowCubeMaps;

// Poisson disk sampling for softer shadows
const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845),
    vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554),
    vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507),
    vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367),
    vec2(0.14383161, -0.14100790)
);

// Hash function for pseudo-random numbers based on world position
float hash(vec2 p) {
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

// 2D noise for color variation
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Calculate pixelated position
vec2 pixelate(vec2 worldXZ) {
    return floor(worldXZ / pixelSize) * pixelSize;
}

// Get random green color for a pixel cell
vec3 getPixelatedGreen(vec2 worldXZ) {
    vec2 cellPos = pixelate(worldXZ);

    // Generate random value for this cell
    float rand = hash(cellPos * 0.1);

    // Add some noise layers for variation
    float n1 = noise(cellPos * 0.05);
    float n2 = noise(cellPos * 0.02 + 17.3);

    // Combine randoms
    float variation = mix(rand, n1 * 0.5 + n2 * 0.5, 0.5);

    // Create green color with variation
    // Base green: (0.2-0.5, 0.4-0.8, 0.1-0.3)
    vec3 baseGreen = albedo;
    vec3 darkGreen = baseGreen * 0.6;
    vec3 lightGreen = baseGreen * 1.4;

    // Mix between dark and light green based on random value
    vec3 pixelColor = mix(darkGreen, lightGreen, variation * colorVariation + (1.0 - colorVariation) * 0.5);

    return pixelColor;
}

// Shadow calculation for 2D shadow maps (directional/spot lights)
float calculateShadow2D(int mapIndex, vec4 fragPosLightSpace) {
    if (mapIndex < 0 || mapIndex >= numShadowMaps) return 0.0;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float bias = 0.002;

    // PCF with Poisson disk sampling
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps[0], 0);

    for (int i = 0; i < 16; i++) {
        vec2 offset = poissonDisk[i] * texelSize * 2.0;
        float pcfDepth;
        if (mapIndex == 0) pcfDepth = texture(shadowMaps[0], projCoords.xy + offset).r;
        else if (mapIndex == 1) pcfDepth = texture(shadowMaps[1], projCoords.xy + offset).r;
        else if (mapIndex == 2) pcfDepth = texture(shadowMaps[2], projCoords.xy + offset).r;
        else pcfDepth = texture(shadowMaps[3], projCoords.xy + offset).r;

        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
    shadow /= 16.0;

    return shadow;
}

// Shadow calculation for cubemap shadows (point lights)
float calculateShadowCube(int cubeIndex, vec3 fragToLight) {
    if (cubeIndex < 0 || cubeIndex >= numShadowCubeMaps) return 0.0;

    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float farPlane = pointLightFarPlanes[cubeIndex];

    float shadow = 0.0;
    float samples = 4.0;
    float offset = 0.1;

    for (float x = -offset; x < offset; x += offset / (samples * 0.5)) {
        for (float y = -offset; y < offset; y += offset / (samples * 0.5)) {
            for (float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                float closestDepth;
                if (cubeIndex == 0) closestDepth = texture(shadowCubeMaps[0], fragToLight + vec3(x, y, z)).r;
                else closestDepth = texture(shadowCubeMaps[1], fragToLight + vec3(x, y, z)).r;

                closestDepth *= farPlane;
                if (currentDepth - bias > closestDepth) shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);

    return shadow;
}

void main() {
    // Get pixelated green color based on world position
    vec3 pixelatedAlbedo = getPixelatedGreen(FragPos.xz);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Ambient
    vec3 result = ambient * pixelatedAlbedo;

    // Process each light
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        Light light = lights[i];
        vec3 lightDir;
        float attenuation = 1.0;
        float shadow = 0.0;

        if (light.type == 0) {
            // Directional light
            lightDir = normalize(-light.direction);

            // Shadow
            if (light.shadowMapIndex >= 0 && light.shadowMapIndex < 4) {
                shadow = calculateShadow2D(light.shadowMapIndex, FragPosLightSpace[light.shadowMapIndex]);
            }
        } else if (light.type == 1) {
            // Point light
            vec3 fragToLight = light.position - FragPos;
            lightDir = normalize(fragToLight);
            float distance = length(fragToLight);
            attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

            // Shadow from cubemap (indices 0-1 for point lights)
            if (light.shadowMapIndex >= 0 && light.shadowMapIndex < 2) {
                shadow = calculateShadowCube(light.shadowMapIndex, -fragToLight);
            }
        } else {
            // Spot light
            vec3 fragToLight = light.position - FragPos;
            lightDir = normalize(fragToLight);
            float distance = length(fragToLight);
            attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

            float theta = dot(lightDir, normalize(-light.direction));
            float epsilon = light.cutoff - light.outerCutoff;
            float spotIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
            attenuation *= spotIntensity;

            // Shadow
            if (light.shadowMapIndex >= 0 && light.shadowMapIndex < 4) {
                shadow = calculateShadow2D(light.shadowMapIndex, FragPosLightSpace[light.shadowMapIndex]);
            }
        }

        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * light.color * light.intensity * pixelatedAlbedo;

        // Specular (Blinn-Phong)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), max((1.0 - roughness) * 128.0, 1.0));
        vec3 specularColor = mix(specular, pixelatedAlbedo, metallic);
        vec3 specularResult = spec * light.color * light.intensity * specularColor * (1.0 - roughness);

        // Apply shadow and attenuation
        result += (1.0 - shadow) * attenuation * (diffuse + specularResult);
    }

    FragColor = vec4(result, 1.0);
}
