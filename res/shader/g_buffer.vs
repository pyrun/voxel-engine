#version 440

layout( location = 0 ) in vec3 vertexPosition;
layout( location = 1 ) in vec3 vertexNormals;
layout( location = 2 ) in vec3 vertexTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;
out vec3 FragPos;
out vec3 Normal;

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

    // position
    gl_Position = projection * view * worldPos;
}