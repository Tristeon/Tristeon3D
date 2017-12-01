#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 0) uniform UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

out gl_PerVertex {
  vec4 gl_Position;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout (location = 0) out vec3 inWorldPos;
layout(location = 1) out vec3 inNormal;
layout(location = 2) out vec2 inUV;
layout(location = 3) out vec3 inViewPos;

void main()
{
  inWorldPos =  vec3(ubo.model * vec4(pos, 1));
  inNormal = vec3(ubo.model * vec4(normal, 1));
  inUV = texCoord;
  inViewPos = vec3(inverse(ubo.view)[3]);

  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos, 1);
}
