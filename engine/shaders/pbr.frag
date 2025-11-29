#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 VertexColor;
in vec4 FragPosLightSpace[4];

// Material properties
uniform vec3 albedo;
uniform vec3 specularColor;
uniform float metallic;
uniform float roughness;

// Texture flags
uniform bool useAlbedoMap;
uniform bool useSpecularMap;
uniform bool useMetallicMap;
uniform bool useRoughnessMap;
uniform bool useNormalMap;

// Textures (units 0-4)
uniform sampler2D albedoMap;
uniform sampler2D specularMap;
uniform sampler2D metallicRoughnessMap;  // R=metallic, G=roughness
uniform sampler2D normalMap;

// View position
uniform vec3 viewPos;

// Ambient light
uniform vec3 ambientColor;

// ============================================================================
// Light structure (matches C++ Light struct)
// ============================================================================
const int MAX_LIGHTS = 8;

struct LightData {
    int type;           // 0=Directional, 1=Point, 2=Spot
    vec3 position;      // Position or direction-to-light for directional
    vec3 direction;     // Direction for spot lights
    vec3 color;
    float intensity;
    float constant;     // Attenuation
    float linear;
    float quadratic;
    float innerCutoff;  // cos(angle) for spot lights
    float outerCutoff;
    int shadowMapIndex; // -1 = no shadow, 0-3 = 2D shadow map, 0-1 = cubemap for point
};

uniform LightData lights[MAX_LIGHTS];
uniform int numLights;

// ============================================================================
// Shadow maps (texture units 5-8 for 2D, 9-10 for cubemaps)
// ============================================================================
uniform sampler2D shadowMaps[4];
uniform int numShadowMaps;

uniform samplerCube shadowCubeMaps[2];
uniform float pointLightFarPlanes[2];
uniform int numShadowCubeMaps;

const float PI = 3.14159265359;

// ============================================================================
// Shadow calculation (simple PCF for crisp shadows)
// ============================================================================

float sampleShadowMap(int shadowIndex, vec2 coords) {
    if (shadowIndex == 0) return texture(shadowMaps[0], coords).r;
    else if (shadowIndex == 1) return texture(shadowMaps[1], coords).r;
    else if (shadowIndex == 2) return texture(shadowMaps[2], coords).r;
    else return texture(shadowMaps[3], coords).r;
}

float calculateShadow(int shadowIndex, vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    if (shadowIndex < 0 || shadowIndex >= numShadowMaps) return 0.0;

    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;  // Transform to [0,1]

    // Outside shadow map bounds - no shadow
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    vec2 uv = projCoords.xy;
    vec2 texelSize = 1.0 / textureSize(shadowMaps[0], 0);

    // Bias to prevent shadow acne (slope-based)
    float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.0005);

    // Simple 3x3 PCF for slightly smoothed but crisp shadows
    float shadow = 0.0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float depth = sampleShadowMap(shadowIndex, uv + vec2(x, y) * texelSize);
            shadow += currentDepth - bias > depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    float fadeStart = 0.5;
    float edgeFade = 0.8;
    if (projCoords.x < 1.0 - fadeStart) edgeFade = min(edgeFade, projCoords.x / (1.0 - fadeStart));
    if (projCoords.x > fadeStart) edgeFade = min(edgeFade, (1.0 - projCoords.x) / (1.0 - fadeStart));
    if (projCoords.y < 1.0 - fadeStart) edgeFade = min(edgeFade, projCoords.y / (1.0 - fadeStart));
    if (projCoords.y > fadeStart) edgeFade = min(edgeFade, (1.0 - projCoords.y) / (1.0 - fadeStart));
    
    return shadow * edgeFade;
}

// ============================================================================
// Shadow calculation for point lights (cubemap)
// ============================================================================
float calculatePointShadow(int cubemapIndex, vec3 fragPos, vec3 lightPos, float farPlane) {
    if (cubemapIndex < 0 || cubemapIndex >= numShadowCubeMaps) return 0.0;

    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight) / farPlane;

    // PCF for cubemap shadows - sample in a small radius
    float shadow = 0.0;
    float bias = 0.005;
    float samples = 4.0;
    float offset = 0.02;

    for (float x = -offset; x < offset; x += offset / (samples * 0.5)) {
        for (float y = -offset; y < offset; y += offset / (samples * 0.5)) {
            for (float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                float closestDepth;
                vec3 sampleDir = fragToLight + vec3(x, y, z);

                if (cubemapIndex == 0) {
                    closestDepth = texture(shadowCubeMaps[0], sampleDir).r;
                } else {
                    closestDepth = texture(shadowCubeMaps[1], sampleDir).r;
                }

                shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
            }
        }
    }
    shadow /= (samples * samples * samples);

    return shadow;
}

// ============================================================================
// PBR Lighting calculation (Blinn-Phong approximation)
// ============================================================================
vec3 calculateLighting(vec3 N, vec3 V, vec3 L, float attenuation, vec3 lightColor,
                       vec3 albedoColor, float metallic, float roughness, vec3 specular) {
    vec3 H = normalize(V + L);

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor * albedoColor * attenuation;

    // Specular (Blinn-Phong)
    float shininess = mix(32.0, 256.0, 1.0 - roughness);
    float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specularContribution = spec * lightColor * specular * attenuation;

    // Mix between diffuse and specular based on metallic
    return mix(diffuse, specularContribution, metallic);
}

// ============================================================================
// Main
// ============================================================================
void main() {
    // Sample textures
    vec3 finalAlbedo = albedo;
    if (useAlbedoMap) {
        finalAlbedo *= texture(albedoMap, TexCoords).rgb;
    }
    finalAlbedo *= VertexColor.rgb;  // Multiply by vertex color

    vec3 finalSpecular = specularColor;
    if (useSpecularMap) {
        finalSpecular *= texture(specularMap, TexCoords).rgb;
    }

    float finalMetallic = metallic;
    float finalRoughness = roughness;
    if (useMetallicMap || useRoughnessMap) {
        vec3 mrSample = texture(metallicRoughnessMap, TexCoords).rgb;
        if (useMetallicMap) finalMetallic *= mrSample.r;
        if (useRoughnessMap) finalRoughness *= mrSample.g;
    }

    // Normal (TODO: implement tangent-space normal mapping)
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    // Ambient component
    vec3 result = ambientColor * finalAlbedo;

    // Process each light
    for (int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        vec3 L;
        float attenuation = 1.0;
        float shadow = 0.0;

        if (lights[i].type == 0) {
            // Directional light - position stores direction TO light
            L = normalize(lights[i].position);

            // Shadow from 2D shadow map
            if (lights[i].shadowMapIndex >= 0 && lights[i].shadowMapIndex < 4) {
                shadow = calculateShadow(lights[i].shadowMapIndex,
                                         FragPosLightSpace[lights[i].shadowMapIndex], N, L);
            }
        }
        else if (lights[i].type == 1) {
            // Point light
            vec3 lightVec = lights[i].position - FragPos;
            float distance = length(lightVec);
            L = normalize(lightVec);

            // Distance attenuation
            attenuation = 1.0 / (lights[i].constant +
                                lights[i].linear * distance +
                                lights[i].quadratic * distance * distance);

            // Shadow from cubemap
            if (lights[i].shadowMapIndex >= 0 && lights[i].shadowMapIndex < 2) {
                shadow = calculatePointShadow(lights[i].shadowMapIndex, FragPos,
                                              lights[i].position,
                                              pointLightFarPlanes[lights[i].shadowMapIndex]);
            }
        }
        else if (lights[i].type == 2) {
            // Spot light
            vec3 lightVec = lights[i].position - FragPos;
            float distance = length(lightVec);
            L = normalize(lightVec);

            // Spot cone attenuation
            float theta = dot(L, normalize(-lights[i].direction));
            float epsilon = lights[i].innerCutoff - lights[i].outerCutoff;
            float spotIntensity = clamp((theta - lights[i].outerCutoff) / epsilon, 0.0, 1.0);

            // Distance attenuation combined with spot intensity
            attenuation = spotIntensity / (lights[i].constant +
                                           lights[i].linear * distance +
                                           lights[i].quadratic * distance * distance);

            // Shadow from 2D shadow map
            if (lights[i].shadowMapIndex >= 0 && lights[i].shadowMapIndex < 4) {
                shadow = calculateShadow(lights[i].shadowMapIndex,
                                         FragPosLightSpace[lights[i].shadowMapIndex], N, L);
            }
        }

        // Calculate lighting contribution
        vec3 lightContrib = calculateLighting(N, V, L, attenuation * lights[i].intensity,
                                               lights[i].color, finalAlbedo,
                                               finalMetallic, finalRoughness, finalSpecular);

        // Apply shadow (1.0 - shadow means fully lit)
        result += lightContrib * (1.0 - shadow);
    }

    FragColor = vec4(result, 1.0);
}
