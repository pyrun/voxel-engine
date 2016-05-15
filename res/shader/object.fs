#version 400

layout( location = 0 ) out vec4 fragColor;

in vec2 texcoords;

uniform sampler2D sampler;

void main() {
    fragColor = texture2D( sampler, texcoords); //vec4( UV.x, 1, 1, 1); //texture( sampler, UV ).rgb; //vec4 ( 0.1, 0.1, 0.1, 1);
}
