layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aTangent;  // W = handedness

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 vTangent;
out vec3 vBitangent;
out vec3 vNormal;

void main() {
    // Proper TBN from GLTF tangents — no cross-with-up approximation.
    vec3 N = normalize(mat3(model) * aNormal);
    vec3 T = normalize(mat3(model) * aTangent.xyz);
    vec3 B = cross(N, T) * aTangent.w;

    vTangent   = T;
    vBitangent = B;
    vNormal    = N;
    FragPos    = vec3(model * vec4(aPosition, 1.0));
    TexCoord   = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
