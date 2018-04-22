#version 440

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gTexture;
layout (location = 3) out vec4 gShadow;

in vec3 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform vec2 tilesize;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture_image;
uniform sampler2D shadow_map;

float ShadowCalculation()
{
    // perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture( shadow_map, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize( Normal);
    vec3 lightDir = normalize( FragPosLightSpace.xyz);

    float bias = 0.00002;

    float shadow = currentDepth-bias> closestDepth  ? 0.0 : 1.0;  
        
    return shadow;
}


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

    float shadow = ShadowCalculation();   

    gShadow = vec4( shadow, shadow, shadow, 1);
}