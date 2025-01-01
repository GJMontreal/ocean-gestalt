layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform mat4 model;

struct WAVE{  
  vec3 direction;
  float amplitude;
  float wavelength;
  float steepness;
};

uniform WAVE waves[3];  // increase this to 10?

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} vs_out;

uniform float time;
uniform mat3 normalMatrix;

void main(void)
{
    // for shader uniformity
    float aTime = time;
    mat3 aNormal = normalMatrix;
    vs_out.FragPos = vec3( view * model * vec4(position, 1.0));
    vs_out.Normal = normal;
    vs_out.Color = vec3(color);
    gl_Position = projection * view * model * vec4(position, 1.0) ;
}
