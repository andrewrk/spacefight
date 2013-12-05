#version 330 core

in vec3 VertexPosition;
out vec3 TexCoords;
uniform mat4 MVP;

void main(void)
{
    TexCoords = VertexPosition;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
