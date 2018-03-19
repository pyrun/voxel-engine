#version 400

uniform sampler2D sampler;

in vec3 coord;
in vec3 normal;
in vec3 blockdata;

in vec2 size;
in vec3 backgroundcolor;
in vec4 camerapositon;
in vec4 object_positon;

in float alpha_cutoff;

layout( location = 0 ) out vec4 fragColor;

float getFogFactor(float d)
{
    const float FogMax = 530.0;
    const float FogMin = 480.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}
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
    vec4 color = texture2D( sampler, Texture);

    float e = 2.718;

    // Fog
    float z = distance( camerapositon, object_positon);
    //float z = gl_FragCoord.z / gl_FragCoord.w;

    // Alphachannel
    if( alpha_cutoff > 1.0) {
        if(color.a > 0.99)
            discard;
    } else
        if(color.a < alpha_cutoff)
            discard;

    fragColor = mix( color, vec4( backgroundcolor.x, backgroundcolor.y, backgroundcolor.z, 1) , getFogFactor( z*e));
    //fragColor = vec4( 1,0,1,1);
}
