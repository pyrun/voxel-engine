#version 400

layout( location = 0 ) in vec4 coord;

uniform mat4 g_mvp;

void main(void) {
    gl_Position = g_mvp * vec4( coord.xyz, 1.0);
}
