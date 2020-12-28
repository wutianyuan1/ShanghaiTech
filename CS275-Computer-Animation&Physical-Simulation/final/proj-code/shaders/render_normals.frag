#version 330 core

in VertOut {
    vec2 texcoords;
} i;

uniform sampler2D g_normals;

out vec4 FragColor;

void main() {
    vec3 normal = normalize(texture(g_normals, i.texcoords).xyz);
    vec3 normal_vis = (normal + vec3(1.0, 1.0, 1.0)) / 2.0;
    FragColor = vec4(normal_vis, 1.0);
}