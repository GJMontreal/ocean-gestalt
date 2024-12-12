#version 330 core

in vec3 position;
//in vec3 normal;
//in vec3 color;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;

out vec3 aColor;
void main()
{
    //mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    //vs_out.normal = vec3(vec4(normalMatrix * normal, 0.0));
    //aColor = color * sin(mod(time,6));
    gl_Position = model * vec4(position, 1.0) ;
}
