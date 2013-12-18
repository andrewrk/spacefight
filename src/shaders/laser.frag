#version 330 core

out vec4 FragColor;

uniform vec3 BeamColor;

void main()
{
    FragColor = vec4(BeamColor, 1.0);
}


