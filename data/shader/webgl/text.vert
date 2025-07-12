in vec3 aPosition; 
in vec4 aColor;
in vec2 aTexCoord;

out vec4 color;
out vec2 texCoord;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(aPosition, 1.0);
    color = aColor;
    texCoord = aTexCoord;
}
