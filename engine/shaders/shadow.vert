#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 4) in vec4 aWeights;
layout (location = 5) in ivec4 aBoneIndices;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

// Skeletal animation support
const int MAX_BONES = 100;
uniform mat4 boneTransforms[MAX_BONES];
uniform bool useSkinning;

// Point light shadow support
uniform bool isPointLight;
out vec3 FragPos;

void main() {
    vec4 finalPosition;

    if (useSkinning) {
        vec4 totalPosition = vec4(0.0);
        for (int i = 0; i < 4; i++) {
            int boneIndex = clamp(aBoneIndices[i], 0, MAX_BONES - 1);
            float weight = aWeights[i];
            totalPosition += boneTransforms[boneIndex] * vec4(aPos, 1.0) * weight;
        }
        finalPosition = totalPosition;
    } else {
        finalPosition = vec4(aPos, 1.0);
    }

    vec4 worldPos = model * finalPosition;

    // Pass world position for point light distance calculation
    if (isPointLight) {
        FragPos = worldPos.xyz;
    }

    gl_Position = lightSpaceMatrix * worldPos;
}
