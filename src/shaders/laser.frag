#version 330 core

in vec3 FragPosition;
out vec4 FragColor;

uniform vec3 BeamColor;

void main()
{
    FragColor = vec4(BeamColor, max(min(1-length(FragPosition), 1.0), 0));
}


