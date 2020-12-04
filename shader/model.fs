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

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform int pointCount;

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    // 属性
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 第一阶段：定向光照
    vec4 result = CalcDirLight(dirLight, norm, viewDir);
    // // 第二阶段：点光源
    // for (int i = 0; i < pointCount; i++) {
    //     result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // }
    // // 第三阶段：聚光
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = result;
}

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    vec4 ambient  = vec4(light.ambient, 1.0) * texture(material.texture_diffuse1, TexCoords);
    vec4 diffuse  = vec4(light.diffuse, 1.0)  * diff * texture(material.texture_diffuse1, TexCoords);
    vec4 specular = vec4(light.specular, 1.0) * spec * texture(material.texture_specular1, TexCoords);
    return ambient; // (ambient + diffuse + specular);
}

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // 合并结果
    vec4 ambient  = vec4(light.ambient, 1.0) * texture(material.texture_diffuse1, TexCoords);
    vec4 diffuse  = vec4(light.diffuse, 1.0)  * diff * texture(material.texture_diffuse1, TexCoords);
    vec4 specular = vec4(light.specular, 1.0) * spec * texture(material.texture_specular1, TexCoords);
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return ambient; // (ambient + diffuse + specular);
}

vec4 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Ambient
    vec4 ambient = vec4(light.ambient, 1.0) * texture(material.texture_diffuse1, TexCoords);

    // Diffuse    
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = vec4(light.diffuse, 1.0) * diff * texture(material.texture_diffuse1, TexCoords);

    // Specular Highlight
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = vec4(light.specular, 1.0) * spec * texture(material.texture_specular1, TexCoords);

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

   return ambient; // (ambient + diffuse + specular);
}