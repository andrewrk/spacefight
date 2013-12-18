#version 330 core

in vec3 VertexPosition;
out vec3 FragPosition;

uniform mat4 MVP; // projection * ModelViewMatrix

void main()
{
    FragPosition = VertexPosition;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}

