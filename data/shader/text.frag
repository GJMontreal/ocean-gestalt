in vec4 color;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D textAtlas;

void main() {
    float mask = texture(textAtlas, texCoord).r;
    fragColor = mask * color;
}
