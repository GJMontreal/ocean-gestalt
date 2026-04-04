in vec3 aPos;

out vec3 TexDir;

uniform mat4 projection;
uniform mat4 view;

void main() {
    mat4 viewNoTranslation = mat4(mat3(view)); // strip position
    vec4 pos = projection * viewNoTranslation * vec4(aPos, 1.0);
    gl_Position = pos;
    TexDir = aPos;
}
