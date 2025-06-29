layout(location = 0) in vec2 ndcCoord;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};
             
uniform mat4 invViewProjection;
uniform float ndcScale;
uniform vec3 patchOffset;

out vec3 Color;

const float planeY = 0.0;
// Warps the vertical sampling density to cluster vertices near bottom of screen
vec2 warpNDC(vec2 uv) {
    vec2 norm = (uv + 1.0) * 0.5;
    norm = pow(norm, vec2(2.0)); // bias both x and y
    return norm * 2.0 - 1.0;
}


// Ray/plane intersection, assuming plane normal is +Y
vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float t = (yPlane - origin.y) / dir.y;
    return origin + dir * t;
}

void main() {
    // Step 1: Apply NDC warp and scale
    vec2 warpedCoord = warpNDC(ndcCoord) * ndcScale;

    // Step 2: Construct clip-space ray endpoints
    vec4 clipNear = vec4(warpedCoord, -1.0, 1.0);
    vec4 clipFar  = vec4(warpedCoord,  1.0, 1.0);

    // Step 3: Project into world space
    vec3 worldNear = (invViewProjection * clipNear).xyz;
    vec3 worldFar  = (invViewProjection * clipFar).xyz;

    // Step 4: Intersect ray with Y=0 plane
    vec3 rayDir = normalize(worldFar - worldNear);
    vec3 surfacePos = intersectRayWithPlane(worldNear, rayDir, planeY);

    // Step 5: Offset the entire patch to a fixed anchor point
    surfacePos += patchOffset;

    // Step 6: Transform to clip space
    gl_Position = projection * view * vec4(surfacePos, 1.0);
    // gl_Position = projection * view *vec4(warpedCoord.x,0,warpedCoord.y,1.0);
    Color = vec3(1.0);
}
