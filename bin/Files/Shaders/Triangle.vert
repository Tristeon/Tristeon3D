#version 450

layout(binding = 0) uniform Transformation
{
    mat4x4 model;
    mat4x4 view;
    mat4x4 projection;
} transform;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec2 outTexCoord;

void main()
{
    gl_Position = transform.projection * transform.view * transform.model * vec4(position, 1);
    outTexCoord = texcoord;
}