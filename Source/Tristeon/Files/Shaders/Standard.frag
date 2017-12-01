#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(set = 1, binding = 1) uniform sampler2D diffuse;

layout (location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inViewPos;
layout(location = 0) out vec4 frag_color;

//layout(push_constant) uniform PushConsts {
//  layout(offset = 12) float roughness;
//  layout(offset = 16) float metallic;
//} material;

const float PI = 3.14159265359;

vec3 materialColor()
{
    return vec3(1.0f, 0.765557f, 0.336057f);
}

//The D term is the normal distribution  function (NDF) component and
//it is responsible for determining the initial proportion of light that is on the surface.
//The GGX/Throwbridge-Reitz (Karis, 2013) is defined as follows:
// denom = (n ⋅ h)²(a² - 1) + 1)
//     a² /
// PI*(denom)²
float D_GGX(float dotNH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = dotNH * dotNH * (a2 - 1.0) + 1.0;
    return (a2) / (PI * denom * denom);
}

//The F term represents the Fresnel approximation.
//The Fresnel reflection factor represents how specular light is reflected on a surface
//with respect to the angle of intersection between the view and the light on the surface.
//It assumes that smooth surfaces will reach 100% specular reflection at grazing angles.
//Rough surfaces won't reach full reflection
//The Schlick-Fresnel method with spherical-gaussian approximation is defined as follows:
vec3 F_Schlick(float cosTheta, float metallic)
{
    //F0 = specular reflectance at normal incidence
    vec3 F0 = mix(vec3(0.04, 0.04, 0.04), materialColor(), metallic);

    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);

    // x = (v ⋅ h)
    //F(x) = F0 + (1 - F0) 2⁻⁵⋅⁵⁵⁴⁷³ˣ⁻⁶⋅⁹⁸³¹⁶ˣ
    //return F0 + (1.0 - F0) * pow(2, -5.55473*dotVH - 6.98316*dotVH);
}

//The G term is the geometric attenuation / self-shadowing factor also known as the visibility function.
//It represents of the probability that the microfacet, taking into account the microfacett normal,
//is visible from the direction of both the camera and the light. It has significant effect on the
//surface albedo and therefore surface appearanced (Burley, 2012).
//The most common approach is the Schlick-Smith model, which takes the direction of the light, camera and
//surface roughness into account:
//k = (material roughness + 1)² / 8;
//G1(v) = (n ⋅ v) / ((n ⋅ v)(1 - k) + k)
//G(l, v, h) = G1(l)*G1(v)
float G_Schlick_Smith(float dotNL, float dotNV, float roughness)
{
  float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

vec3 BRDF(vec3 l, vec3 v, vec3 n)
{
    float roughness = 0.1;
    float metallic = 1;

    vec3 h = normalize(v + l);
    float dotNV = clamp(dot(n, v), 0.0, 1.0);
    float dotNL = clamp(dot(n, l), 0.0, 1.0);
    float dotNH = clamp(dot(n, h), 0.0, 1.0);

    vec3 lightColor = vec3(1, 1, 1);

    vec3 color = vec3(0.0);

    if (dotNL > 0.0)
    {
        //D = normal distribution (distribution of the microfacets)
        float D = D_GGX(dotNH, roughness);

        //G = Geometric shadowing term (Microfacets shadowing)
        float G = G_Schlick_Smith(dotNL, dotNV, roughness);

        //F = Fresnel factor (reflectance depending on angle of incidence)
        vec3 F = F_Schlick(dotNV, metallic);

        vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

        color += spec * dotNL * lightColor;
    }

    return color;
}

void main()
{
    vec3 lights[4] = {
        vec3(-15, -15*0.5, -15),
        vec3(-15, -15*0.5, 15),
        vec3(15, -15*0.5, 15),
        vec3(15, -15*0.5, -15)
    };

    //Normal and view
    vec3 N = normalize(inNormal);
    vec3 V = normalize(inViewPos - inWorldPos);

    vec3 light = vec3(0.0);
    for (int i = 0; i < lights.length(); i++)
    {
        vec3 L = normalize(lights[i] - inWorldPos);
        light += BRDF(L, V, N);
    }

      //Ambient
	   vec3 color = materialColor() * 0.02;
     color += light;

     //Gamma correct
	   color = pow(color, vec3(0.4545));

    frag_color = texture(diffuse, inUV) * vec4(color, 1.0);
}
