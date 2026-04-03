layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aTangent; // stores the OBJ mesh normal

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
    vec3 localNormal = normalize(aTangent);

    // Build a stable tangent frame from the mesh normal
    vec3 up     = abs(localNormal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 localT = normalize(cross(up, localNormal));
    vec3 localB = cross(localNormal, localT);

    vec3 N = normalize(mat3(model) * localNormal);
    vec3 T = normalize(mat3(model) * localT);
    vec3 B = cross(N, T);

    vs_out.TBN     = mat3(T, B, N);
    vs_out.FragPos = vec3(model * vec4(aPosition, 1.0));
    vs_out.TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
    gl_ClipDistance[0] = vs_out.FragPos.y;
}
