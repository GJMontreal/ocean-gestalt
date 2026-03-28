layout(location = 0) in vec3 aPosition; 
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec4 aColor;

out vec4 color;
out vec2 texCoord;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(aPosition, 1.0);
    color = aColor;
    texCoord = aTexCoord;
}
