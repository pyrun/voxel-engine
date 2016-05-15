#version 400

layout( location = 0 ) in vec4 coord;
layout( location = 2 ) in vec4 data;

uniform mat4 g_mvp;
uniform vec2 g_size;
uniform vec4 g_backgroundcolor;
uniform float g_alpha_cutoff;
uniform vec4 g_sun;

out vec4 texcoord;
out vec4 blockdata;

out vec2 size;
out vec4 backgroundcolor;
out float alpha_cutoff;

out vec4 o_sun;

void main(void) {
    texcoord = coord;
    blockdata = data;

    size = g_size;
    backgroundcolor = g_backgroundcolor;
    alpha_cutoff = g_alpha_cutoff;

    o_sun =  g_sun;

    gl_Position = g_mvp * vec4( coord.xyz, 1.0);
}
