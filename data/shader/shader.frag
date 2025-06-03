in vec4 fPosition;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
    vec3 ModelUp;
    float FoamModulation;
} fs_in;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

// Output
out vec4 FragColor;

// Constants
const float FOG_DENSITY = 0.01;
const float AMBIENT_STRENGTH = 0.001;
const float SPECULAR_STRENGTH = 0.125;
const float SHININESS = 16.0;

const float FOAM_SLOPE_MIN = 0.1;
const float FOAM_SLOPE_MAX = 0.3;

const float CAUSTIC_INTENSITY = 0.125;
const float CAUSTIC_SCALE = 4.0;
const float CAUSTIC_SPEED = 0.4;

const float FOAM_BRIGHTNESS_BASE     = 0.4;  // Minimum foam brightness
const float FOAM_BRIGHTNESS_VARIANCE = 0.2;  // Additional brightness from FBM modulation

// Hash and FBM
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float valueNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p) {
    float sum = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    for (int i = 0; i < 5; ++i) {
        sum += amp * valueNoise(p * freq);
        freq *= 2.0;
        amp *= 0.5;
    }
    return sum;
}

// Main
void main() {
    vec3 normal   = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir  = normalize(viewPos - fs_in.FragPos);
    vec3 halfway  = normalize(lightDir + viewDir);

    // Lighting terms
    vec3 ambient  = AMBIENT_STRENGTH * fs_in.Color;
    float diff    = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = diff * fs_in.Color;
    float spec    = pow(max(dot(normal, halfway), 0.0), SHININESS);
    float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 5.0);
    vec3 specular = vec3(1.0) * spec * fresnel * SPECULAR_STRENGTH;

    // Depth-based colour modulation
    float depth = length(viewPos - fs_in.FragPos);
    float depthFade = smoothstep(10.0, 80.0, depth);
    vec3 deepColor = fs_in.Color * vec3(0.1, 0.2, 0.3);
    vec3 shiftedColor = mix(fs_in.Color, deepColor, depthFade);

    // Slope-based foam
    float slope = 1.0 - dot(normal, fs_in.ModelUp);
    float foamBase = smoothstep(FOAM_SLOPE_MIN, FOAM_SLOPE_MAX, slope);

    float foamMask = foamBase * fs_in.FoamModulation;
    vec3 foamColor = vec3(1.0) * (FOAM_BRIGHTNESS_BASE + FOAM_BRIGHTNESS_VARIANCE * fs_in.FoamModulation);
    
    // Optional: make foam colour slightly modulated
    // vec3 foamColor = vec3(1.0) * (0.8 + 0.2 * foamNoise);

    // Caustic flicker in troughs
    float causticStrength = smoothstep(-0.2, 0.2, -fs_in.FragPos.y);
    float causticFlicker = fbm(fs_in.FragPos.xz * CAUSTIC_SCALE + vec2(time * CAUSTIC_SPEED));
    float causticMask = 1.0 - foamMask;
    vec3 causticLight = vec3(1.0, 0.9, 0.7) * causticFlicker * causticMask * causticStrength * CAUSTIC_INTENSITY;

    // Combine lighting
    vec3 litColor = (ambient + diffuse + specular + causticLight) * shiftedColor;

    // Foam blend
    vec3 finalColor = mix(litColor, foamColor, foamMask);

    // Fog
    float fogFactor = clamp(exp(-FOG_DENSITY * depth), 0.0, 1.0);
    vec3 fogColor = vec3(0.4, 0.6, 0.7); // sky-ish blue
    vec3 fogged = mix(fogColor, finalColor, fogFactor);

    FragColor = vec4(fogged, 1.0);
}
