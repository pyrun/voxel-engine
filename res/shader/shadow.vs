#version 440

layout( location = 0 ) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // position
    gl_Position = projection * view * model * vec4( vertexPosition, 1.0);
}
