in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D colorMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;
uniform int isReflectionPass;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;
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

out vec4 FragColor;

float shadowFactor(vec4 fragPosLightSpace) {
    vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj = proj * 0.5 + 0.5;
    if (proj.z > 1.0) return 0.0;
    float closestDepth = texture(shadowMap, proj.xy).r;
    float currentDepth = proj.z;
    return currentDepth - 0.005 > closestDepth ? 0.5 : 0.0;
}

void main() {
    float surfaceY = waveHeightAt(FragPos.xz);
    if (waterlineClip >  0.5 && FragPos.y < surfaceY) discard;
    if (waterlineClip < -0.5 && FragPos.y >= surfaceY) discard;
    if (isReflectionPass == 1 && FragPos.y < 0.0) discard;

    if (waterlineClip < -0.5) {
        FragColor = vec4(wireframeColor, 1.0);
        return;
    }

    vec3 albedo   = texture(colorMap, TexCoord).rgb;
    vec3 norm     = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff    = max(dot(norm, lightDir), 0.0);

    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec4 fragPosLight = lightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = shadowFactor(fragPosLight);

    vec3 ambient  = 0.2 * albedo;
    vec3 diffuse  = (1.0 - shadow) * diff * albedo;
    vec3 specular = (1.0 - shadow) * spec * vec3(0.3);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
