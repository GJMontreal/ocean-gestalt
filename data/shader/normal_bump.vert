layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aTangent;

layout(std140) uniform Matrices
{
  uniform mat4 projection;
  uniform mat4 view;
};

uniform mat4 model;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    mat3 TBN;
} vs_out;

void main() {
    vec3 normal = normalize(aPosition);  // unit sphere assumption
    vec3 tangent = normalize(mat3(model) * aTangent);
    vec3 N = normalize(mat3(model) * normal);
    vec3 B = cross(N, tangent);  // bitangent

    vs_out.TBN = mat3(tangent, B, N);
    vs_out.FragPos = vec3(model * vec4(aPosition, 1.0));
    vs_out.TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}
