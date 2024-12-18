in vec4 fPosition;

in  vec3 FragPos;
in  vec3 Normal;
in  vec3 Color;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 lightPos;
uniform vec3 viewPos;

// output
out vec4 FragColor;


void main(void)
{       
    vec3 color = Color;
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.5;
    
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4(.2,.2,.2,1.0);
}