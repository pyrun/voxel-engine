#version 440

layout( location = 0 ) in vec4 coord;
layout( location = 1 ) in vec4 normal;
layout( location = 2 ) in vec4 data;

uniform mat4 g_mvp;
uniform vec2 g_size;
uniform mat4 g_shadowmvp;

void main(void) {
    gl_Position = g_mvp * vec4( coord.xyz, 1.0);
}
