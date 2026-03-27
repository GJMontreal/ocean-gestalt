in float vLifetimeFrac;
in vec2  vUV;
in float vSeed;

out vec4 FragColor;

void main() {
    vec2 c = vUV * 2.0 - 1.0;

    // Four independent values from the seed
    float s1 = fract(vSeed * 7.319);
    float s2 = fract(vSeed * 13.271);
    float s3 = fract(vSeed * 3.617);
    float s4 = fract(vSeed * 19.431);

    // Random rotation
    float a = vSeed * 6.2832;
    vec2 rc = vec2(cos(a)*c.x - sin(a)*c.y,
                   sin(a)*c.x + cos(a)*c.y);

    // Sub-blob positions start near centre (foam chunk) and
    // drift apart with age (dispersion into smaller wisps)
    float spread = mix(0.05, 0.55, 1.0 - vLifetimeFrac);

    vec2 p1 = rc + vec2(s1 - 0.5, s2 - 0.5) * spread;
    vec2 p2 = rc - vec2(s3 - 0.5, s4 - 0.5) * spread * 0.8;
    vec2 p3 = rc + vec2(s2 - 0.5, s3 - 0.5) * spread * 0.6;

    // Sub-blob sharpness: tight and solid when young, diffuse when old
    float tightness = mix(2.0, 6.0, vLifetimeFrac);
    float b1 = exp(-dot(p1, p1) * tightness);
    float b2 = exp(-dot(p2, p2) * tightness * 0.75) * mix(0.5, 0.9, s1);
    float b3 = exp(-dot(p3, p3) * tightness * 0.55) * mix(0.3, 0.7, s3);

    float shape = b1 + b2 + b3;
    if (shape < 0.02) discard;

    float birthFade = 1.0 - smoothstep(0.85, 1.0, vLifetimeFrac);
    float deathFade = smoothstep(0.0, 0.12, vLifetimeFrac);
    // Young = opaque foam; old = transparent spray
    float ageFade   = mix(0.12, 0.85, vLifetimeFrac);

    float alpha = min(shape, 1.0) * birthFade * deathFade * ageFade;

    // Colour: bright white foam → pale blue-grey spray
    vec3 color = mix(vec3(0.72, 0.87, 1.0), vec3(1.0),
                     vLifetimeFrac * 0.85 * b1);

    FragColor = vec4(color, alpha);
}
