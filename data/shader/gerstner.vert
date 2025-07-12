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

struct GUST{
  vec3 direction;     // normalized wind direction
  float speed;        // how fast the gust moves
  float scale;        // spatial scale of modulation
  float strength;     // vertical amplitude
};

uniform GUST gust;

struct NormalMapping{
  float scale;
  float speed;
  vec3 direction;
 };

uniform NormalMapping normalMapping;

uniform int showMesh;

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

vec2 calcUV(vec3 position, vec2 dir, vec2 origin, float speed, float scale, float time){
  vec2 offset = normalize(dir) * speed * time;
  vec2 uv = (position.xz - origin) * scale;
  uv += offset;
  uv = fract(uv);
  return uv;
}

vec2 uvLocation(vec3 position, vec2 gustDir){
    vec2 gustUVOrigin = vec2(-60);  //this should be a uniform
    vec2 gustUVScale = vec2(gust.scale);
    vec2 gustOffset = normalize(gustDir) * gust.speed * time;
    vec2 uv = (position.xz - gustUVOrigin) * gustUVScale;
    uv += gustOffset;
    uv = fract(uv);
    return uv;
}

float gustDisplacement(
    vec2 uv) {
    float gustDis = texture(gustNoise, uv).r;
    return (gustDis - 0.5) * 2.0 * gust.strength;
}

const float VELOCITY_SCALE = 1.0; // Scale spatial dimensions and wave wavelengths

vec3 waveOffset(float time, vec3 aPosition, WAVE wave) {
    float safeWavelength = max(wave.wavelength, 0.01);
    float k = 2.0 * PI / safeWavelength; //what happens when wavelength is zero?
    float w = VELOCITY_SCALE * sqrt(GRAVITY * k); // only frequency is scaled
    vec2 D = normalize(wave.direction.xy);

    float phase = dot(D * k, aPosition.xz) - w * time;

    float S = sin(phase);
    float C = cos(phase);

    float y = wave.amplitude * C;

    vec2 xz = -wave.steepness *  D * S * wave.amplitude;

    return vec3(xz.x, y, xz.y);
}


vec3 calcNewPosition(vec3 aPosition){
  vec3 offset = vec3(0.0);
  for(int i=0; i < NUM_WAVES; i++){
    vec3 newOffset = waveOffset(time, aPosition, waves[i] );
    offset += newOffset;
  }
  offset = aPosition + offset;
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
    vec2 gustUV = calcUV(position, normalize(gust.direction.xy), vec2(-60), gust.speed, gust.scale, time);
    vec2 normalUV = calcUV(position, normalize(normalMapping.direction.xy), vec2(-60), normalMapping.speed, normalMapping.scale, time);

    vec3 newPosition = calcNewPosition(position);
    float gust = gustDisplacement(gustUV);
    newPosition.y += gust;
    vec3 tangent;
    vec3 bitangent;

    vec3 normalFD = calcNormal(position, newPosition, gustUV, NORMAL_OFFSET, tangent, bitangent);

    vec4 projectedPosition = projection * view * model * vec4(newPosition, 1.0);
    vec4 offscreen = vec4(2.0, 2.0, 2.0, 1.0);
    vec4 finalPosition = mix(offscreen, projectedPosition, float(showMesh));

    gl_Position = finalPosition;
   
    vs_out.FragPos = newPosition;
    vs_out.Normal = normalize(normalFD);
    vs_out.Color = vec3(color);
    vs_out.Bitangent = bitangent;
    vs_out.Tangent = tangent;
    vs_out.FragUV = normalUV;
}
