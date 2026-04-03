in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 Color;
} fs_in;

in vec2 TexCoord;

layout(std140) uniform Matrices
{
    uniform mat4 projection;
    uniform mat4 view;
};

uniform sampler2D colorMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;
uniform int isReflectionPass;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

out vec4 FragColor;

float shadowFactor(vec4 fragPosLightSpace) {
    vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj = proj * 0.5 + 0.5;
    if (proj.z > 1.0) return 0.0;
    float closestDepth = texture(shadowMap, proj.xy).r;
    float currentDepth = proj.z;
    return currentDepth - 0.005 > closestDepth ? 0.5 : 0.0;
}

void main() {
    if (isReflectionPass == 1 && fs_in.FragPos.y < 0.0) discard;

    vec3 albedo   = texture(colorMap, TexCoord).rgb;
    vec3 norm     = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff    = max(dot(norm, lightDir), 0.0);

    vec3 viewDir    = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec4 fragPosLight = lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);
    float shadow = shadowFactor(fragPosLight);

    vec3 ambient  = 0.2 * albedo;
    vec3 diffuse  = (1.0 - shadow) * diff * albedo;
    vec3 specular = (1.0 - shadow) * spec * vec3(0.3);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
