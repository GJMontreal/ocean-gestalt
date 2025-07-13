in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

out vec4 FragColor;

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D bumpMap;

uniform vec3 viewPos;
uniform vec3 lightPos;

void main() {
    // Base color
    vec3 albedo = texture(colorMap, fs_in.TexCoords).rgb;

    // Normal map (Tangent space to World space)
    vec3 sampledNormal = texture(normalMap, fs_in.TexCoords).rgb;
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
    vec3 normal = normalize(fs_in.TBN * sampledNormal);

    // Bump map (height) — used to perturb the normal further
    float height = texture(bumpMap, fs_in.TexCoords).r;
    vec3 bumpNormal = normalize(normal + vec3(0.0, 0.0, height * 0.1)); // simple perturbation

    vec3 finalNormal = normalize(bumpNormal);  // optionally blend with normal

    // Lighting
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, finalNormal);

    float diff = max(dot(finalNormal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

    vec3 ambient = 0.1 * albedo;
    vec3 diffuse = 0.6 * diff * albedo;
    vec3 specular = 0.3 * spec * vec3(1.0);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
