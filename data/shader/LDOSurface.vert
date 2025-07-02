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
const vec2 clipMin = vec2(-60.0,-60.0); // e.g., vec2(-10.0, -5.0)
const vec2 clipMax = vec2(60.0,60.0); // e.g., vec2(10.0, 5.0)


vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float t = (yPlane - origin.y) / dir.y;
    return origin + dir * t;
}

void main() {
    // Step 1: Build ray direction in view space
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

    vec3 hit = intersectRayWithPlane(rayOrigin, rayDir, planeY);

 // Step 4: Apply world-space rectangle clipping
    vec2 posXZ = hit.xz;

    if (any(lessThan(posXZ, clipMin)) || any(greaterThan(posXZ, clipMax))) {
        gl_Position = vec4(0.0/0.0); // OpenGl says we need NaN in order to cull
        return;
    }

    // Step 4: Project to screen
    gl_Position = projection * view * vec4(hit, 1.0);
    Color = vec3(1.0);
}
