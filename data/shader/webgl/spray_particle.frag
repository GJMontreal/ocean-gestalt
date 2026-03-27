in float vLifetimeFrac;
in vec2  vUV;
in float vSeed;

out vec4 FragColor;

void main() {
    vec2 c = vUV * 2.0 - 1.0;

    float s1 = fract(vSeed * 7.319);
    float s2 = fract(vSeed * 13.271);
    float s3 = fract(vSeed * 3.617);

    float a = vSeed * 6.2832;
    vec2 rc = vec2(cos(a) * c.x - sin(a) * c.y,
                   sin(a) * c.x + cos(a) * c.y);

    // Young particles are blobby foam chunks; old ones are thin wisps
    float seedAspect = mix(1.5, 4.0, s1);
    float aspect = mix(seedAspect, 1.4, vLifetimeFrac * 0.8);
    float dist2 = rc.x * rc.x * aspect * aspect + rc.y * rc.y;
    if (dist2 > 1.0) discard;

    // Tight hard edge when young (foam), soft diffuse when old (spray)
    float tightness = mix(mix(1.5, 4.5, s3), 6.0, vLifetimeFrac);
    float edgeFade = exp(-dist2 * tightness);

    // Short pop-in, very brief fade-out — foam chunks appear and dissolve
    float birthFade = 1.0 - smoothstep(0.85, 1.0, vLifetimeFrac);
    float deathFade = smoothstep(0.0, 0.12, vLifetimeFrac);

    // Young = opaque foam; old = transparent spray
    float ageFade = mix(0.15, 0.80, vLifetimeFrac);

    float alpha = edgeFade * birthFade * deathFade * ageFade * mix(0.5, 0.9, s2);

    // Young = bright pure white (foam), old = pale blue-grey (fine spray)
    vec3 foamColor = vec3(1.0, 1.0, 1.0);
    vec3 sprayColor = vec3(0.75, 0.88, 1.0);
    vec3 color = mix(sprayColor, foamColor, vLifetimeFrac * 0.85);
    color = mix(color, vec3(1.0), edgeFade * vLifetimeFrac * 0.5);

    FragColor = vec4(color, alpha);
}
