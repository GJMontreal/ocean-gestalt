layout(location = 0) in vec2 aCorner;
layout(location = 1) in vec3 aWorldPos;
layout(location = 2) in float aLifetimeFrac;
layout(location = 3) in float aSize;
layout(location = 4) in vec3 aVelocityDir;
layout(location = 5) in float aSeed;

uniform mat4 projection;
uniform mat4 view;

out float vLifetimeFrac;
out vec2  vUV;
out float vSeed;

void main() {
    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp    = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 worldPos = aWorldPos
        + camRight * aCorner.x * aSize
        + camUp    * aCorner.y * aSize;

    gl_Position   = projection * view * vec4(worldPos, 1.0);
    vLifetimeFrac = aLifetimeFrac;
    vUV           = aCorner * 0.5 + 0.5;
    vSeed         = aSeed;
}
