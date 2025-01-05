in vec3 position;
in vec3 normal;
in vec4 color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float time;
uniform mat3 normalMatrix;

struct WAVE{  
  vec3 direction;
  float amplitude;
  float wavelength;
  float steepness;
};

uniform WAVE waves[6];  

struct PARTICLE{
  vec3 position;
  vec3 normal;
};

//TO DO: make capitalization consistent throughout
out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

const float PI = 3.14159265358979323;
const float speedScale = 3.0;

//Why is it moving with the camera
vec3 waveOffset(float time, vec3 aPosition, WAVE wave){
    float k =  2.0f * PI / wave.wavelength;
    vec2 D = normalize(wave.direction.xy);  // normalized direction
    vec2 K = D * k;              
    float w = speedScale * k;
    float wT = w * time;        
    float KPwT = dot(K, vec2(aPosition)) - wT;
    float S0 = sin(KPwT);
    float C0 = cos(KPwT);
    // float z = wave.amplitude * C0 - (wave.amplitude / 2.0 ); // 0 is our mean height
     float z = wave.amplitude * C0;
    vec2 xy = aPosition.xy - D * wave.steepness * wave.amplitude * S0; //are we sure about this?
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
  return normal; 
}

PARTICLE calcWaves(vec3 aPosition){
  vec3 offset = aPosition;
  vec3 normal = vec3(0.0,0.0,0.0);
  for(int i=0; i < waves.length(); i++){
    vec3 newOffset = waveOffset(time, aPosition, waves[i] );
    offset += newOffset;
    normal = normal + numericalDerivativeNormal(newOffset,aPosition,waves[i],time,.001);
  }
  PARTICLE particle;
  particle.normal = normalMatrix * normal;
  particle.position = offset;
  return particle;
}

void main(void)
{
    Color = vec3(color);
    PARTICLE particle = calcWaves(position);       
    gl_Position = projection * view * model * vec4(particle.position, 1.0) ;
    FragPos = vec3(view*model*vec4(particle.position,1.0));
    Normal = particle.normal;
}
