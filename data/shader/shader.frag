in vec4 fPosition;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
    vec3 ModelUp;
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
    // Normalize inputs
    vec3 normal   = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir  = normalize(viewPos - fs_in.FragPos);
    vec3 halfway  = normalize(lightDir + viewDir);

    // Ambient term
    vec3 ambient = 0.02 * color;

    // Diffuse term
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // Specular with Fresnel approximation
    float specStrength = 0.25;
    float shininess = 64.0;
    float spec = pow(max(dot(normal, halfway), 0.0), shininess);
    float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 5.0);
    vec3 specular = vec3(1.0) * spec * fresnel * specStrength;

    // Approximate foam where surface slope is steep
    float slope = 1.0 - dot(normal, fs_in.ModelUp);  // how non-horizontal the surface is
    float foam = smoothstep(0.1, 0.3, slope);              // adjust thresholds as needed
    vec3 foamColor = vec3(1.0);                            // white foam
    
    vec3 lightColor = ambient + diffuse + specular;
    vec3 finalColor = mix(lightColor, foamColor, foam);
    FragColor = vec4(finalColor, 1.0);
}
