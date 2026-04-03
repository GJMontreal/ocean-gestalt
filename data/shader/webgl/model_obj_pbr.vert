in vec3 aPosition;
in vec2 aTexCoord;
in vec3 aTangent; // stores the OBJ mesh normal

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN;

void main() {
    vec3 localNormal = normalize(aTangent);

    // Build a stable tangent frame from the mesh normal
    vec3 up     = abs(localNormal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 localT = normalize(cross(up, localNormal));
    vec3 localB = cross(localNormal, localT);

    vec3 N = normalize(mat3(model) * localNormal);
    vec3 T = normalize(mat3(model) * localT);
    vec3 B = cross(N, T);

    TBN      = mat3(T, B, N);
    FragPos  = vec3(model * vec4(aPosition, 1.0));
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
