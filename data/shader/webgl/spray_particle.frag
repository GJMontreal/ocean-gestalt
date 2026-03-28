in float vLifetimeFrac;
in vec2  vUV;
in float vSeed;
in vec3  vWorldPos;

out vec4 FragColor;

// Shared with water.frag — same foam texture character
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float valueNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p) {
    float sum = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    for (int i = 0; i < 5; ++i) {
        sum += amp * valueNoise(p * freq);
        freq *= 2.0;
        amp  *= 0.5;
    }
    return sum;
}

void main() {
    vec2 c = vUV * 2.0 - 1.0;
    float r = dot(c, c);
    if (r > 1.0) discard;

    // Soft circular billboard boundary
    float edgeMask = 1.0 - smoothstep(0.5, 1.0, sqrt(r));

    // Sample FBM at world XZ so the texture is spatially continuous
    // with the surface foam. Seed offsets each particle to a different
    // patch of the foam field rather than all sampling the same spot.
    vec2 foamUV = vWorldPos.xz * 1.8 + vec2(vSeed * 6.1, vSeed * 3.7);
    float foam = fbm(foamUV);
    foam = pow(foam, 0.7);

    float shape = foam * edgeMask;
    if (shape < 0.04) discard;

    float birthFade = 1.0 - smoothstep(0.85, 1.0, vLifetimeFrac);
    float deathFade = smoothstep(0.0, 0.12, vLifetimeFrac);
    float ageFade   = mix(0.10, 0.80, vLifetimeFrac);

    float alpha = shape * birthFade * deathFade * ageFade;

    // Young = bright white foam; old = pale blue-grey spray
    vec3 color = mix(vec3(0.70, 0.87, 1.0), vec3(1.0),
                     foam * vLifetimeFrac * 0.9);

    FragColor = vec4(color, alpha);
}
