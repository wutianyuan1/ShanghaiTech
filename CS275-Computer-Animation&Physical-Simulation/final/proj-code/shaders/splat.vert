#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;

uniform mat4 view_proj_matrix;
uniform float fov_y;
uniform vec2 viewport_size;
uniform float particle_radius;

out VertOut {
    float radius;
    vec3 color;
} o;

void main() {
    // project point to canonical view volume and scale radius according to perspective
    gl_Position = view_proj_matrix * vec4(in_position, 1);
    // see https://stackoverflow.com/questions/25780145/gl-pointsize-corresponding-to-world-space-size
    gl_PointSize = viewport_size[1] * (1 / tan(fov_y / 2)) * particle_radius / gl_Position.w;
    o.radius = particle_radius;
    o.color = in_color;
}