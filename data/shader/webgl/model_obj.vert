in vec3 aPosition;
in vec2 aTexCoord;
in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main() {
    FragPos  = vec3(model * vec4(aPosition, 1.0));
    Normal   = normalize(mat3(transpose(inverse(model))) * aNormal);
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
