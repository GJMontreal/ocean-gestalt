layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} gs_in[];

const float MAGNITUDE = 10;

layout(std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void GenerateLine(int index)
{
    // gl_Position = projection * vec4(gs_in[index].FragPos,0.0);
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    // gl_Position = projection * (vec4(gs_in[index].FragPos,0.0) + vec4(gs_in[index].Normal, 0.0) * MAGNITUDE);
    // gl_Position = projection * vec4(1,1,1,0);
    gl_Position = projection * (gl_in[index].gl_Position + vec4(1,1,1,0)) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}
