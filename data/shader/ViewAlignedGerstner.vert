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
uniform vec4 color;

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

const float planeY = 0.0;
uniform float ndcScale = 1.0;

uniform int showMesh = 0;
uniform int showDisplaced = 1;

const vec2 clipMin = vec2(-60.0,-60.0); // e.g., vec2(-10.0, -5.0) these should be passed in 
const vec2 clipMax = vec2(60.0,60.0); // e.g., vec2(10.0, 5.0)
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
// const float speedScale = 3.0;

const float GRAVITY = 9.81; 

// rather than relying on a minslope here, we should limit the camera angle, otherwise all the rays will collapse to a point
vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float minSlope = 1e-5; // just to prevent divide by zero errors
    float safeY = max(abs(dir.y), minSlope) * sign(dir.y);
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
    vec2 xz = -wave.steepness * D *  S * wave.amplitude;

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

bool isFinite(vec3 v) {
    return !any(isnan(v)) && !any(isinf(v));
}
mat3 clampViewRot(mat3 rotation){
  vec3 forward = rotation * vec3(0.0, 0.0, -1.0); // camera forward in world space

  // Ensure projection direction has a minimum vertical component
  float minY = 0.2; // adjust to taste; 0.0 = no clamp, 1.0 = straight down
  float signY = sign(forward.y);
  forward.y = clamp(forward.y, signY * minY, 1.0);

  // Renormalize after clamping
  forward = normalize(forward);

  vec3 worldUp = vec3(0.0, 1.0, 0.0);
  vec3 right = normalize(cross(worldUp, forward));
  vec3 up = cross(forward, right);
  mat3 safeViewRot = mat3(right, up, -forward); // columns
  return safeViewRot;
}

void main(void)
{   
    float tanHalfFovY = tan(0.5 * fovYRadians);

    vec2 projectionUV = ndcCoord * ndcScale;

    vec3 rayView = vec3(
        projectionUV.x * aspect * tanHalfFovY,
        projectionUV.y * tanHalfFovY,
        -1.0
    );

    mat3 viewRot = inverse(mat3(view));
    // viewRot = clampViewRot(viewRot);
    vec3 rayOrigin = cameraPos + (viewRot * rayView);
    vec3 rayDir = normalize(viewRot * rayView);

    vec3 position = intersectRayWithPlane(rayOrigin, rayDir, planeY);

    vec2 windDirection = normalize(gust.direction.xy);
    vec2 uv = uvLocation(position, windDirection);

    vec3 newPosition = calcNewPosition(position);
    float gust = gustDisplacement(uv);
    newPosition.y += gust;
    vec3 tangent;
    vec3 bitangent;

    vec3 normalFD = calcNormal(position, newPosition, uv, NORMAL_OFFSET, tangent, bitangent);

    // Project to screen and optionally hide
    vec4 projectedPosition = projection * view * vec4(newPosition, 1.0);
    vec4 projectedOriginal = projection * view * vec4(position, 1.0);
    vec4 offscreen = vec4(2.0, 2.0, 2.0, 1.0);

     // Step 4: Apply world-space rectangle clipping
    vec2 posXZ = newPosition.xz;

    vec2 minMask = step(clipMin, posXZ);     // 1.0 if posXZ ≥ clipMin
    vec2 maxMask = step(posXZ, clipMax);     // 1.0 if posXZ ≤ clipMax
    // float inside = min(min(minMask.x, minMask.y), min(maxMask.x, maxMask.y));  // 1.0 if inside both bounds
    float inside = 1.0;

    vec4 finalPosition = mix(offscreen, projectedPosition, inside);
    finalPosition = mix(projectedOriginal, finalPosition, float(showDisplaced));

    finalPosition = mix(offscreen, finalPosition, float(showMesh));
    gl_Position = finalPosition;

    vs_out.FragPos = newPosition;
    vs_out.Normal = normalize(normalFD);
    vs_out.Color = vec3(color);
    vs_out.Bitangent = bitangent;
    vs_out.Tangent = tangent;
    vs_out.FragUV = uv;
}
