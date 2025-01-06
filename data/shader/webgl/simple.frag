in vec4 fPosition;


in  vec3 FragPos;
in  vec3 Normal;
in vec3 Color;

uniform mat4 projection;
uniform mat4 view;

// Just to keep our uniforms consistent between shaders
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 lineColor;

// output
out vec4 FragColor;

void main(void)
{    vec3 _ = lightPos; // To keep our uniforms consistent between shaders
    _ = viewPos;

    FragColor = lineColor;
}