#version 330 core

in VertOut {
    vec2 texcoords;
} i;

uniform sampler2D g_depths;

uniform float z_near;
uniform float z_far;

out vec4 FragColor;

float linearDepth(float depthSample)
{
    depthSample = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * z_near * z_far / (z_far + z_near - depthSample * (z_far - z_near));
    return zLinear;
}

float depthSample(float linearDepth)
{
    float nonLinearDepth = (z_far + z_near - 2.0 * z_near * z_far / linearDepth) / (z_far - z_near);
    nonLinearDepth = (nonLinearDepth + 1.0) / 2.0;
    return nonLinearDepth;
}

void main() {
    float depth = linearDepth(texture(g_depths, i.texcoords).x) / z_far;
    FragColor = vec4(depth, depth, depth, 1.0);
}