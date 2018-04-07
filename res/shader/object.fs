#version 410

in vec3 normal;
in vec2 textureCoords;

layout( location = 0 ) out vec4 fragColor;

uniform sampler2D sampler;

void main() {
    fragColor = texture2D( sampler, textureCoords);
}
