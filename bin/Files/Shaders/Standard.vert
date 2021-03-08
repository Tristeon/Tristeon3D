#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 0) uniform Transformation
{
    mat4x4 model;
    mat4x4 view;
    mat4x4 proj;
} transform;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout (location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec3 outViewPos;
layout(location = 4) out float outDepth;

void main()
{
  outWorldPos =  vec3(transform.model * vec4(pos, 1));
  outNormal = mat3(transpose(inverse(transform.model))) * normal;
  outUV = texCoord;
  outViewPos = vec3(inverse(transform.view)[3]);
  outDepth = (transform.proj * transform.view * transform.model * vec4(pos, 1))[3] / 100; //TODO: Replace 100 with actual camera stat

  gl_Position = transform.proj * transform.view * transform.model * vec4(pos, 1);
}
