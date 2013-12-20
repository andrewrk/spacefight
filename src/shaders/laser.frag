#version 330 core

in float FragOffset;
out vec4 FragColor;

uniform vec3 BeamColor;

void main()
{
    FragColor = vec4(mix(vec3(1, 1, 1), BeamColor, FragOffset), FragOffset);

}


