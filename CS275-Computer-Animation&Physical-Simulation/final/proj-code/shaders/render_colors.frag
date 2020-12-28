#version 330 core

in VertOut {
    vec2 texcoords;
} i;

uniform sampler2D g_colors;

out vec4 FragColor;

void main() {
    vec4 color_weighted = texture(g_colors, i.texcoords);
    vec3 color = color_weighted.rgb / color_weighted.a;
    FragColor = vec4(color, 1.0);
}