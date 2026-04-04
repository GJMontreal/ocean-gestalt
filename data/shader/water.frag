in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
    vec3 Tangent;
    vec3 Bitangent;
    vec2 FragUV;
} fs_in;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform sampler2D normalMap;
uniform samplerCube envMap;
uniform sampler2D reflectionTex;
uniform mat4 reflectionMatrix;
uniform float reflectionDistortion;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

out vec4 FragColor;

uniform float fogDensity;
uniform vec3 fogColor;

uniform float causticScale;
uniform float causticSpeed;
uniform float causticIntensity;
uniform vec3 causticColor;
uniform float causticTroughMin;
uniform float causticTroughMax;
uniform float causticThresholdMin;
uniform float causticThresholdMax;
uniform float causticSharpness;

uniform float foamScale;
uniform float foamScrollSpeed;
uniform float foamSlopeMin;
uniform float foamSlopeMax;
uniform float foamSlopeAmplifier;
uniform float foamPower;

uniform float depthFadeNear;
uniform float depthFadeFar;
uniform vec3 deepWaterTint;

uniform float fresnelF0;
uniform float gamma;

uniform int showNormalMap;
uniform int showUVs;
uniform int debugLightPos;
uniform int debugFragPos;
uniform int debugDiffuse;
uniform int showEnv;
uniform int showReflection;


vec3 visualizeLightContribution(vec3 lightPos, vec3 fragPos) {
    float intensity = 1.0 / distance(lightPos, fragPos);
    intensity = clamp(intensity * 0.2, 0.0, 1.0);
    vec3 lightColor = vec3(1.0 - intensity, 0.0, intensity);
    return lightColor;
}

// Hash and FBM
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float valueNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    // Wrap grid coords to [0,289) so hash inputs stay bounded at all frequencies
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
        // Wrap per-octave so fract() always operates on small values
        sum += amp * valueNoise(mod(p * freq, 289.0));
        freq *= 2.0;
        amp *= 0.5;
    }
    return sum;
}

float calcFoam(vec3 position){
    float slope = length(vec2(dFdx(fs_in.FragPos.y), dFdy(fs_in.FragPos.y))) * foamSlopeAmplifier;
    vec2 slopeDir = normalize(vec2(dFdx(position.y), dFdy(position.y)));
    vec2 foamUV = position.xz + slopeDir * time * foamScrollSpeed;
    float foamNoise = fbm(foamUV * foamScale);
    float foamMask = smoothstep(foamSlopeMin, foamSlopeMax, slope);
    float foam = foamNoise * foamMask;
    foam = pow(foam, foamPower);
    return foam;
}

float shadowPCF(vec3 fragPos, vec3 normal, vec3 lightDir) {
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 1.0;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float currentDepth = projCoords.z - bias;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth < pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

vec3 fresnelSchlick(float cosTheta, float f0)
{
    return vec3(f0) + (1.0 - vec3(f0)) * pow(1.0 - cosTheta, 5.0);
}

// Main
void main() {
    mat3 TBN = mat3(normalize(fs_in.Tangent),
                normalize(fs_in.Bitangent),
                normalize(fs_in.Normal));

    vec3 sampledNormalTS = normalize(texture(normalMap, fs_in.FragUV).rgb * 2.0 - 1.0);
    vec3 normal = normalize(TBN * sampledNormalTS);

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir  = normalize(viewPos - fs_in.FragPos);
    vec3 halfway  = normalize(lightDir + viewDir);

    // Lighting terms
    float diff    = max(dot(normal, lightDir), 0.0);
    float shadow  = shadowPCF(fs_in.FragPos, normal, lightDir);

    vec3 reflectedDir = reflect(-viewDir, normal);
    vec3 envReflection = texture(envMap, reflectedDir).rgb;

    // Planar reflection
    vec4 clipSpaceRefl = reflectionMatrix * vec4(fs_in.FragPos, 1.0);
    vec2 reflUV = (clipSpaceRefl.xy / clipSpaceRefl.w) * 0.5 + 0.5;
    reflUV += vec2(normal.x, normal.z) * reflectionDistortion;
    reflUV = clamp(reflUV, 0.001, 0.999);
    vec3 planarRefl = texture(reflectionTex, reflUV).rgb;

    float cosTheta = max(dot(viewDir, normal), 0.0);
    float planarWeight = 1.0 - cosTheta;
    vec3 combinedReflection = mix(envReflection, planarRefl, planarWeight);

    vec3 fresnel = fresnelSchlick(cosTheta, fresnelF0);

    vec3 reflection = fresnel * combinedReflection;


    // Depth-based colour modulation
    float depth = length(viewPos - fs_in.FragPos);
    float depthFade = smoothstep(depthFadeNear, depthFadeFar, depth);
    vec3 deepColor = fs_in.Color * deepWaterTint;
    vec3 shiftedColor = mix(fs_in.Color, deepColor, depthFade);

   // Gamma correction
    shiftedColor = pow(shiftedColor, vec3(gamma));
    
    vec3 diffuse  = (1.0 - fresnel) * diff * shadow * shiftedColor;

    // Caustic flicker in troughs
    float causticStrength = smoothstep(causticTroughMin, causticTroughMax, -fs_in.FragPos.y);


    vec2 flickerUV = fs_in.FragPos.xz * causticScale;
    flickerUV += time * causticSpeed;
    float causticFlicker = fbm(flickerUV);
    causticFlicker = smoothstep(causticThresholdMin, causticThresholdMax, causticFlicker);

    float NdotL = max(dot(normalize(fs_in.Normal), normalize(lightPos - fs_in.FragPos)), 0.0);
    causticFlicker *= NdotL;

    causticFlicker = pow(causticFlicker, causticSharpness);

    vec3 causticLight = causticColor * causticFlicker * causticStrength * causticIntensity;



    vec3 finalColor =
    reflection  +
    diffuse  +
    causticLight;

    // Foam blend
    float foam = calcFoam(fs_in.FragPos);
    vec3 foamColor = vec3(1.0);
    finalColor = mix(finalColor, foamColor, foam);

    // Fog
    float fogFactor = clamp(exp(-fogDensity * depth), 0.0, 1.0);
    finalColor = mix(fogColor, finalColor, fogFactor);

    finalColor = mix(finalColor, sampledNormalTS * 0.5 + 0.5, float(showNormalMap));
    finalColor = mix(finalColor, vec3(fs_in.FragUV,0.0), float(showUVs));
    finalColor = mix(finalColor, diffuse, float(debugDiffuse));

    vec3 lightViz = visualizeLightContribution(lightPos, fs_in.FragPos);
    finalColor = mix(finalColor, lightViz, float(debugLightPos));

    vec3 fragViz = normalize(fs_in.FragPos);
    finalColor = mix(finalColor, fragViz, float(debugFragPos));
    finalColor = mix(finalColor, reflection, float(showEnv));
    finalColor = mix(finalColor, planarRefl, float(showReflection));
    FragColor = vec4(finalColor, 1.0);
}
