in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    mat3 TBN;
} fs_in;

out vec4 FragColor;

uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D bumpMap;
uniform sampler2D roughnessMap;
uniform samplerCube envMap;

uniform vec3 viewPos;
uniform vec3 lightPos;

uniform float specularFactor;
uniform float bumpFactor;

void main() {
    // Base color
    vec3 albedo = texture(colorMap, fs_in.TexCoord).rgb;

    // Normal map (Tangent space to World space)
    vec3 sampledNormal = texture(normalMap, fs_in.TexCoord).rgb;
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
    vec3 normal = normalize(fs_in.TBN * sampledNormal);

    // Bump map (height) — used to perturb the normal further
    float height = texture(bumpMap, fs_in.TexCoord).r;
    vec3 bumpNormal = normalize(normal + vec3(0.0, 0.0, height * bumpFactor)); // simple perturbation

    vec3 finalNormal = normalize(bumpNormal);  // optionally blend with normal

    // Lighting
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-viewDir, finalNormal);

    float diff = max(dot(finalNormal, lightDir), 0.0);
    
    float roughness = texture(roughnessMap, fs_in.TexCoord).r; // [0,1]
    float shininess = pow(1.0 - roughness, 2.0);  // High roughness → low shininess
    
    vec3 envLighting = texture(envMap, finalNormal).rgb;
    vec3 ambient = 0.2 * albedo * envLighting;

    vec3 specularSample = texture(envMap, reflectDir).rgb;
    vec3 specular = specularFactor * shininess * specularSample;

    vec3 diffuse = 0.6 * diff * albedo;
   
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
