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
    vec3 ModelUp;
} vs_out;

const float PI = 3.14159265358979323;
const float speedScale = 3.0;

const float GRAVITY = 9.81; 

float gustDisplacement(
    vec3 position, vec2 gustDir) {
    vec2 gustUVOrigin = vec2(-20,-20);
    vec2 gustUVScale = vec2(gustScale,gustScale);
    vec2 uv = (position.xy - gustUVOrigin) * gustUVScale;
    uv += gustDir * time * gustSpeed;
    float gust = texture(gustNoise, uv).r;
    return (gust - 0.5) * 2.0 * gustStrength;
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
  return offset;
}

vec3 calcNormal(vec3 originalPosition,
                vec3 newPosition,
                vec2 windDir,
                float offset) {
  
  vec3 someVec =calcNewPosition(vec3(originalPosition.x + offset, originalPosition.y, 0));
  someVec.z += gustDisplacement(someVec, windDir);

  vec3 tangent =  someVec - newPosition;

  vec3 someOtherVec = calcNewPosition(vec3(originalPosition.x, originalPosition.y + offset, 0));
  someOtherVec.z += gustDisplacement(someOtherVec, windDir);

  vec3 bitangent =  someOtherVec - newPosition; 

  vec3 normal = normalize(cross(tangent , bitangent)) ;
  return normal;
}

void main(void)
{
    vec2 windDirection = normalize(direction.xy);
    vec3 newPosition = calcNewPosition(position);
    float gust = gustDisplacement(newPosition, windDirection);
    newPosition.z += gust;
    vec3 normalFD = calcNormal(position, newPosition, windDirection, NORMAL_OFFSET);

    gl_Position = projection * view * model * vec4(newPosition, 1.0) ;
    vs_out.FragPos = vec3(model * vec4(newPosition,1.0));
    vs_out.Normal = normalize(transpose(inverse(mat3(model))) * normalFD);
    vs_out.Color = vec3(color);
    vec3 modelUp = normalize(mat3(model) * vec3(0.0, 0.0, 1.0));
    vs_out.ModelUp = modelUp;
}
