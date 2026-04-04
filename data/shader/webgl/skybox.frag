precision mediump float;

in vec3 TexDir;
out vec4 FragColor;

uniform samplerCube envMap;

void main() {
    FragColor = texture(envMap, normalize(TexDir));
}
