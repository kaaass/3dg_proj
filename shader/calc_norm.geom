#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 iFragPos[3];
in vec2 iTexCoords[3];

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

vec3 GetNormal() {
    vec3 a = iFragPos[0] - iFragPos[1];
    vec3 b = iFragPos[2] - iFragPos[1];
    return normalize(cross(a, b));
}

void main() {
    vec3 normal = -GetNormal();
    for (int i = 0; i < gl_in.length(); i++) {
        gl_Position = gl_in[i].gl_Position;
        FragPos = iFragPos[i];
        Normal = normal;
        TexCoords = iTexCoords[i];
        EmitVertex();
    }
    EndPrimitive();
}