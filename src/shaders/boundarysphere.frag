#version 330 core

in vec3 EyePosition;

out vec4 FragColor;

const float MAX_DIST = 85.0;

void main()
{
    float farness = (length(EyePosition) / MAX_DIST);
    float closeness = 1 - farness;
    FragColor = vec4(1.0, 1.0, 1.0, min(max(1 - farness, 0), 0.25));
}
