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

uniform WAVE waves[3];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} vs_out;

const float PI = 3.14159265358979323;

vec3 waveOffset(float time, vec3 position, WAVE wave){
    float k = 2.0f * PI / wave.wavelength;
    vec2 D = normalize(wave.direction.xy);  // normalized direction
    vec2 K = D * k;              
    float w = 2.0 * k;
    float wT = w * time;        
    float KPwT = dot(K, vec2(position)) - wT;
    float S0 = sin(KPwT);
    float C0 = cos(KPwT);
    float z = wave.amplitude * C0;
    vec2 xy = position.xy - D * wave.steepness * wave.amplitude * S0;
    return vec3(xy,z);
}

vec3 numericalDerivativeNormal(vec3 lastPosition,
                                      vec3 position,
                                      WAVE wave,
                                      float time,
                                      float offset) {
  // discrete/numerical derivative
  vec3 dx = vec3(offset, 0, 0) +
            waveOffset(time, vec3(position.x + offset, position.y,position.z), wave);
  vec3 dy = vec3(0, offset, 0) +
            waveOffset(time, vec3(position.x, position.y + offset,position.z), wave);
  vec3 normal = normalize(cross(dx - lastPosition, dy - lastPosition));
  return normal;  // flip the normal because of our winding order
}

vec3 calcWaves(vec3 aPosition){
  vec3 offset = aPosition;
  vec3 normal = vec3(0.0,0.0,0.0);
  for(int i=0; i < waves.length(); i++){
    vec3 newOffset = waveOffset(time, aPosition, waves[i] );
    offset += newOffset;
    normal = normal + numericalDerivativeNormal(newOffset,aPosition,waves[i],time,.01);
  }
  vs_out.Normal = normal * mat3(transpose(inverse(view * model)));
  return offset;
}



void main(void)
{
    vs_out.FragPos = position;
    vs_out.Normal = normal;
    vs_out.Color = vec3(color);
    vec3 offset = calcWaves(position); 
    gl_Position = projection * view * model * vec4(offset, 1.0) ;
}
