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
    vec3 ModelUp;
} vs_out;

const float PI = 3.14159265358979323;
const float speedScale = 3.0;

// Fractal Brownian Motion
const float FBM_SCALE = 0.2;           // spatial frequency
const float FBM_TIME_SPEED_X = 0.1;    // x scroll speed
const float FBM_TIME_SPEED_Y = 0.07;   // y scroll speed
const float FBM_AMPLITUDE = 0.2;      // max vertical displacement
const float FBM_MODULATION_MIN = 0.0;  // waveHeight threshold low
const float FBM_MODULATION_MAX = 1.0;  // waveHeight threshold high

const float GRAVITY = 9.81; 

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

    vec2 u = f * f * (3.0 - 2.0 * f);  // smoothstep interpolation

    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p) {
    float sum = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;

    for (int i = 0; i < 5; ++i) {
        sum += amplitude * valueNoise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }

    return sum;
}

const float VELOCITY_SCALE = 1.0; // Scale spatial dimensions and wave wavelengths

vec3 waveOffset(float time, vec3 aPosition, WAVE wave) {
    float k = 2.0 * PI / wave.wavelength;
    float w = VELOCITY_SCALE * sqrt(GRAVITY * k); // only frequency is scaled
    vec2 D = normalize(wave.direction.xy);

    float phase = dot(D * k, aPosition.xy) - w * time;

    float S = sin(phase);
    float C = cos(phase);

    float z = wave.amplitude * C;
    vec2 xy = aPosition.xy - D * wave.steepness * S * wave.amplitude;

    return vec3(xy, z);
}


vec3 calcNewPosition(vec3 aPosition){
  vec3 offset = vec3(0.0);
  for(int i=0; i < NUM_WAVES; i++){
    vec3 newOffset = waveOffset(time, aPosition, waves[i] );
    offset += newOffset;
  }
  offset = aPosition + offset / float(NUM_WAVES);

  vec2 q = offset.xy * FBM_SCALE + vec2(time * FBM_TIME_SPEED_X, time * FBM_TIME_SPEED_Y);
  float n = fbm(q);

  float waveHeight = offset.z - aPosition.z;
  float modulation = smoothstep(FBM_MODULATION_MIN, FBM_MODULATION_MAX, abs(waveHeight));
  
  offset.z += n * FBM_AMPLITUDE * modulation;

  return offset;
}

vec3 calcNormal(vec3 originalPosition,
                vec3 newPosition,
                float offset) {
  vec3 tangent =  calcNewPosition(vec3(originalPosition.x + offset, originalPosition.y, 0)) - newPosition; 
  vec3 bitangent = calcNewPosition(vec3(originalPosition.x, originalPosition.y + offset, 0)) - newPosition; 
  vec3 normal = normalize(cross(tangent , bitangent)) ;
  return normal;
}

void main(void)
{
    vec3 newPosition = calcNewPosition(position);
    vec3 normalFD = calcNormal(position, newPosition, NORMAL_OFFSET);
    gl_Position = projection * view * model * vec4(newPosition, 1.0) ;
    vs_out.FragPos = vec3(model * vec4(newPosition,1.0));
    vs_out.Normal = normalize(transpose(inverse(mat3(model))) * normalFD);
    vs_out.Color = vec3(color);
    vec3 modelUp = normalize(mat3(model) * vec3(0.0, 0.0, 1.0));
    vs_out.ModelUp = modelUp;
}
