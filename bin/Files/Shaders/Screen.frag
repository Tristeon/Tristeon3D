#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D color;
layout(binding = 1) uniform sampler2D depth;
layout(binding = 2) uniform sampler2D normal;

layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 fragColor;

void main()
{
    vec4 colorVal = texture(color, texCoord);
    float depthVal = texture(depth, texCoord).r;
    vec4 normalVal = texture(normal, texCoord);

    vec3 albedo = colorVal.rgb;
    vec3 normal = normalVal.rgb;
    float metalic = colorVal.a;
    float roughness = normalVal.a;

    fragColor = vec4(albedo, 1);
}
