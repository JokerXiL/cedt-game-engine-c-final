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
out vec4 FragPosLightSpace[4];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Shadow mapping
uniform mat4 lightSpaceMatrices[4];
uniform int numShadowMaps;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    VertexColor = aColor;

    // Calculate positions in light space for shadow mapping
    for (int i = 0; i < numShadowMaps && i < 4; i++) {
        FragPosLightSpace[i] = lightSpaceMatrices[i] * worldPos;
    }

    gl_Position = projection * view * worldPos;
}
