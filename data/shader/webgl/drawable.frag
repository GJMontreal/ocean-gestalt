
in vec3 oFragPos;
in vec3 oNormal;
in vec3 oColor;

uniform mat4 projection;
uniform mat4 view;

// Just to keep our uniforms consistent between shaders
uniform vec3 lightPos;
uniform vec3 viewPos;
  
uniform float ambientStrength;

// output
out vec4 FragColor;

void main(void)
{    
   vec3 _ = viewPos;
   
    // Normalize interpolated normal
    vec3 normal = normalize(oNormal);
    
    // Compute lighting direction
    vec3 lightDir = normalize(lightPos - oFragPos);

    // Lambertian diffuse term
    float diff = max(dot(normal, lightDir), 0.0);

    // Final color is base color modulated by diffuse term
    vec3 color = oColor * diff;

    color = mix(color, oColor, ambientStrength);
    FragColor = vec4(color, 1.0);
}
