#version 450
#extension GL_ARB_separate_shader_objects : enable
out gl_PerVertex {
  vec4 gl_Position;
};

//Big triangle covering entire screen
vec2 positions[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2(3.0, -1.0),
    vec2(-1.0, 3.0)
);

vec2 uvs[3] = vec2[](
    vec2(0.0, 0.0),
    vec2(2.0, 0.0),
    vec2(0.0, 2.0)
);

layout(location = 0) out vec2 fragTexCoord;

void main()
{
  fragTexCoord = uvs[gl_VertexIndex];
  gl_Position = vec4(positions[gl_VertexIndex], 0.0f, 1.0f);
}
