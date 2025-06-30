layout(location = 0) in vec2 ndcCoord;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};
             

uniform mat4 invViewProjection;

uniform float ndcScale;     // how far to extend beyond the screen bounds
uniform float planeY;       // height of the target plane (e.g. 0.0)

out vec3 Color;

const float EPSILON = 1e-5;

vec2 warpNDC(vec2 uv) {
    // Optional: denser near bottom of screen
    float y = pow((uv.y + 1.0) * 0.5, 2.0) * 2.0 - 1.0;
    return vec2(uv.x, y);
}

vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float t = (yPlane - origin.y) / dir.y;
    return origin + t * dir;
}

void main() {
    // Step 1: Apply optional warp and scale
    vec2 warpedNDC = warpNDC(ndcCoord) * ndcScale;
    // vec2 warpedNDC = ndcCoord * ndcScale;

    // Step 2: Compute clip-space endpoints
    vec4 clipNear = vec4(warpedNDC, -1.0, 1.0);
    vec4 clipFar  = vec4(warpedNDC,  1.0, 1.0);

    // Step 3: Unproject to world space
    vec3 worldNear = (invViewProjection * clipNear).xyz;
    vec3 worldFar  = (invViewProjection * clipFar).xyz;

    // Step 4: Ray direction
    vec3 rayDir = normalize(worldFar - worldNear);

    // Step 5: Guard against near-horizontal rays
    if (abs(rayDir.y) < EPSILON) {
        gl_Position = vec4(0.0); // Discard
        return;
    }

    // Step 6: Intersect with ground plane
    vec3 surfacePos = intersectRayWithPlane(worldNear, rayDir, planeY);

    // Step 7: Project to screen
    gl_Position = projection * view * vec4(surfacePos, 1.0);
    Color = vec3(0.0);
if (abs(ndcCoord.x) < 0.01 && abs(ndcCoord.y) < 0.01){
  Color = vec3(1.0,0.0,0.0);
}
}
