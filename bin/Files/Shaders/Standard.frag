#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;
layout(set = 1, binding = 2) uniform sampler2D metallicMap;
layout(set = 1, binding = 3) uniform sampler2D roughnessMap;
layout(set = 1, binding = 4) uniform sampler2D aoMap;
layout(set = 1, binding = 5) uniform Material
{
    vec4 color;
    float roughness;
    float metallic;
} material;

//Lighting
layout(set = 2, binding = 0) uniform samplerCube skybox;
//TODO: Pass lights properly
vec3 lights[5] = {
  vec3(-30, 5, 10),
  vec3(-15, 5, 10),
  vec3(0, 5, 10),
  vec3(15, 5, 10),
  vec3(30, 5, 10)
};

//From vertex shader
layout (location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inViewPos;

//Output
layout(location = 0) out vec4 frag_color;

//Function prototypes
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

//Constants
const float PI = 3.14159265359;

vec3 _pow(vec3 vec, float p)
{
  return vec3(pow(vec.x, p), pow(vec.y, p), pow(vec.z, p));
}
void main()
{
    //Textures
    vec3 albedo = _pow(texture(albedoMap, inUV).rgb, 2.2) * vec3(material.color); //Convert from srgb to linear
    float metallic = texture(metallicMap, inUV).r * material.metallic;
    float roughness = texture(roughnessMap, inUV).r * material.roughness;
    float ao = texture(aoMap, inUV).r;

    //Normal and viewdirection
    vec3 normal = normalize(inNormal);
    vec3 v = normalize(inViewPos - inWorldPos);
    float NdotV = max(dot(normal, v), 0.0);

    vec3 light = vec3(0.0);
    for (int i = 0; i < 5; i++)
    {
        vec3 l = normalize(lights[i] - inWorldPos);
        vec3 h = normalize(v + l);

        float distance    = length(lights[i] - inWorldPos);
        float range = 1000;
        float attenuation = range / (distance * distance);
        vec3 radiance     = vec3(1) * attenuation;

        float NdotL = max(dot(normal, l), 0.0);

        //F0 represents the base reflectivity of the surface
        //Non metallic surfaces are 0.04. We're assuming that value is generally visually correct.
        //We interpolate torwards the albedo value based on the metallicness for metallic objects
        vec3 f0 = mix(vec3(0.04), albedo, metallic);

        //BRDF components
        vec3  F = fresnelSchlick(max(dot(h, v), 0.0), f0);
        float D = DistributionGGX(normal, h, roughness);
        float G = GeometrySmith(normal, v, l, roughness);

        //fcook−torrance = DFG / 4(ωo ⋅ n)(ωi ⋅ n)
        //In where:
        //w = solid angle. Defined as the size or area of a shape projected onto a unit sphere, but in our case it's infintely small and can be seen as a single point in space.
        //ωo = The outgoing direction to the viewer (camera)
        //ωi = Incoming direction vector (lightdirection)
        //The reflectance equation calculates the sum of reflected radiance of a point p in direction ωo.
        vec3 specular = (D * F * G) / max(4.0 * NdotV * NdotL, 0.001); //Prevent division by 0 using max

        //Ratio of refraction, F directly corresponds to kS so kS is being left out here
        vec3 kD = vec3(1.0) - F;
        kD *= 1.0 - metallic; //The more metallic the less we refract

        light += (kD * albedo / PI + specular) * NdotL * radiance;
    }

    //Ambient light (improvised)
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + light;

    //Convert from HDR to LDR range
    color = color / (color + vec3(1.0));
    //Gamma correct from linear space (PBR required linear)
    color = pow(color, vec3(1.0/2.2));

    //Result
    frag_color = vec4(color, 1);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    //Calculate ratio between specular and diffuse reflection using freschnel schlick, which is an approximation
    //of the actual freschnel equation. https://i.imgur.com/LDSKvhR.png
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    //Normal distribution function statistically approximates the ratio of microfacets aligned to some vector h
    //Trowbridge-reitz GGX: https://i.imgur.com/1m1O4h9.png
    float a      = roughness*roughness;
    float a2 = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    return a2 / (PI * pow((NdotH*NdotH * (a2 - 1.0) + 1.0), 2));
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    //Geometry function statistically approximates the ratio of microfacets that overshadow each other causing light rays to lose their energy in the process
    //Geometry schlick GGX function: https://i.imgur.com/BeSn6sR.png
    float k = pow(roughness+1.0, 2) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    //To effectively approximate the geometry we need to take account of both the view direction (geometry obstruction) and the
    //light direction vector (geometry shadowing). We can take both into account using Smith's method: https://i.imgur.com/2feeXY6.png
    float ggx2  = GeometrySchlickGGX(max(dot(N, V), 0.0), roughness);
    float ggx1  = GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
    return ggx1 * ggx2;
}
