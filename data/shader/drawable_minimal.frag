in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} fs_in;


uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float ambientStrength;

out vec4 FragColor;


void main() {
    vec3 _ = viewPos;
    // Normalize interpolated normal
    vec3 normal = normalize(fs_in.Normal);
    
    // Compute lighting direction
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    // Lambertian diffuse term
    float diff = max(dot(normal, lightDir), 0.0);

    // Final color is base color modulated by diffuse term
    vec3 color = fs_in.Color * diff;

    color = mix(color, fs_in.Color, ambientStrength);
    FragColor = vec4(color, 1.0);
}
