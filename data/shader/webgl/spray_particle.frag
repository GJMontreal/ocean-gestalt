in float vLifetimeFrac;
in vec2  vUV;
in float vSeed;

out vec4 FragColor;

void main() {
    vec2 c = vUV * 2.0 - 1.0;

    // Derive several independent per-particle values from the seed
    float s1 = fract(vSeed * 7.319);   // aspect ratio
    float s2 = fract(vSeed * 13.271);  // brightness
    float s3 = fract(vSeed * 3.617);   // falloff tightness

    // Random rotation
    float a = vSeed * 6.2832;
    vec2 rc = vec2(cos(a) * c.x - sin(a) * c.y,
                   sin(a) * c.x + cos(a) * c.y);

    // Aspect ratio varies from near-circular (1.5:1) to wispy (4:1)
    float aspect = mix(1.5, 4.0, s1);
    float dist2 = rc.x * rc.x * aspect * aspect + rc.y * rc.y;
    if (dist2 > 1.0) discard;

    float fadeIn  = smoothstep(0.0, 0.15, vLifetimeFrac);
    float fadeOut = smoothstep(0.0, 0.55, vLifetimeFrac);

    // Falloff tightness varies — some are hard bright cores, some are diffuse
    float tightness = mix(1.5, 4.5, s3);
    float alpha = exp(-dist2 * tightness) * fadeIn * fadeOut * mix(0.25, 0.55, s2);

    vec3 color = mix(vec3(0.78, 0.91, 1.0), vec3(1.0), exp(-dist2 * 6.0));

    FragColor = vec4(color, alpha);
}
