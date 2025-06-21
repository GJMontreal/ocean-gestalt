layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform mat4 model;

uniform float time;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
    // vec3 Tangent;
    // vec3 Bitangent;
    // vec2 FragUV;
} vs_out;

void main() {

  vs_out.FragPos = vec3(model * vec4(position, 1.0));
  vs_out.Normal = mat3(transpose(inverse(model))) * normal;
  vs_out.Color = vec3(color.xyz);
  // vs_out.Bitangent = vs_out.FragPos;
  // vs_out.Tangent = vs_out.FragPos;
  // vs_out.FragUV = (vs_out.FragPos).xz;
  gl_Position = projection * view * model * vec4(position, 1.0);
}
