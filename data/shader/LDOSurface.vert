layout(location = 0) in vec2 ndcCoord;

layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 invViewProjection;

const float planeY = 0.0;

vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float t = (yPlane - origin.y) / dir.y;
    return origin + dir * t;
}

void main() {
    vec4 clipNear = vec4(ndcCoord, -1.0, 1.0);
    vec4 clipFar  = vec4(ndcCoord,  1.0, 1.0);

    vec3 worldNear = (invViewProjection * clipNear).xyz;
    vec3 worldFar  = (invViewProjection * clipFar).xyz;

    vec3 rayOrigin = worldNear;
    vec3 rayDir    = normalize(worldFar - worldNear);

    vec3 surfacePos = intersectRayWithPlane(rayOrigin, rayDir, planeY);

    gl_Position = projection * view * vec4(surfacePos, 1.0);
}
