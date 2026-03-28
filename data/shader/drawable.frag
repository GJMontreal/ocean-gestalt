
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
uniform vec4 lineColor;

// output
out vec4 FragColor;

void main(void)
{    
  vec3 lightDir = normalize(viewPos - fs_in.FragPos);
  vec3 norm = normalize(fs_in.Normal);

  vec3 _ = lightPos; // To keep our uniforms consistent between shaders
  _ = fs_in.Color;
  
  // fake diffuse shading based on view angle
    float diff = max(dot(norm, lightDir), 0.0);

    // ambient + view-aligned diffuse
    float ambientStrength = 0.2;
    vec3 color = (ambientStrength + diff) * fs_in.Color;
    FragColor = vec4(color, 1.0);
}