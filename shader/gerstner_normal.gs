layout (lines) in;
layout (line_strip, max_vertices=2) out;

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

const float MAGNITUDE = 10;

layout(std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void GenerateLine(int index)
{
    // gl_Position = projection * vec4(gs_in[index].FragPos,0.0);
    gl_Position = projection * vec4(gs_in[index].FragPos,1);
    EmitVertex();
    gl_Position = projection * (vec4(gs_in[index].FragPos,1) + vec4(gs_in[index].Normal,1)) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main(){
  GenerateLine(0);
  GenerateLine(1);
}
