in vec4 fPosition;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
    vec3 ModelUp;  // Added for slope-aware calculations
} fs_in;

layout(std140) uniform Matrices {
    uniform mat4 projection;
    uniform mat4 view;
};

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

// Constants
const float FOAM_SLOPE_MIN = 0.05;
const float FOAM_SLOPE_MAX = 0.25;
const float SHININESS = 64.0;
const float SPECULAR_STRENGTH = 0.3;
const float BASE_REFLECTANCE = 0.02;
const vec3 FOAM_COLOR = vec3(1.0);
const vec3 DEEP_COLOR = vec3(0.0, 0.05, 0.1); // deep water hue

void main(void)
{
    vec3 baseColor = fs_in.Color;

    // --- Normalize vectors ---
    vec3 N = normalize(fs_in.Normal);
    vec3 L = normalize(lightPos - fs_in.FragPos);
    vec3 V = normalize(viewPos - fs_in.FragPos);
    vec3 H = normalize(L + V);
    vec3 upVec = normalize(fs_in.ModelUp);  // world-space up

    // --- Lighting ---
    vec3 ambient = 0.02 * baseColor;
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * baseColor;

    // --- Specular with Fresnel ---
    float spec = pow(max(dot(N, H), 0.0), SHININESS);
    float fresnel = pow(1.0 - max(dot(V, N), 0.0), 5.0);
    fresnel = mix(BASE_REFLECTANCE, 1.0, fresnel);
    vec3 specular = vec3(1.0) * spec * fresnel * SPECULAR_STRENGTH;

    // --- Foam via slope wrt model up ---
    float slope = 1.0 - dot(N, upVec);
    float foam = smoothstep(FOAM_SLOPE_MIN, FOAM_SLOPE_MAX, slope);
    vec3 foamBlend = mix(baseColor, FOAM_COLOR, foam);

    // --- Depth-based absorption ---
    float depthFade = clamp((fs_in.FragPos.z + 1.0) * 0.25, 0.0, 1.0);
    vec3 tintedBase = mix(DEEP_COLOR, foamBlend, depthFade);

    // --- Final output ---
    vec3 lighting = ambient + diffuse + specular;
    vec3 finalColor = mix(lighting, FOAM_COLOR, foam * 0.5);

    FragColor = vec4(finalColor * tintedBase, 1.0);
}
