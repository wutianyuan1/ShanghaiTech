#version 330 core
layout (location = 0) in vec2 v_position;
layout (location = 1) in vec2 v_texcoords;

out VertOut {
    vec2 texcoords;
} o;

void main() {
    gl_Position = vec4(v_position, 0.0, 1.0);
    o.texcoords = v_texcoords;
}