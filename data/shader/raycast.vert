layout(location = 0) in vec2 ndcCoord;

layout(std140) uniform Matrices {
    uniform mat4 projection;
    uniform mat4 view;
};

uniform vec3 cameraPos;
uniform float fovYRadians;
uniform float aspect;
uniform float ndcScale;
const float planeY = 0.0;

out vec3 Color;

vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float safeY = sign(dir.y) * max(abs(dir.y), 1e-5);
    float t = (yPlane - origin.y) / safeY;
    return origin + dir * t;
}

void main() {
    vec2 scaledNDC = ndcCoord * ndcScale;
    float tanHalfFovY = tan(0.5 * fovYRadians);
    vec2 uv = ndcCoord; // assumed in [-1, 1]

    vec3 rayView = vec3(
        uv.x * aspect * tanHalfFovY,
        uv.y * tanHalfFovY,
        -1.0
    );

    mat3 viewRot = inverse(mat3(view));
    vec3 rayOrigin = cameraPos + (viewRot * rayView);
    vec3 rayDir = normalize(viewRot * rayView);

    vec3 hit = intersectRayWithPlane(rayOrigin, rayDir, planeY);

    vec4 projectedOrigin = projection * view * vec4(rayOrigin, 1.0);
    if (gl_VertexID % 2 == 0) {
        // gl_Position = projectedOrigin; // purely in clip space
        gl_Position = vec4(ndcCoord,0.0,1.0);
         Color = vec3(0.0, 1.0, 0.0);
    } else {
        gl_Position = projection * view * vec4(10.0,0.0,10.0,1.0); // world-space projected point
        Color = vec3(1.0, 0.0, 0.0);
    }

}


        // gl_Position = projection * view * vec4(ndcCoord.x,0.0,ndcCoord.y,1.0);