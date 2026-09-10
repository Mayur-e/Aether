#version 330 core

// -----------------------------------------------------------------------
// Fragment shader — Blinn-Phong point-light model
// Supports up to 4 point lights.
// -----------------------------------------------------------------------

in  vec3 fragPos;
in  vec3 fragNormal;
in  vec2 texCoord;

out vec4 FragColor;

// ---- Per-object material ----
uniform vec3  objectColor;
uniform float shininess;

// ---- Camera ----
uniform vec3 viewPos;

// ---- Point lights (max 4) ----
struct PointLight {
    vec3  position;
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_LIGHTS 4
uniform int        numLights;
uniform PointLight lights[MAX_LIGHTS];

// -----------------------------------------------------------------------
vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3  lightDir = normalize(light.position - fragPos);
    float dist     = length(light.position - fragPos);
    float atten    = 1.0 / (light.constant
                          + light.linear    * dist
                          + light.quadratic * dist * dist);

    // Ambient
    vec3 ambient = light.ambient * objectColor;

    // Diffuse
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3  diffuse = light.diffuse * diff * objectColor;

    // Blinn-Phong specular
    vec3  halfway = normalize(lightDir + viewDir);
    float spec    = pow(max(dot(norm, halfway), 0.0), shininess);
    vec3  specular = light.specular * spec;   // white specular highlight

    return (ambient + diffuse + specular) * atten;
}

// -----------------------------------------------------------------------
void main()
{
    vec3 norm    = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0);
    for (int i = 0; i < numLights && i < MAX_LIGHTS; ++i)
        result += calcPointLight(lights[i], norm, fragPos, viewDir);

    // Gamma correction (simple 2.2 approximation)
    result = pow(clamp(result, 0.0, 1.0), vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}
