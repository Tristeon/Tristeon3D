#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) out vec4 frag_color;

layout(set = 1, binding = 0) uniform Color
{
  vec4 color;
} color;

void main()
{
    frag_color = color.color;
}
