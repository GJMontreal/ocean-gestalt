layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform mat4 model;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} vs_out;

out vec2 TexCoord;

void main() {
    vs_out.FragPos  = vec3(model * vec4(aPosition, 1.0));
    vs_out.Normal   = normalize(mat3(transpose(inverse(model))) * aNormal);
    vs_out.Color    = vec3(1.0);
    TexCoord        = aTexCoord;

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
    gl_ClipDistance[0] = vs_out.FragPos.y;
}
