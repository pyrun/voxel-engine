#version 440

layout( location = 0 ) in vec4 vertexPosition;
layout( location = 1 ) in vec4 vertexNormals;
layout( location = 3 ) in vec4 voxelData;

uniform mat4 g_mvp;
uniform vec2 g_size;
uniform vec4 g_backgroundcolor;
uniform float g_alpha_cutoff;

out vec4 coord;
out vec4 normal;
out vec4 blockdata;

out vec2 size;
out vec4 backgroundcolor;
out float alpha_cutoff;

out vec4 o_sun;

void main(void) {
    coord = vertexPosition;
    blockdata = voxelData;
    normal = vertexNormals;

    size = g_size;
    backgroundcolor = g_backgroundcolor;
    alpha_cutoff = g_alpha_cutoff;

    gl_Position = g_mvp * vec4( vertexPosition.xyz, 1.0);
}
