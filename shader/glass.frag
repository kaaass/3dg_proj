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
uniform vec3 cameraPos;
uniform samplerCube skybox;

vec4 GetTex();

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir);

void main() {
    // 属性
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 第一阶段：定向光照
    vec4 result = CalcDirLight(dirLight, norm, viewDir);
    // 第二阶段：点光源
    for (int i = 0; i < pointCount; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    // 第三阶段：聚光
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = result;
}

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // 镜面光着色
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // 合并结果
    vec4 tex = GetTex();
    vec4 ambient  = vec4(1.0) * tex;
    vec4 specular = vec4(light.specular, 1.0) * spec;
    // 计算阴影
    float shadow = ShadowCalculation(FragPosLightSpace, lightDir);
    vec4 lighting = (ambient + (1.0 - shadow) * (specular)) * tex;// vec4(shadow, shadow, shadow, 1.0);
    return lighting;
}

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // 合并结果
    vec4 tex = GetTex();
    vec4 ambient  = vec4(light.ambient, 1.0) * tex;
    vec4 diffuse  = vec4(light.diffuse, 1.0)  * diff * tex;
    vec4 specular = vec4(light.specular, 1.0) * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec4 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec4 tex = GetTex();

    // Ambient
    vec4 ambient = vec4(light.ambient, 1.0) * tex;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * tex;

    // Specular Highlight
    float specularStrength = 0.5;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec4 specular = vec4(light.specular, 1.0) * spec;

    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    // Clamp
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    return (ambient + diffuse + specular);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir) {
    if (lightDir[1] < 0)// 夜晚
    return 1.0;
    if (FragPos[2] < -10)// 超界
    return 0;
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
    shadow = 0.0;
    return shadow;
}

vec4 GetTex() {
    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec4 mirror = vec4(texture(skybox, R).rgb, 1.0);
    float ratio = 1.00 / 1.52;
    I = normalize(FragPos - cameraPos);
    R = refract(I, normalize(Normal), ratio);
    return vec4(texture(skybox, R).rgb, 1.0) * 0.7 + mirror * 0.3;
}