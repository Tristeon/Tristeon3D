#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 texcoord;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 normal;

void main()
{
    color = vec4(0, 1, 1, 1.0);
    normal = vec4(0, 0, -1, 0);
}