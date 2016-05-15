#version 400

layout(location = 0) out float fragColor;

void main() {
    fragColor= gl_FragCoord.z;
}
