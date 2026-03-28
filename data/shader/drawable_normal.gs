layout (lines) in;
layout (line_strip, max_vertices=6) out;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} gs_in[];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} fs_in;

const float MAGNITUDE = .5;

layout(std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
void EmitDebugLine(vec3 origin, vec3 dir, vec3 color) {
    fs_in.FragPos = origin;
    fs_in.Color = color;
    fs_in.Normal = normalize(origin);
    gl_Position = projection * view * vec4(origin, 1.0);
    EmitVertex();

    fs_in.FragPos = origin + dir * MAGNITUDE;
    fs_in.Color = color;
    fs_in.Normal = normalize(origin);
    gl_Position = projection * view * vec4(origin + dir * MAGNITUDE, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main() {
    vec3 origin = gs_in[0].FragPos;
    // EmitDebugLine(origin, normalize(gs_in[0].Tangent), vec3(1, 0, 0));   // red = Tangent
    // EmitDebugLine(origin, normalize(gs_in[0].Bitangent), vec3(0, 1, 0));   // green = Bitangent
    EmitDebugLine(origin, normalize(gs_in[0].Normal),  vec3(1, 1, 0));   // yellow = DebugNormal
}
