in vec3 TexDir;
out vec4 FragColor;

uniform samplerCube envMap;

void main() {
    // FragColor = texture(envMap, normalize(TexDir));
    vec3 dir = normalize(TexDir);
vec4 sampleColor = texture(envMap, dir);

// Highlight which cube face this sample hits
vec3 absDir = abs(dir);
float maxAxis = max(max(absDir.x, absDir.y), absDir.z);

vec3 faceColor;
if (absDir.x == maxAxis) {
    faceColor = dir.x > 0.0 ? vec3(1,0,0) : vec3(0.5,0,0); // +X / -X
} else if (absDir.y == maxAxis) {
    faceColor = dir.y > 0.0 ? vec3(0,1,0) : vec3(0,0.5,0); // +Y / -Y
} else {
    faceColor = dir.z > 0.0 ? vec3(0,0,1) : vec3(0,0,0.5); // +Z / -Z
}

FragColor = mix(sampleColor, vec4(faceColor, 1.0), 0.5);
FragColor = texture(envMap, vec3(TexDir.x, TexDir.y, TexDir.z));
// FragColor = texture(envMap, vec3(-TexDir.x, TexDir.y, TexDir.z));
}
