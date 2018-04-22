#version 440

layout( location = 0 ) in vec3 vertexPosition;
layout( location = 1 ) in vec3 vertexNormals;
layout( location = 2 ) in vec2 vertexTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

void main()
{
    // pos
    vec4 worldPos = model * vec4( vertexPosition, 1.0);
    FragPos = worldPos.xyz; 

    // data
    TexCoords = vertexTexture;

    // normal
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * vertexNormals;

    // shadow
    FragPosLightSpace = lightSpaceMatrix * vec4( FragPos, 1.0);

    // position
    gl_Position = projection * view * worldPos;
}