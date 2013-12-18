#version 330 core

in float FragOffset;
out vec4 FragColor;

uniform vec3 BeamColor;

void main()
{
    FragColor = vec4(BeamColor, FragOffset);

}


