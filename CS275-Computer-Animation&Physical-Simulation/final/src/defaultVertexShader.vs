#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

out vec3 Normal;
out vec3 FragPos;
out vec3 Color;

uniform mat4 pvm;
uniform mat4 model;

void main()
{
    gl_Position = pvm * vec4(pos, 1.0);
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = normalize(mat3(transpose(inverse(model))) * normal);
    Color = color;
}