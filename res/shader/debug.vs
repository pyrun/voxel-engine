#version 440
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormals;

uniform mat4 g_mvp;

out vec3 color;

void main()
{
    gl_Position = g_mvp * vec4(vertexPosition, 1.0f);
    color = vertexNormals;
}