in float vLifetimeFrac;
in vec2  vUV;
in float vSeed;

out vec4 FragColor;

void main() {
    vec2 c = vUV * 2.0 - 1.0;

    // Rotate each particle by a random angle so none look the same
    float a = vSeed * 6.2832;
    vec2 rc = vec2(cos(a) * c.x - sin(a) * c.y,
                   sin(a) * c.x + cos(a) * c.y);

    // Elongated 2:1, so they're wisp-like rather than circular
    float dist2 = rc.x * rc.x * 4.0 + rc.y * rc.y;
    if (dist2 > 1.0) discard;

    float fadeIn  = smoothstep(0.0, 0.15, vLifetimeFrac);
    float fadeOut = smoothstep(0.0, 0.55, vLifetimeFrac);

    // Soft falloff — bright core, feathered edge, low per-particle alpha
    // (additive blending accumulates them into a bright mass at crests)
    float alpha = exp(-dist2 * 2.5) * fadeIn * fadeOut * 0.35;

    vec3 color = mix(vec3(0.80, 0.92, 1.0), vec3(1.0), exp(-dist2 * 6.0));

    FragColor = vec4(color, alpha);
}
