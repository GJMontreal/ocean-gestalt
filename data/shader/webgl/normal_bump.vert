in vec3 aPosition;
in vec2 aTexCoord;
in vec3 aTangent;
in vec4 aColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN;

void main() {
    vec3 normal = normalize(aPosition);  // unit sphere assumption
    vec3 tangent = normalize(mat3(model) * aTangent);
    vec3 N = normalize(mat3(model) * normal);
    vec3 B = cross(N, tangent);  // bitangent

    TBN = mat3(tangent, B, N);
    FragPos = vec3(model * vec4(aPosition, 1.0));
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
