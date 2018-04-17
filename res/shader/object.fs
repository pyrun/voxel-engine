#version 440

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gTexture;
layout (location = 3) out vec4 gShadow;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_image;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4( FragPos, 1);
    // also store the per-fragment normals into the gbuffer
    gNormal = vec4( normalize(Normal), 1);

    gTexture = texture2D( texture_image, TexCoords);
    gTexture.w = 1;

    gShadow = vec4( 1, 1, 1, 1);
}
