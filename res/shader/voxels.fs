#version 440

uniform sampler2D sampler;

in vec3 fragPos;
in vec3 normal;
in vec3 blockdata;

uniform vec2 tilesize;

layout( location = 0 ) out vec4 fragColor;

void main() {
    vec2 Texture;

    // Texture Position errechnen
    if(blockdata.z > 0) {
        Texture.x = fract( fragPos.x);
        Texture.y = fract( fragPos.z);
    } else {
        Texture.x = fract( fragPos.x + fragPos.z );
        Texture.y = fract( -fragPos.y);
    }

    // Textur aus Tileset waehlen
    Texture.x = (Texture.x + blockdata.x)/tilesize.x;
    Texture.y = (Texture.y + blockdata.y)/tilesize.y;

    vec3 color = vec3( 1, 1, 1) * gl_FragCoord.z;

    fragColor = texture( sampler, Texture);
}
