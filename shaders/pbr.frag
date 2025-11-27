#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 VertexColor;

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

// Textures
uniform sampler2D albedoMap;
uniform sampler2D specularMap;
uniform sampler2D metallicRoughnessMap;  // R=metallic, G=roughness
uniform sampler2D normalMap;

// Lighting
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

const float PI = 3.14159265359;

// Simplified PBR lighting (Blinn-Phong approximation)
vec3 calculatePBR(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness, vec3 specular) {
    vec3 H = normalize(V + L);

    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor * albedo;

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor * albedo;

    // Specular (Blinn-Phong)
    float shininess = mix(32.0, 256.0, 1.0 - roughness);
    float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specularContribution = spec * lightColor * specular;

    // Mix between diffuse and specular based on metallic
    vec3 result = mix(diffuse, specularContribution, metallic) + ambient;

    return result;
}

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

    // Normal mapping (simplified - just use vertex normal for now)
    vec3 N = normalize(Normal);
    // TODO: Implement proper tangent-space normal mapping when needed

    vec3 V = normalize(viewPos - FragPos);
    vec3 L = normalize(lightPos - FragPos);

    // Calculate PBR lighting
    vec3 color = calculatePBR(N, V, L, finalAlbedo, finalMetallic, finalRoughness, finalSpecular);

    FragColor = vec4(color, 1.0);
}
