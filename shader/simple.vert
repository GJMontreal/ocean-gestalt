layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform mat4 model;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} vs_out;

void main(void)
{
    vs_out.FragPos = position;
    vs_out.Normal = normal;
    vs_out.Color = vec3(color);
    gl_Position = projection * view * model * vec4(position, 1.0) ;
}
