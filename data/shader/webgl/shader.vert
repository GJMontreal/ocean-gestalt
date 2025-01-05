in vec3 position;
in vec3 normal;
in vec4 color;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(void)
{
    FragPos = position;
    Normal = normal;
    Color = vec3(color);
    
    gl_Position = projection * view * model * vec4(position,1.0);
}
