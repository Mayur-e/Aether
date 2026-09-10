#version 330 core

// -----------------------------------------------------------------------
// Vertex shader — Phong lighting (Gouraud-style data pass-through)
// Inputs at location 0: position, 1: normal, 2: texCoord
// -----------------------------------------------------------------------

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;    // world-space position
out vec3 fragNormal; // world-space normal
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;   // transpose(inverse(model))

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    fragPos       = worldPos.xyz;
    fragNormal    = normalize(normalMatrix * aNormal);
    texCoord      = aTexCoord;

    gl_Position = projection * view * worldPos;
}
