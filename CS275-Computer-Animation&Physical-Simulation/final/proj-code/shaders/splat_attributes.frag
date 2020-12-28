#version 330 core

in VertOut {
    float radius;
    vec3 color;
} i;

uniform float epsilon;
uniform vec2 viewport_size;
uniform sampler2D g_depth;

uniform float z_near;
uniform float z_far;

layout(location = 0) out vec4 g_normal;
layout(location = 1) out vec4 g_color;



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
    // render point as a sphere again and accumulate weighted attributes
    // see http://graphics.cs.kuleuven.be/publications/PSIRPBSD/

    vec2 from_center_pt_spc = gl_PointCoord - vec2(0.5, 0.5);
    float mag_sq = dot(from_center_pt_spc, from_center_pt_spc);
    if (mag_sq > 0.25 /* 0.5^2 */) {
        discard;
    }

    vec2 from_center_wld_spc = from_center_pt_spc * 2 * i.radius;
    float depth_offset = sqrt(i.radius * i.radius - dot(from_center_wld_spc, from_center_wld_spc));
    float depth = linearDepth(gl_FragCoord.z) - depth_offset;

    vec2 tex_coords = gl_FragCoord.xy / viewport_size;
    float surface_depth = linearDepth(texture(g_depth, tex_coords).x);
    if (depth > surface_depth) {
        discard;
    }

    float w1 = 1 - (sqrt(mag_sq) / 0.5);
    float w2 = (surface_depth - depth) / epsilon;
    float weight = w1 * w2;

    vec3 normal = normalize(vec3(from_center_wld_spc, depth_offset));
    g_normal.xyz = weight * normal;

    vec3 color = weight * i.color;
    g_color.rgb = color;
    g_color.a = weight;
}