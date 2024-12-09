#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.2;

uniform mat4 projection;
uniform mat4 view;

void GenerateLine(int start, int stop )
{
    gl_Position = projection * view * gl_in[start].gl_Position;
    EmitVertex();
    gl_Position = projection * view * gl_in[stop].gl_Position;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    //very simply, don't draw the longest vector
    vec4 vector_0 = gl_in[0].gl_Position - gl_in[1].gl_Position;
    vec4 vector_1 = gl_in[1].gl_Position - gl_in[2].gl_Position;
    vec4 vector_2 = gl_in[2].gl_Position - gl_in[0].gl_Position;

    float distance_0 = dot(vector_0, vector_0);
    float distance_1 = dot(vector_1, vector_1);
    float distance_2 = dot(vector_2, vector_2);

    if( distance_0 < distance_1 ){
        GenerateLine(0,1);
        if( distance_1 < distance_2){
          GenerateLine(1,2);
        }else{
          GenerateLine(2,0);
        }
    }else{
        if( distance_1 < distance_2){
          GenerateLine(1,2);
          if( distance_0 < distance_2){
            GenerateLine(0,1);
          }else{
            GenerateLine(2,0);
          }
        }else{
          if( distance_2 < distance_0){
            GenerateLine(2,0);
            if(distance_1 < distance_2){
              GenerateLine(1,2);
            }else{
              GenerateLine(1,0);
            }
          }
        }
      }
  }
