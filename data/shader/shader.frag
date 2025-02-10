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

uniform vec3 lightPos;
uniform vec3 viewPos;

void main(void)
{       
    vec3 color = fs_in.Color;
    // ambient
    vec3 ambient = 0.001 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    vec3 specular = vec3(0.25) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
