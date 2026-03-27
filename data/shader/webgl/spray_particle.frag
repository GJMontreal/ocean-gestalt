in float vLifetimeFrac;
in vec2  vUV;
in float vSeed;

out vec4 FragColor;

// Rotate a 2D vector by angle a
vec2 rot(vec2 v, float a) {
    return vec2(cos(a)*v.x - sin(a)*v.y, sin(a)*v.x + cos(a)*v.y);
}

// Elongated gaussian blob: aspect > 1 stretches along local x
float blob(vec2 p, float aspect, float tightness) {
    return exp(-(p.x*p.x*aspect*aspect + p.y*p.y) * tightness);
}

void main() {
    vec2 c = vUV * 2.0 - 1.0;

    float s1 = fract(vSeed * 7.319);
    float s2 = fract(vSeed * 13.271);
    float s3 = fract(vSeed * 3.617);
    float s4 = fract(vSeed * 19.431);
    float s5 = fract(vSeed * 5.111);

    // Overall rotation
    vec2 rc = rot(c, vSeed * 6.2832);

    float spread = mix(0.05, 0.6, 1.0 - vLifetimeFrac);
    float tight  = mix(1.8, 5.5, vLifetimeFrac);

    // Main blob: large, moderately elongated
    vec2 p1  = rc + vec2(s1 - 0.5, s2 - 0.5) * spread * 0.3;
    float b1 = blob(rot(p1, s1 * 3.14), mix(1.2, 2.5, s3), tight);

    // Tendril 1: narrow and long, rotated differently
    vec2 p2  = rc + vec2(s2 - 0.5, s3 - 0.5) * spread;
    float b2 = blob(rot(p2, s2 * 6.28), mix(2.5, 5.0, s4), tight * 0.7) * mix(0.4, 0.85, s1);

    // Tendril 2: smaller, different angle
    vec2 p3  = rc - vec2(s4 - 0.5, s5 - 0.5) * spread * 0.8;
    float b3 = blob(rot(p3, s5 * 4.71), mix(1.8, 4.0, s2), tight * 0.55) * mix(0.2, 0.6, s3);

    float shape = b1 + b2 + b3;
    if (shape < 0.02) discard;

    float birthFade = 1.0 - smoothstep(0.85, 1.0, vLifetimeFrac);
    float deathFade = smoothstep(0.0, 0.12, vLifetimeFrac);
    float ageFade   = mix(0.10, 0.70, vLifetimeFrac);

    float alpha = min(shape, 1.0) * birthFade * deathFade * ageFade;

    vec3 color = mix(vec3(0.70, 0.86, 1.0), vec3(1.0), vLifetimeFrac * b1 * 0.9);

    FragColor = vec4(color, alpha);
}
