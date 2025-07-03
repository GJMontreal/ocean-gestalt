layout(location = 0) in vec2 ndcCoord;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};
             
out vec3 Color;

uniform vec3 cameraPos;
uniform float fovYRadians;
uniform float aspect;

uniform float planeY = 0.0;
uniform float ndcScale = 1.0; // default values might not work across platforms

uniform int showMesh = 1;

const vec2 clipMin = vec2(-60.0,-60.0); // TODO: these should be passed in 
const vec2 clipMax = vec2(60.0,60.0); // e.g., vec2(10.0, 5.0)


vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float t = (yPlane - origin.y) / dir.y;
    return origin + dir * t;
}

void main() {
// Build ray direction in view space
    float tanHalfFovY = tan(0.5 * fovYRadians);

    vec2 uv = ndcCoord * ndcScale;

    vec3 rayView = vec3(
        uv.x * aspect * tanHalfFovY,
        uv.y * tanHalfFovY,
        -1.0
    );

    mat3 viewRot = inverse(mat3(view));
    vec3 rayOrigin = cameraPos + (viewRot * rayView);
    vec3 rayDir = normalize(viewRot * rayView);

// Find the intersection
    vec3 hit = intersectRayWithPlane(rayOrigin, rayDir, planeY);

// Test against bounds
    vec2 posXZ = hit.xz;

    vec2 minMask = step(clipMin, posXZ);     // 1.0 if posXZ ≥ clipMin
    vec2 maxMask = step(posXZ, clipMax);     // 1.0 if posXZ ≤ clipMax
    float inside = min(min(minMask.x, minMask.y), min(maxMask.x, maxMask.y));  // 1.0 if inside both bounds
    
// Project to screen and optionally hide
    vec4 projectedPosition = projection * view * vec4(hit, 1.0);
    vec4 offscreen = vec4(2.0, 2.0, 2.0, 1.0);
    vec4 finalPosition = mix(offscreen, projectedPosition, inside);

    finalPosition = mix(offscreen, finalPosition, float(showMesh));
    gl_Position = finalPosition;
    Color = vec3(1.0);
}
