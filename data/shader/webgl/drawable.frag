
in vec3 oFragPos;
in vec3 oNormal;
in vec3 oColor;

uniform mat4 projection;
uniform mat4 view;

// Just to keep our uniforms consistent between shaders
uniform vec3 lightPos;
uniform vec3 viewPos;
  
uniform int isReflectionPass;

// output
out vec4 FragColor;

void main(void)
{
  if (isReflectionPass == 1 && oFragPos.y < 0.0) discard;

vec3 lightDir = normalize(viewPos - oFragPos);
  vec3 norm = normalize(oNormal);

  // fake diffuse shading based on view angle
  float diff = max(dot(norm, lightDir), 0.0);

  // ambient + view-aligned diffuse
  float ambientStrength = 0.2;
  vec3 color = (ambientStrength + diff) * oColor;
  FragColor = vec4(color, 1.0);
  FragColor.rgb += lightPos * 0.0; // To keep our uniforms consistent between shaders
}
