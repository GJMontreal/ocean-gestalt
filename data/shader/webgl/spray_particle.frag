in float vLifetimeFrac;
in vec2  vUV;
in float vSeed;
in vec3  vWorldPos;

out vec4 FragColor;

void main() {
    // Soft circular sprite — the spray effect comes from many of these
    // accumulating additively, not from per-particle shape complexity.
    float r = length(vUV);
    if (r > 1.0) discard;

    // Smooth radial falloff: opaque core, soft edge
    float softness = pow(1.0 - smoothstep(0.0, 1.0, r), 1.5);

    // Per-particle brightness jitter so they don't all look identical
    float jitter = fract(sin(vSeed * 127.1 + 43.7) * 43758.5453);

    float birthFade = 1.0 - smoothstep(0.97, 1.0, vLifetimeFrac);
    float deathFade = smoothstep(0.0, 0.15, vLifetimeFrac);
    // Keep alpha low — accumulation builds brightness, not individual opacity
    float ageFade   = mix(0.04, 0.35, vLifetimeFrac) * (0.7 + 0.3 * jitter);

    float alpha = softness * birthFade * deathFade * ageFade;

    // Young = bright white; old = pale blue-grey
    vec3 color = mix(vec3(0.72, 0.88, 1.0), vec3(1.0), vLifetimeFrac * 0.85);

    FragColor = vec4(color, alpha);
}
