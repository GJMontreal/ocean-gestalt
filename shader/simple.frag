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

// Just to keep our uniforms consistent between shaders
uniform vec3 lightPos;
uniform vec3 viewPos;

// output
out vec4 FragColor;

void main(void)
{    vec3 _ = lightPos; // To keep our uniforms consistent between shaders
    _ = viewPos;

    FragColor = vec4(.5,.5,.5,1.0);
}
