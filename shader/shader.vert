#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;

layout(std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

out vec4 fPosition;
out vec4 fColor;
out vec4 fLightPosition;
out vec3 fNormal;

void main(void)
{
    fPosition = view * vec4(position,1.0);
    fLightPosition = view * vec4(0.0,0.0,1.0,1.0);

    fNormal = mat3(transpose(inverse(model))) * normal;
    
    fColor = color;
    
    gl_Position = projection * view * model * vec4(position, 1.0) ;
    /*gl_Position.x *= 1000.0f;*/
    /*gl_Position.y = 0.0;*/
}
