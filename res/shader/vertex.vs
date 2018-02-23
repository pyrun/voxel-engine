#version 400

layout( location = 0 ) in vec4 positions;

uniform mat4 g_mvp;
uniform vec2 g_size;
uniform vec4 g_backgroundcolor;

void main(void) {
    gl_Position = g_mvp * vec4( positions.xyz, 1.0);
}
