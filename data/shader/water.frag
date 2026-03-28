in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
    vec3 Tangent;
    vec3 Bitangent;
    vec2 FragUV;
    float Crestness;
} fs_in;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform sampler2D normalMap; 
uniform samplerCube envMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

// uniform vec4 fogColor;

// Output
out vec4 FragColor;

uniform float fogDensity;

uniform float causticScale;
uniform float causticSpeed;
uniform float causticIntensity;

const vec3 CAUSTIC_COLOUR = vec3(1.0, 0.9, 0.7); //another uniform perhaps

uniform float foamScale;
uniform float foamScrollSpeed;
uniform float foamSlopeMin;
uniform float foamSlopeMax;
uniform float foamSlopeAmplifier;


uniform int showNormalMap;
uniform int showUVs;
uniform int debugLightPos;
uniform int debugFragPos;
uniform int debugDiffuse;
uniform int showEnv;

const vec3 F0 = vec3(0.04); // typical for dielectrics, metals use albedo or tuned value

vec3 visualizeLightContribution(vec3 lightPos, vec3 fragPos) {
    float intensity = 1.0 / distance(lightPos, fragPos); // or squared falloff
    intensity = clamp(intensity * 0.2, 0.0, 1.0); // optional scale
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

float calcFoam(vec3 position){
  
  float slope = length(vec2(dFdx(fs_in.FragPos.y), dFdy(fs_in.FragPos.y))) * foamSlopeAmplifier;
// --- Slope direction approximates foam flow ---
vec2 slopeDir = normalize(vec2(
    dFdx(position.y),
    dFdy(position.y)
));


// --- Flowing foam UVs: move along slope direction over time ---
  vec2 foamUV = position.xz + slopeDir * time * foamScrollSpeed;
  float foamNoise = fbm(foamUV * foamScale);
  // --- Modulate foam by surface slope: appears only on steep regions ---
  float foamMask = smoothstep(foamSlopeMin, foamSlopeMax, slope);
  float foam = foamNoise * foamMask;
  foam = pow(foam, 0.5);
  return foam;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Main
void main() {
    mat3 TBN = mat3(normalize(fs_in.Tangent),
                normalize(fs_in.Bitangent),
                normalize(fs_in.Normal));

    vec3 sampledNormalTS = normalize(texture(normalMap, fs_in.FragUV).rgb * 2.0 - 1.0);
    vec3 normal = normalize(TBN * sampledNormalTS);
    // vec3 normal = normalize(mix(fs_in.Normal, normalWS, normalStrength));
    
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir  = normalize(viewPos - fs_in.FragPos);
    vec3 halfway  = normalize(lightDir + viewDir);

    // Lighting terms
    float diff    = max(dot(normal, lightDir), 0.0);

    vec3 reflectedDir = reflect(-viewDir, normal);
    vec3 envReflection = texture(envMap, reflectedDir).rgb;

    float cosTheta = max(dot(viewDir, normal), 0.0);
    
    vec3 fresnel = fresnelSchlick(cosTheta, F0);

    vec3 reflection = fresnel * envReflection;
 
    vec3 diffuse  = (1.0 - fresnel) * diff * fs_in.Color;

    // Depth-based colour modulation
    float depth = length(viewPos - fs_in.FragPos);
    float depthFade = smoothstep(10.0, 80.0, depth);
    vec3 deepColor = fs_in.Color * vec3(0.1, 0.2, 0.3);
    vec3 shiftedColor = mix(fs_in.Color, deepColor, depthFade);

    // Caustic flicker in troughs
    float causticStrength = smoothstep(-0.6, 0.1, -fs_in.FragPos.y);

    vec2 flickerUV = vec2(fs_in.FragPos.x * 2.0, fs_in.FragPos.z * 0.75); // stretched FBM domain
    flickerUV += vec2(time * 0.3, time * 0.1);
    float causticFlicker = fbm(flickerUV);
    causticFlicker = smoothstep(0.55, 0.8, causticFlicker);

    float NdotL = max(dot(normalize(fs_in.Normal), normalize(lightPos - fs_in.FragPos)), 0.0);
    causticFlicker *= NdotL;

    causticFlicker = pow(causticFlicker, 6.0);
    
    float causticMask = 1.0;
    vec3 causticLight = CAUSTIC_COLOUR * causticFlicker * causticMask * causticStrength * causticIntensity;

    //Gamma correction
    shiftedColor = pow(shiftedColor, vec3(0.45));

    vec3 finalColor =
    reflection  +
    diffuse  +
    causticLight; 

    // Slope-based surface foam
    float foam = calcFoam(fs_in.FragPos);

    // Crest-driven whitecap: brighter, FBM-textured white at wave crests.
    float whitecapNoise = fbm(fs_in.FragPos.xz * 1.8, time);
    float whitecap = smoothstep(0.45, 0.75, fs_in.Crestness) * whitecapNoise;

    vec3 foamColor = vec3(1.0);
    finalColor = mix(finalColor, foamColor, clamp(foam + whitecap, 0.0, 1.0));

    // Fog
    float fogFactor = clamp(exp(-fogDensity * depth), 0.0, 1.0);
    vec3 fogColor = vec3(0.4, 0.6, 0.7); // sky-ish blue
    finalColor = mix(fogColor, finalColor, fogFactor);
    
    finalColor = mix(finalColor, sampledNormalTS * 0.5 + 0.5, float(showNormalMap));
    finalColor = mix(finalColor, vec3(fs_in.FragUV,0.0), float(showUVs));
    finalColor = mix(finalColor, diffuse, float(debugDiffuse));

    vec3 lightViz = visualizeLightContribution(lightPos, fs_in.FragPos);
    finalColor = mix(finalColor, lightViz, float(debugLightPos));

    vec3 fragViz = normalize(fs_in.FragPos);
    finalColor = mix(finalColor, fragViz, float(debugFragPos));
    finalColor = mix(finalColor, reflection, float(showEnv));

    FragColor = vec4(finalColor, 1.0);
}
