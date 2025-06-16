in vec4 fPosition;

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

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

// Output
out vec4 FragColor;

// Constants
const float FOG_DENSITY = 0.01;
const float AMBIENT_STRENGTH = 0.9;
const float SPECULAR_STRENGTH = 0.9;
const float SHININESS = 8.0;

uniform float causticScale;
uniform float causticSpeed;
uniform float causticIntensity;

const vec3 CAUSTIC_COLOUR = vec3(1.0, 0.9, 0.7);

uniform float foamScale;
uniform float foamScrollSpeed;
uniform float foamSlopeMin;
uniform float foamSlopeMax;
uniform float foamSlopeAmplifier;

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

vec3 calcFoam(vec3 position){
  
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
  return vec3(foam);
}


// Main
void main() {
  
    mat3 TBN = mat3(normalize(fs_in.Tangent),
                normalize(fs_in.Bitangent),
                normalize(fs_in.Normal));
    

    vec3 sampledNormal = texture(normalMap, fs_in.FragUV).rgb;
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
    vec3 normal = normalize(TBN * sampledNormal);

    // vec3 normal   = normalize(fs_in.Normal);
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

    float lightingIntensity = diff + spec + fresnel;

    shiftedColor = pow(shiftedColor, vec3(0.45));

    vec3 finalColor =
    ambient  * shiftedColor +
    diffuse * shiftedColor +
    specular * shiftedColor +            // white or lightly tinted
    causticLight; 

    lightingIntensity = clamp(lightingIntensity, 0.0, 1.0);
    
    // Foam blend
    vec3 foamColor = calcFoam(fs_in.FragPos);
    finalColor = mix(finalColor, foamColor, foamColor.r);

    // Fog
    float fogFactor = clamp(exp(-FOG_DENSITY * depth), 0.0, 1.0);
    vec3 fogColor = vec3(0.4, 0.6, 0.7); // sky-ish blue
    vec3 fogged = mix(fogColor, finalColor, fogFactor);

    FragColor = vec4(fogged, 1.0);
}
