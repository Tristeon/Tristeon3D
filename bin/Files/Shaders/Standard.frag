#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;
layout(set = 1, binding = 2) uniform sampler2D metallicMap;
layout(set = 1, binding = 3) uniform sampler2D roughnessMap;
layout(set = 1, binding = 4) uniform sampler2D aoMap;

struct Material
{
    vec4 color;
    float roughness;
    float metallic;
} material;

//From vertex shader
layout (location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inViewPos;
layout(location = 4) in float inDepth;

//Output
layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 frag_normal;

vec3 _pow(vec3 vec, float p)
{
  return vec3(pow(vec.x, p), pow(vec.y, p), pow(vec.z, p));
}
void main()
{
    material.color = vec4(1, 1, 1, 1);
    material.roughness = 1;
    material.metallic = 1;

    //Textures
    vec3 albedo = _pow(texture(albedoMap, inUV).rgb, 2.2) * vec3(material.color); //Convert from srgb to linear
    float metallic = texture(metallicMap, inUV).r * material.metallic;
    float roughness = texture(roughnessMap, inUV).r * material.roughness;
    float ao = texture(aoMap, inUV).r;

    frag_color = vec4(albedo, metallic);
    frag_normal = vec4(inNormal, roughness);
}