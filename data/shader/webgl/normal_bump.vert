in vec3 aPosition;
in vec2 aTexCoord;
in vec3 aTangent;
in vec4 aColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float hullDisplacementAmt;

out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN;

// 3D value noise for hull displacement
float hash3(vec3 p) {
    p = fract(p * vec3(127.1, 311.7, 74.7));
    p += dot(p, p.yzx + 19.19);
    return fract((p.x + p.y) * p.z);
}

float valueNoise3(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(
        mix(mix(hash3(i),               hash3(i + vec3(1,0,0)), u.x),
            mix(hash3(i + vec3(0,1,0)), hash3(i + vec3(1,1,0)), u.x), u.y),
        mix(mix(hash3(i + vec3(0,0,1)), hash3(i + vec3(1,0,1)), u.x),
            mix(hash3(i + vec3(0,1,1)), hash3(i + vec3(1,1,1)), u.x), u.y), u.z);
}

float hullNoise(vec3 p) {
    float n = valueNoise3(p * 2.1) * 0.65
            + valueNoise3(p * 4.3) * 0.35;
    return n * 2.0 - 1.0;
}

void main() {
    vec3 localNormal = normalize(aPosition);

    vec3 up = abs(localNormal.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 localT = normalize(cross(up, localNormal));
    vec3 localB = cross(localNormal, localT);

    float eps = 0.02;
    float d  = hullNoise(aPosition);
    float dT = hullNoise(aPosition + localT * eps);
    float dB = hullNoise(aPosition + localB * eps);

    vec3 displacedPos = aPosition + localNormal * d * hullDisplacementAmt;

    vec3 dpT = localT * eps + localNormal * (dT - d) * hullDisplacementAmt;
    vec3 dpB = localB * eps + localNormal * (dB - d) * hullDisplacementAmt;
    vec3 perturbedLocalNormal = normalize(cross(dpT, dpB));

    vec3 N = normalize(mat3(model) * perturbedLocalNormal);
    vec3 T = normalize(mat3(model) * normalize(aTangent));
    vec3 B = cross(N, T);

    TBN     = mat3(T, B, N);
    FragPos = vec3(model * vec4(displacedPos, 1.0));
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
