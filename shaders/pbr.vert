#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aColor;
layout (location = 4) in vec4 aWeights;
layout (location = 5) in ivec4 aBoneIndices;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Bone transforms (max 100 bones)
const int MAX_BONES = 100;
uniform mat4 boneTransforms[MAX_BONES];
uniform bool useSkinning;

void main() {
    vec4 finalPosition;
    vec3 finalNormal;

    if (useSkinning) {
        // Apply skeletal animation
        vec4 totalPosition = vec4(0.0);
        vec3 totalNormal = vec3(0.0);

        // Apply up to 4 bone influences per vertex
        for (int i = 0; i < 4; i++) {
            int boneIndex = clamp(aBoneIndices[i], 0, MAX_BONES - 1);
            float weight = aWeights[i];

            totalPosition += boneTransforms[boneIndex] * vec4(aPos, 1.0) * weight;
            totalNormal += mat3(boneTransforms[boneIndex]) * aNormal * weight;
        }

        finalPosition = totalPosition;
        finalNormal = totalNormal;
    } else {
        // Static mesh - use vertex data directly
        finalPosition = vec4(aPos, 1.0);
        finalNormal = aNormal;
    }

    FragPos = vec3(model * finalPosition);
    Normal = mat3(transpose(inverse(model))) * finalNormal;
    TexCoords = aTexCoords;
    VertexColor = aColor;

    gl_Position = projection * view * model * finalPosition;
}
