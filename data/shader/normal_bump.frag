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
uniform float time;
uniform float causticIntensity;
uniform float waterlineBias;
uniform float waterlineWidth;
uniform float waterlineStrength;
uniform float waterlineNoise;
uniform float wetStrength;
uniform float waterlineClip;
uniform vec3 wireframeColor;

struct WAVE {
    vec3 direction;
    float amplitude;
    float wavelength;
    float steepness;
    float phase;
};

#define NUM_WAVES 10
uniform WAVE waves[NUM_WAVES];

const float PI = 3.14159265358979323;
const float GRAVITY = 9.81;

vec2 waveXZDispAt(vec2 xz) {
    vec2 disp = vec2(0.0);
    for (int i = 0; i < NUM_WAVES; i++) {
        float k = 2.0 * PI / max(waves[i].wavelength, 0.01);
        float w = sqrt(GRAVITY * k);
        vec2 D = normalize(waves[i].direction.xy);
        float phase = dot(D * k, xz) - mod(w * time, 2.0 * PI) + waves[i].phase;
        disp -= waves[i].steepness * D * sin(phase) * waves[i].amplitude;
    }
    return disp;
}

float waveHeightAt(vec2 worldXZ) {
    // Approximate rest XZ by subtracting the XZ displacement at the query point
    vec2 restXZ = worldXZ - waveXZDispAt(worldXZ);

    float h = 0.0;
    for (int i = 0; i < NUM_WAVES; i++) {
        float k = 2.0 * PI / max(waves[i].wavelength, 0.01);
        float w = sqrt(GRAVITY * k);
        vec2 D = normalize(waves[i].direction.xy);
        float phase = dot(D * k, restXZ) - mod(w * time, 2.0 * PI) + waves[i].phase;
        h += waves[i].amplitude * cos(phase);
    }
    return h;
}

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float valueNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(mod(i,               289.0));
    float b = hash(mod(i + vec2(1.0, 0.0), 289.0));
    float c = hash(mod(i + vec2(0.0, 1.0), 289.0));
    float d = hash(mod(i + vec2(1.0, 1.0), 289.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p) {
    float sum = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    for (int i = 0; i < 5; ++i) {
        sum += amp * valueNoise(mod(p * freq, 289.0));
        freq *= 2.0;
        amp *= 0.5;
    }
    return sum;
}

void main() {
    float surfaceY_clip = waveHeightAt(fs_in.FragPos.xz) - waterlineBias;
    if (waterlineClip > 0.5  && fs_in.FragPos.y <  surfaceY_clip) discard;
    if (waterlineClip < -0.5 && fs_in.FragPos.y >= surfaceY_clip) discard;

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
    
    float roughness = texture(roughnessMap, fs_in.TexCoord).g; // glTF: roughness in G channel
    float shininess = pow(1.0 - roughness, 2.0);  // High roughness → low shininess
    
    vec3 envLighting = texture(envMap, finalNormal).rgb;
    vec3 ambient = 0.2 * albedo * envLighting;

    vec3 specularSample = texture(envMap, reflectDir).rgb;
    vec3 specular = specularFactor * shininess * specularSample;

    vec3 diffuse = 0.6 * diff * albedo;
   
    // Waterline effect
    float surfaceY      = waveHeightAt(fs_in.FragPos.xz);
    float bandCenter    = surfaceY - waterlineBias;
    float belowWater    = smoothstep(surfaceY + 0.2, surfaceY - 0.5, fs_in.FragPos.y);
    float waterlineBand = 1.0 - smoothstep(0.0, waterlineWidth, abs(fs_in.FragPos.y - bandCenter));

    // Break up the solid ring with noise
    float bandNoise = fbm(fs_in.FragPos.xz * 2.5 + time * 0.05);
    float noiseMask = mix(1.0, smoothstep(0.25, 0.75, bandNoise), waterlineNoise);
    waterlineBand *= noiseMask;

    // Wet hull darkening below waterline
    vec3 wetTint = vec3(0.05, 0.08, 0.1);
    vec3 color = mix(ambient + diffuse + specular, wetTint, belowWater * wetStrength);

    // Foam rim at waterline
    color = mix(color, vec3(0.9, 0.95, 1.0), waterlineBand * waterlineStrength);

    if (waterlineClip < -0.5) {
        FragColor = vec4(wireframeColor, 1.0);
        return;
    }

    // Gamma correction: linear → sRGB for display
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
