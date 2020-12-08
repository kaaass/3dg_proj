#version 330 core
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 color;
uniform sampler2D texture;

void main() {
    float a = texture2D(texture, TexCoords).r;
    FragColor = vec4(color.rgb, a);
}