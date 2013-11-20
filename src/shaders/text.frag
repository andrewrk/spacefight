#version 330 core

in vec2 FragTexCoord;
out vec4 FragColor;

uniform sampler2D Tex;
uniform vec4 Color;


void main(void)
{
    FragColor = vec4(1, 1, 1, texture(Tex, FragTexCoord).a) * Color;
}
