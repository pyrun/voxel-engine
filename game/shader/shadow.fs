#version 440

in vec4 texcoord;
in vec4 texnormal;
in vec4 blockdata;
in vec2 size;

layout(location = 0) out float fragmentdepth;

void main() {
    fragmentdepth = gl_FragCoord.z;
}
