#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(set = 1, binding = 1) uniform sampler2D _screenTexture;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 frag_color;

void main()
{
    frag_color = texture(_screenTexture, fragTexCoord);
}
