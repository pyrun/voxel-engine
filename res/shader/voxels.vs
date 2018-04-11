#version 440

layout( location = 0 ) in vec3 vertexPosition;
layout( location = 1 ) in vec3 vertexNormals;
layout( location = 2 ) in vec3 vertexTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 normal;
out vec3 blockdata;

void main(void) {
    // pos
    vec4 worldPos = model * vec4( vertexPosition, 1.0);
    fragPos = worldPos.xyz; 

    // data
    blockdata = vertexTexture;

    // normal
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    normal = normalMatrix * vertexNormals;

    // position
    gl_Position = projection * view * worldPos;
}
