#version 440

//out vec4 FragColor;
layout(location = 0) out float fragmentdepth;

void main()
{
    fragmentdepth = gl_FragCoord.z;//FragColor = vec4( gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
}
