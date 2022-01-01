#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_Texture_UV;
layout (location = 3) in float a_TextureIndex;

layout (location = 0) out vec4 v_Color;
layout (location = 1) out vec2 v_Texture_UV;
layout (location = 2) out float v_TextureIndex;

layout (std140, binding = 0) uniform Camera
{
    mat4 ViewProjectionMatrix;
} u_Camera;

void main()
{
    v_Color = a_Color;
    v_Texture_UV = a_Texture_UV;
    v_TextureIndex = a_TextureIndex;

    gl_Position = u_Camera.ViewProjectionMatrix * vec4(a_Position, 1.0);
}