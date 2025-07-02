layout(location = 0) in vec2 ndcCoord;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform vec3 cameraPos;
uniform float fovYRadians;
uniform float aspect;

const float ndcScale = 1.0;
const float planeY = 1.0;

out vec3 Color;

// something very odd is going on in here
vec3 intersectRayWithPlane(vec3 origin, vec3 dir, float yPlane) {
    float denom = dir.y;
    if (abs(denom) < 1e-5)
      return origin + dir * 1e6; // send it far, but not NaN

    float t = (yPlane - origin.y) / denom;
    t = clamp(t, -100.0, 100.0); // or whatever range your scene uses
    return origin + dir * t;
}

bool isFinite(vec3 v) {
    return !any(isnan(v)) && !any(isinf(v));
}


void main() {
    float tanHalfFovY = tan(0.5 * fovYRadians);
    // vec2 uv = vec2(ndcScale) * ndcCoord; // assumed in [-1, 1]
    vec2 uv = ndcCoord;
    vec3 rayView = vec3(
        uv.x * aspect * tanHalfFovY,
        uv.y * tanHalfFovY,
        -1.0
    );

    mat3 viewRot = inverse(mat3(view));
    vec3 rayOrigin = cameraPos + (viewRot * rayView);
    vec3 rayDir = vec3(0.263223, -0.498269, 0.826100);
 
    vec3 hit = intersectRayWithPlane(rayOrigin, rayDir, planeY);

 
    if (gl_VertexID % 2 == 0) {
      gl_Position = projection * view * vec4(rayOrigin,1.0); // equivalent to vec4(ncdCoord, 0.0, 1.0 )
       Color = vec3(clamp(hit, 0.0, 1.0));
    } else {
        gl_Position = projection * view * vec4(hit, 1.0);
        Color = vec3(0.0, 1.0, 0.0);  
    }
}
