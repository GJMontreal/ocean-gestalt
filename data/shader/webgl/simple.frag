in vec4 fPosition;

in vec3 oFragPos;
in vec3 oNormal;
in vec3 oColor;

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

    FragColor = vec4(oColor,1.0);
}
