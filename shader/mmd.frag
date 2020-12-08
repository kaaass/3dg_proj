#version 330 core

#define NR_POINT_LIGHTS 10

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    float     shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform int pointCount;
uniform bool disableTexture;
uniform sampler2D shadowMap;

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main() {
    // 属性
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 第一阶段：定向光照
    vec4 result = CalcDirLight(dirLight, norm, viewDir);

    FragColor = result;
}

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // 合并结果
    vec4 tex;
    if (disableTexture) {
        tex = vec4(1, 1, 1, 1);
    } else {
        tex = texture(material.texture_diffuse1, TexCoords);
    }
    vec4 ambient  = vec4(1.0) * tex;
    // 计算阴影
    float y = dirLight.direction[1];
    float l = 1.0f;
    l = -y * 4.0f;
    if (l > 1.0f)
        l = 1.0f;
    if (l < 0.1f)
        l = 0.1f;
    vec4 lighting = (1.0f - (1.0f - l) * 0.5f) * ambient;
    return vec4(vec3(lighting), ambient[3]);
}
