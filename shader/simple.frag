in vec4 fPosition;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} fs_in;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

// output
out vec4 FragColor;

void main(void)
{       
    FragColor = vec4(.5,.5,.5,1.0);
}
