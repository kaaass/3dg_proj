#version 330 core
in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox;
uniform float light;

void main() {    
    FragColor = vec4(vec3(texture(skybox, TexCoords)) * light, 1);
}