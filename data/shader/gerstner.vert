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

uniform WAVE waves[10];  

struct PARTICLE{
  vec3 position;
  vec3 normal;
};

//TO DO: make capitalization consistent throughout
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} vs_out;

const float PI = 3.14159265358979323;
const float speedScale = 3.0;

vec3 waveOffset(float time, vec3 aPosition, WAVE wave){
    float k =  2.0f * PI / wave.wavelength;
    vec2 D = normalize(wave.direction.xy);  // normalized direction
    vec2 K = D * k;              
    float w = speedScale * k;
    float wT = w * time;        
    float KPwT = dot(K, vec2(aPosition)) - wT;
    float S0 = sin(KPwT);
    float C0 = cos(KPwT);
    float z = wave.amplitude * C0;
    vec2 xy = aPosition.xy - D * wave.steepness * S0 * wave.amplitude; //are we sure about this?
    return vec3(xy,z);
}


vec3 calcNewPosition(vec3 aPosition){
  vec3 offset = aPosition;
  for(int i=0; i < waves.length(); i++){
    vec3 newOffset = waveOffset(time, aPosition, waves[i] );
    offset += newOffset;
  }
  offset = offset / waves.length();
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
    vs_out.Color = vec3(color);
    vec3 newPosition = calcNewPosition(position);
    vec3 normal = calcNormal(position, newPosition, .01);   
    gl_Position = projection * view * model * vec4(newPosition, 1.0) ;
    vs_out.FragPos = vec3(model * vec4(newPosition,1.0));
    vs_out.Normal = vec3(model * vec4(normal,1.0));
}
