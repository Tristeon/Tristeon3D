#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(set = 0, binding = 1) uniform samplerCube skybox;

layout(location = 0) in vec3 pos;
layout(location = 0) out vec4 frag_color;

void main()
{
    frag_color = vec4(texture(skybox, pos).rgb, 1);
}
