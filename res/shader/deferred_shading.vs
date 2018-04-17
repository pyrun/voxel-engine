#version 440
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexNormals;

out vec2 TexCoords;

void main()
{
    TexCoords = vertexNormals;

    gl_Position = vec4(vertexPosition, 1.0);
}