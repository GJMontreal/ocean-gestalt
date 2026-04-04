layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aTangent;  // W = handedness

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
    // Proper TBN from GLTF tangents — no cross-with-up approximation.
    vec3 N = normalize(mat3(model) * aNormal);
    vec3 T = normalize(mat3(model) * aTangent.xyz);
    vec3 B = cross(N, T) * aTangent.w;  // handedness from W component

    vs_out.TBN      = mat3(T, B, N);
    vs_out.FragPos  = vec3(model * vec4(aPosition, 1.0));
    vs_out.TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
    gl_ClipDistance[0] = vs_out.FragPos.y;
}
