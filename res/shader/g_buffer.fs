#version 440

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gTexture;

in vec3 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec2 tilesize;

uniform sampler2D texture_image;

void main()
{
    // Texture position
    vec2 Texture;
    if(TexCoords.z > 0) {
        Texture.x = fract( FragPos.x);
        Texture.y = fract( FragPos.z);
    } else {
        Texture.x = fract( FragPos.x + FragPos.z );
        Texture.y = fract( -FragPos.y);
    }
    Texture.x = (Texture.x + TexCoords.x)/tilesize.x;
    Texture.y = (Texture.y + TexCoords.y)/tilesize.y;

    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4( FragPos, 1);
    // also store the per-fragment normals into the gbuffer
    gNormal = vec4( normalize(Normal), 1);

    gTexture = texture( texture_image, Texture);
}