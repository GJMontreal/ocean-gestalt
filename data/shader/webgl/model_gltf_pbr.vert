in vec3 aPosition;
in vec2 aTexCoord;
in vec3 aNormal;
in vec4 aTangent;  // W = handedness

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN;

void main() {
    // Proper TBN from GLTF tangents — no cross-with-up approximation.
    vec3 N = normalize(mat3(model) * aNormal);
    vec3 T = normalize(mat3(model) * aTangent.xyz);
    vec3 B = cross(N, T) * aTangent.w;

    TBN      = mat3(T, B, N);
    FragPos  = vec3(model * vec4(aPosition, 1.0));
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
