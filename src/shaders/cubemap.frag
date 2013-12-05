#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube CubeTexture;

void main(void)
{
    FragColor = texture(CubeTexture, TexCoords);
}
