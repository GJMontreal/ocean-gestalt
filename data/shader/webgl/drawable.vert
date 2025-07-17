in vec3 aPosition;
in vec2 aTexCoord;
in vec3 aTangent;
in vec4 aColor;

uniform mat4 projection;
uniform mat4 view;

uniform mat4 model;

uniform float time;

out vec3 oFragPos;
out vec3 oNormal;
out vec3 oColor;

void main() {

  oFragPos = vec3(model * vec4(aPosition, 1.0)); 
  oNormal = mat3((model)) * normalize(aPosition);
  oColor = vec3(1.0);
  gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
