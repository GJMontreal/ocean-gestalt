in vec3 Position;
in vec3 Normal;
in vec4 Color;

uniform mat4 projection;
uniform mat4 view;

uniform mat4 model;

uniform float time;

out vec3 oFragPos;
out vec3 oNormal;
out vec3 oColor;

void main() {

  oFragPos = vec3(model * vec4(Position, 1.0));
  oNormal = mat3(transpose(inverse(model))) * Normal;
  oColor = vec3(Color.xyz);

  gl_Position = projection * view * model * vec4(Position, 1.0);
}
