#version 440

layout( location = 0 ) in vec3 vertexPosition;
layout( location = 1 ) in vec3 vertexNormals;
layout( location = 2 ) in vec3 vertexTexture;

uniform mat4 g_mvp;
uniform vec2 g_size;

out vec3 coord;
out vec3 normal;
out vec3 blockdata;

out vec2 size;

void main(void) {
    coord = vertexPosition;
    blockdata = vertexTexture;
    normal = vertexNormals;
    size = g_size;

    gl_Position = g_mvp * vec4( vertexPosition.xyz, 1.0);
}
