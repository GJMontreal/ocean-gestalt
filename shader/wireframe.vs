

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;

layout(std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

/*out VS_OUT {
    vec3 normal;
} vs_out;
*/

uniform mat4 model;

out vec3 aColor;
void main()
{
    //mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    //vs_out.normal = vec3(vec4(normalMatrix * normal, 0.0));
    aColor = vec3(position);
    gl_Position = model * vec4(position, 1.0) ;
}
