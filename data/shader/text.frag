in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D textAtlas;
uniform vec3 textColor;

void main() {
    float alpha = texture(textAtlas, TexCoords).r;
    FragColor = vec4(textColor, alpha);
}
