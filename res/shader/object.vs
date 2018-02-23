#version 410

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormals;
layout(location=2) in vec2 vertexColor;

uniform mat4 g_mvp;
uniform vec2 g_size;
uniform vec4 g_backgroundcolor;
uniform float g_alpha_cutoff;
uniform vec4 g_sun;

out vec3 normal;
out vec2 color;

void main() {
//    UV = data;
    color = vertexColor;
    normal = vertexNormals;
    gl_Position = g_mvp * vec4( vertexPosition, 1.0);
}
