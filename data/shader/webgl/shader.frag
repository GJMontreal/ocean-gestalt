in vec4 fPosition;

in  vec3 FragPos;
in  vec3 Normal;
in  vec3 Color;

uniform mat4 projection;
uniform mat4 view;

// output
out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main(void)
{       
    vec3 color = Color;
    // ambient
    vec3 ambient = 0.001 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    
    vec3 specular = vec3(0.5) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
