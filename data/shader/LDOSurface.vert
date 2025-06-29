layout(location = 0) in vec2 ndcCoord;

uniform mat4 invViewProjection;

layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

const float planeY = 0.0;

uniform float ndcScale;

vec2 warpCoords(vec2 uv) {
    // bias more vertices near bottom (closer to camera)
    float yWarp = pow((uv.y + 1.0) / 2.0, 2.0) * 2.0 - 1.0;
    return vec2(uv.x, yWarp);
}

vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float t = (yPlane - origin.y) / dir.y;
    return origin + dir * t;
}

void main() {
    vec2 warpedCoord = warpCoords(ndcCoord) * ndcScale;

    vec4 clipNear = vec4(warpedCoord, -1.0, 1.0);
    vec4 clipFar  = vec4(warpedCoord,  1.0, 1.0);

    vec3 worldNear = (invViewProjection * clipNear).xyz;
    vec3 worldFar  = (invViewProjection * clipFar).xyz;

    vec3 rayOrigin = worldNear;
    vec3 rayDir = normalize(worldFar - worldNear);

    vec3 worldPos = intersectRayWithPlane(rayOrigin, rayDir, planeY);

    gl_Position = projection * view * vec4(worldPos, 1.0);
}
