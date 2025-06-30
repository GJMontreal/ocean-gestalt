layout(location = 0) in vec2 ndcCoord;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};
             
out vec3 Color;

const float EPSILON = 1e-5;

uniform vec3 cameraPos;
uniform float fovYRadians;
uniform float aspect;

uniform float planeY = 0.0;
uniform float ndcScale;
const vec2 clipMin = vec2(-60.0,-60.0); // e.g., vec2(-10.0, -5.0)
const vec2 clipMax = vec2(60.0,60.0); // e.g., vec2(10.0, 5.0)


vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float t = (yPlane - origin.y) / dir.y;
    return origin + dir * t;
}

void main() {
    // Step 1: Build ray direction in view space
    float tanHalfFovY = tan(0.5 * fovYRadians);

    vec2 scaledNDC = ndcCoord * ndcScale;

    vec3 rayViewDir = normalize(vec3(
        scaledNDC.x * aspect * tanHalfFovY,
        scaledNDC.y * tanHalfFovY,
        -1.0 // into screen
    ));

    // Step 2: Rotate to world space using view matrix inverse
    mat3 viewRot = mat3(transpose(view)); // inverse of rotation-only view
    vec3 rayWorldDir = viewRot * rayViewDir;

    // Step 3: Raycast to ground
    vec3 hit = intersectRayWithPlane(cameraPos, rayWorldDir, planeY);

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
