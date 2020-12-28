#version 330 core

in VertOut {
    float radius;
    vec3 color;
} i;

uniform float epsilon;

uniform float z_near;
uniform float z_far;



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
    // render point as a sphere and shift it back by epsilon

    vec2 from_center_pt_spc = gl_PointCoord - vec2(0.5, 0.5);
    float mag_sq = dot(from_center_pt_spc, from_center_pt_spc);
    if (mag_sq > 0.25 /* 0.5^2 */) {
        discard;
    }

    vec2 from_center_wld_spc = from_center_pt_spc * 2 * i.radius;
    float depth_offset = sqrt(i.radius * i.radius - dot(from_center_wld_spc, from_center_wld_spc));
    float depth = depthSample(linearDepth(gl_FragCoord.z) - depth_offset + epsilon);
    gl_FragDepth = depth;
}