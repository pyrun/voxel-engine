#version 400

uniform sampler2D sampler;

in vec4 coord;
in vec4 normal;
in vec4 blockdata;

in vec2 size;
in vec4 backgroundcolor;
in float alpha_cutoff;

layout( location = 0 ) out vec4 fragColor;

float getFogFactor(float d)
{
    const float FogMax = 13.0;
    const float FogMin = 10.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}
void main() {
    vec2 Texture;
    // Texture Position errechnen
    if(blockdata.x > 0) {
        Texture.x = fract( coord.x);
        Texture.y = fract( coord.z);
    } else {
        Texture.x = fract( coord.x + coord.z );
        Texture.y = fract( -coord.y);
    }
    // Textur aus Tileset waehlen
    Texture.x = (Texture.x + blockdata.y)/size.x;
    Texture.y = (Texture.y + blockdata.z)/size.y;
    vec4 color = texture2D( sampler, Texture);

    float e = 2.718;

    // Fog
    float z = gl_FragCoord.z / gl_FragCoord.w;

    // Alphachannel
    if( alpha_cutoff > 1.0) {
        if(color.a > 0.99)
            discard;
    } else
        if(color.a < alpha_cutoff)
            discard;

    fragColor = mix( color, backgroundcolor, getFogFactor( z/25.0*e));
}
