#version 440

layout( location = 0 ) in vec3 vertexPosition;
layout( location = 1 ) in vec3 vertexNormals;
layout( location = 3 ) in vec3 voxelData;

uniform mat4 g_mvp;
uniform mat4 g_viewProjectionMatrixe;
uniform vec2 g_size;
uniform vec3 g_backgroundcolor;
uniform float g_alpha_cutoff;
uniform vec3 g_camerapositon;

out vec3 coord;
out vec3 normal;
out vec3 blockdata;

out vec2 size;
out vec3 backgroundcolor;
out float alpha_cutoff;
out vec4 camerapositon;
out vec4 object_positon;

out vec4 o_sun;

void main(void) {
    coord = vertexPosition;
    blockdata = voxelData;
    normal = vertexNormals;
    camerapositon = g_viewProjectionMatrixe * vec4( g_camerapositon.xyz, 1.0);
    object_positon = g_mvp * vec4( vertexPosition.xyz, 1.0);

    size = g_size;
    backgroundcolor = g_backgroundcolor;
    alpha_cutoff = g_alpha_cutoff;

    gl_Position = g_mvp * vec4( vertexPosition.xyz, 1.0);
}
