// The analytic surface normals were derived from the parametric Gerstner wave model as described by Tessendorf (2001) and implemented in GPU-based systems (Finch, 2004). These are obtained by taking partial derivatives of the wave function with respect to horizontal coordinates and computing their cross product.
/*
Tessendorf, J. (2001). Simulating Ocean Water. In ACM SIGGRAPH Course Notes.

Finch, M. (2004). Simulating Ocean Water. In GPU Gems (Chapter 1). NVIDIA Corporation.

*/
layout(location = 0) in vec2 ndcCoord;
// layout (location = 1) in vec4 color;  //we could use this to vary depth

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;

};

uniform float time;

uniform sampler2D gustNoise;

struct GUST{
  vec3 direction;     // normalized wind direction
  float speed;        // how fast the gust moves
  float scale;        // spatial scale of modulation
  float strength;     // vertical amplitude
};

uniform GUST gust;

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

uniform vec3 cameraPos;
uniform float fovYRadians;
uniform float aspect;

// uniform mat4 invViewProjection;

const float planeY = 0.0;
uniform float ndcScale;
const vec2 clipMin = vec2(-30.0,-30.0); // e.g., vec2(-10.0, -5.0)
const vec2 clipMax = vec2(30.0,30.0); // e.g., vec2(10.0, 5.0)

//TO DO: make capitalization consistent throughout
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
    vec3 Tangent;
    vec3 Bitangent;
    vec2 FragUV;
} vs_out;
    
// out vec3 Color;
const float PI = 3.14159265358979323;
const float speedScale = 3.0;

const float GRAVITY = 9.81; 

vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float safeY = sign(dir.y) * max(abs(dir.y), 1e-5);
    float t = (yPlane - origin.y) / safeY;
    return origin + dir * t;
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

bool isFinite(vec3 v) {
    return !any(isnan(v)) && !any(isinf(v));
}


void main(void)
{   
      // Step 1: Build ray direction in view space
    float tanHalfFovY = tan(0.5 * fovYRadians);

    vec2 scaledNDC = ndcCoord * ndcScale;

    vec3 rayViewDir = normalize(vec3(
        ndcCoord.x * aspect * tanHalfFovY,
        ndcCoord.y * tanHalfFovY,
        -1.0 // into screen
    ));

    //     vec3 rayViewDir = normalize(vec3(
    //     ndcCoord.x,
    //     ndcCoord.y,
    //     -1.0 // into screen
    // ));

    // I don't know that this is correct

    // Step 2: Rotate to world space using view matrix inverse
    mat3 viewRot = mat3(inverse(view)); // inverse of rotation-only view
    vec3 rayWorldDir = viewRot * rayViewDir;

    // Step 3: Raycast to ground
    vec3 position = intersectRayWithPlane(cameraPos, rayWorldDir, planeY);

    position = position * ndcScale;
 // Step 4: Apply world-space rectangle clipping
    vec2 posXZ = position.xz;

    // if (any(lessThan(posXZ, clipMin)) || any(greaterThan(posXZ, clipMax))) {
    //     gl_Position = vec4(0.0/0.0); // OpenGl says we need NaN in order to cull
    //     return;
    // }
   
    vec2 windDirection = normalize(gust.direction.xy);
    vec2 uv = uvLocation(position, windDirection);

    vec3 newPosition = calcNewPosition(position);
    float gust = gustDisplacement(uv);
    newPosition.y += gust;
    vec3 tangent;
    vec3 bitangent;

    vec3 normalFD = calcNormal(position, newPosition, uv, NORMAL_OFFSET, tangent, bitangent);
    // gl_Position = vec4(scaledNDC.x,scaledNDC.y,0.0, 1.0);  //isn't this camera space?
    gl_Position = projection * view  * vec4(newPosition, 1.0) ;
    
    if (!isFinite(newPosition)) {
      vs_out.Color = vec3(1.0, 0.0, 0.0);
      return;
    }

    vs_out.FragPos = newPosition;
    vs_out.Normal = normalize(normalFD);
    vs_out.Color = vec3(0.10,0.2,0.25);
    vs_out.Bitangent = bitangent;
    vs_out.Tangent = tangent;
    vs_out.FragUV = uv;
}
