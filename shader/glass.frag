#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec4 mirror = vec4(texture(skybox, R).rgb, 1.0);
    float ratio = 1.00 / 1.52;
    I = normalize(Position - cameraPos);
    R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0) * 0.6 + mirror * 0.4;
}