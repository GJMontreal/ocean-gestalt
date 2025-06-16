// The analytic surface normals were derived from the parametric Gerstner wave model as described by Tessendorf (2001) and implemented in GPU-based systems (Finch, 2004). These are obtained by taking partial derivatives of the wave function with respect to horizontal coordinates and computing their cross product.
/*
Tessendorf, J. (2001). Simulating Ocean Water. In ACM SIGGRAPH Course Notes.

Finch, M. (2004). Simulating Ocean Water. In GPU Gems (Chapter 1). NVIDIA Corporation.

*/
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;

};

uniform mat4 model;

uniform float time;

uniform sampler2D gustNoise;
// uniform sampler2D gustNormalMap;

uniform vec3 direction;     // normalized wind direction
uniform float gustSpeed;        // how fast the gust moves
uniform float gustScale;        // spatial scale of modulation
uniform float gustStrength;     // vertical amplitude

struct WAVE{  
  vec3 direction;
  float amplitude;
  float wavelength;
  float steepness;
};

#define NUM_WAVES 10
#define NUM_WAVES_INPUT 10
#define NORMAL_OFFSET 0.01

uniform WAVE waves[NUM_WAVES_INPUT];  

//TO DO: make capitalization consistent throughout
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
    vec3 Tangent;
    vec3 Bitangent;
    vec2 FragUV;
} vs_out;
    
const float PI = 3.14159265358979323;
const float speedScale = 3.0;

const float GRAVITY = 9.81; 

vec2 uvLocation(vec3 position, vec2 gustDir){
    vec2 gustUVOrigin = vec2(-20,-20);
    vec2 gustUVScale = vec2(gustScale,gustScale);
    vec2 uv = (position.xz - gustUVOrigin) * gustUVScale;
    uv += gustDir * time * gustSpeed;
    return uv;
}

float gustDisplacement(
    vec2 uv) {
    float gust = texture(gustNoise, uv).r;
    return (gust - 0.5) * 2.0 * gustStrength;
}

const float VELOCITY_SCALE = 1.0; // Scale spatial dimensions and wave wavelengths

vec3 waveOffset(float time, vec3 aPosition, WAVE wave) {
    float k = 2.0 * PI / wave.wavelength;
    float w = VELOCITY_SCALE * sqrt(GRAVITY * k); // only frequency is scaled
    vec2 D = normalize(wave.direction.xy);

    float phase = dot(D * k, aPosition.xz) - w * time;

    float S = sin(phase);
    float C = cos(phase);

    float y = wave.amplitude * C;
    vec2 xz = aPosition.xz - D * wave.steepness * S * wave.amplitude;

    return vec3(xz.x, y, xz.y);
}


vec3 calcNewPosition(vec3 aPosition){
  vec3 offset = vec3(0.0);
  for(int i=0; i < NUM_WAVES; i++){
    vec3 newOffset = waveOffset(time, aPosition, waves[i] );
    offset += newOffset;
  }
  offset = aPosition + offset / float(NUM_WAVES);
  return offset;
}

vec3 calcNormal(vec3 originalPosition,
                vec3 newPosition,
                vec2 uv,
                float offset, out vec3 bitangent, out vec3 tangent) {
    vec3 posOffsetX = vec3(originalPosition.x + offset, 0.0, originalPosition.z);
    vec3 displacedX = calcNewPosition(posOffsetX);
    float gustX = gustDisplacement(uv + vec2(offset, 0.0));
    displacedX.y += gustX;

    vec3 posOffsetZ = vec3(originalPosition.x, 0.0, originalPosition.z + offset);
    vec3 displacedZ = calcNewPosition(posOffsetZ);
    float gustZ = gustDisplacement(uv + vec2(0.0, offset));
    displacedZ.y += gustZ;

    tangent = displacedX - newPosition;
    bitangent = displacedZ - newPosition;

    vec3 normal = normalize(cross(bitangent, tangent));
    return normal;
}

void main(void)
{   
    vec2 windDirection = normalize(direction.xy);
    vec2 uv = uvLocation(position, windDirection);
    // vec3 sampledNormal = texture(gustNormalMap, uv).rgb * 2.0 - 1.0;
    // sampledNormal = vec3(0, 0, 1);

    vec3 newPosition = calcNewPosition(position);
    float gust = gustDisplacement(uv);
    newPosition.y += gust;
    vec3 tangent;
    vec3 bitangent;

    vec3 normalFD = calcNormal(position, newPosition, uv, NORMAL_OFFSET, tangent, bitangent);

    // mat3 TBN = mat3(normalize(tangent), normalize(bitangent), normalize(normalFD));

    gl_Position = projection * view * model * vec4(newPosition, 1.0) ;
    vs_out.FragPos = vec3(model * vec4(newPosition,1.0));

    // vec3 finalNormal = normalize(mix(normalFD, sampledNormal, gustStrength));
    // vec3 mappedNormal = normalize(TBN * sampledNormal);

    vs_out.Normal = normalize(normalFD);
    // vs_out.DebugNormal = normalize(mappedNormal);
    vs_out.Color = vec3(color);
    vs_out.Bitangent = bitangent;
    vs_out.Tangent = tangent;
    vs_out.FragUV = uv;
    //we don't need to use this modelUp since we've corrected our model's coordinate system
    // vec3 modelUp = normalize(mat3(model) * vec3(0.0, 0.0, 1.0));
    // vs_out.ModelUp = modelUp;
}
