#version 400

uniform sampler2D sampler;

in vec3 coord;
in vec3 normal;
in vec3 blockdata;

in vec2 size;

layout( location = 0 ) out vec4 fragColor;

void main() {
    vec2 Texture;

    // Texture Position errechnen
    if(blockdata.z > 0) {
        Texture.x = fract( coord.x);
        Texture.y = fract( coord.z);
    } else {
        Texture.x = fract( coord.x + coord.z );
        Texture.y = fract( -coord.y);
    }

    // Textur aus Tileset waehlen
    Texture.x = (Texture.x + blockdata.x)/size.x;
    Texture.y = (Texture.y + blockdata.y)/size.y;

    fragColor = texture2D( sampler, Texture);
}
