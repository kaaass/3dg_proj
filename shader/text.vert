#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 tex_coord;

out vec2 TexCoords;

uniform vec3 offset;

void main() {
    TexCoords = tex_coord.xy;
    gl_Position = vec4(vertex + offset, 1.0);
}