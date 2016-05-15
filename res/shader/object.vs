#version 400

layout( location = 0 ) in vec3 coord;
layout( location = 1 ) in vec2 data;

uniform mat4 g_mvp;
uniform vec2 g_size;
uniform vec4 g_backgroundcolor;
uniform float g_alpha_cutoff;
uniform vec4 g_sun;

//out vec2 UV;
out vec2 texcoords;

void main(void) {

//    UV = data;
    texcoords = data;
    gl_Position = g_mvp * vec4( coord.xyz, 1.0);
}
