#version 330 core

in vec2 FragTexCoord;
out vec4 FragColor;

uniform sampler2D Tex;

void main(void)
{
    FragColor = texture(Tex, FragTexCoord);
}
